#include "device_buffer_allocator.h"

DeviceBufferAllocator gDeviceBufferAllocator;

DeviceBufferAllocator::DeviceBufferAllocator() {
	std::fill_n(mPerUsageAlignement, USAGE_COUNT, ((VkDeviceSize)~0));
}

u64 closestSuperiorMult(u64 val, u64 mult) {
	u64 closest = mult;
	while (val > closest) {
		closest += mult;
	}
	return closest;
}

DeviceBufferAllocator::Handle DeviceBufferAllocator::GetNewBufferHandle(EDeviceBufferType eDeviceBufferType, size_t size, u32 queueFamilyIndex) {
	Handle handle = {};

	VkBufferUsageFlags usageFlags = AsVkFlag_EGeometryBufferType[eDeviceBufferType];

	CreateBuffer(usageFlags, size, queueFamilyIndex, &handle.buffer);

	VkMemoryRequirements bufferMemReqs;
	vkGetBufferMemoryRequirements(gDevice.VkHandle(), handle.buffer, &bufferMemReqs);

	// TODO DO STAGING OF COURSE ! WE WANT DEVICE LOCAL MEMORY !
	u32 bufferMemTypeIndex = getOptimalMemTypeIndex(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, bufferMemReqs.memoryTypeBits);
	if (bufferMemTypeIndex == UINT32_MAX) {
		exit_eprintf("Could not find any suitable memory type for this buffer");
	}

	if (mPerUsageAlignement[eDeviceBufferType] == ((VkDeviceSize)~0)) {
		mPerUsageAlignement[eDeviceBufferType] = bufferMemReqs.alignment;
	}

	std::vector<Bank>& usageBank = mFamilyBanks[eDeviceBufferType][queueFamilyIndex];

	u32 bankIdx = 0;
	u64 bankOffset = npos64;
	for (; bankIdx < usageBank.size(); ++bankIdx) {
		bankOffset = usageBank[bankIdx].allocator.GetContiguousFreeMemoryOffset(bufferMemReqs.size);
		if (bankOffset != npos64) {
			break;
		}
	}

	if (bankOffset == npos64) {
		Bank bank;
		bank.size = closestSuperiorMult(bufferMemReqs.size, USAGE_MEMORY_BANK_SIZE[eDeviceBufferType]);

		bank.allocator.Init(mPerUsageAlignement[eDeviceBufferType], bank.size);

		auto bufferMemAllocInfo = vkstruct<VkMemoryAllocateInfo>();
		bufferMemAllocInfo.allocationSize = bank.size;
		bufferMemAllocInfo.memoryTypeIndex = bufferMemTypeIndex;

		gVkLastRes = vkAllocateMemory(gDevice.VkHandle(), &bufferMemAllocInfo, nullptr, &bank.memory);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkAllocateMemory);

		bankOffset = bank.allocator.GetContiguousFreeMemoryOffset(bufferMemReqs.size);

		usageBank.push_back(bank);
	}

	gVkLastRes = vkBindBufferMemory(gDevice.VkHandle(), handle.buffer, usageBank[bankIdx].memory, bankOffset);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkBindBufferMemory);

	handle.eGeometryBufferType = eDeviceBufferType;
	handle.bankIdx = bankIdx;
	handle.bankOffset = bankOffset;
	handle.queueFamilyIndex = queueFamilyIndex;

	return handle;
}

void DeviceBufferAllocator::CreateBuffer(VkBufferUsageFlags usage, VkDeviceSize size, u32 queueFamilyIndex, VkBuffer * pOutBuffer) {
	auto bufferCreateInfo = vkstruct<VkBufferCreateInfo>();
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 1u;
	bufferCreateInfo.pQueueFamilyIndices = &queueFamilyIndex;

	gVkLastRes = vkCreateBuffer(gDevice.VkHandle(), &bufferCreateInfo, nullptr, pOutBuffer);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateBuffer);
}

void DeviceBufferAllocator::UploadData(Handle handle, VkDeviceSize offset, VkDeviceSize size, const void* data)
{
	assert(size != 0);
	assert(data != nullptr);

	const Bank& assignedBank = mFamilyBanks[handle.eGeometryBufferType][handle.queueFamilyIndex][handle.bankIdx];

	auto mappedMemoryRange = vkstruct<VkMappedMemoryRange>();
	mappedMemoryRange.memory = assignedBank.memory;
	mappedMemoryRange.offset = handle.bankOffset + offset;
	mappedMemoryRange.size = size;

	void* mapAddr;
	gVkLastRes = vkMapMemory(gDevice.VkHandle(), mappedMemoryRange.memory, mappedMemoryRange.offset, mappedMemoryRange.size,
		0, &mapAddr);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkMapMemory);

	memcpy(mapAddr, data, static_cast<size_t>(size));

	gVkLastRes = vkFlushMappedMemoryRanges(gDevice.VkHandle(), 1u, &mappedMemoryRange);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkFlushMappedMemoryRanges);

	vkUnmapMemory(gDevice.VkHandle(), mappedMemoryRange.memory);
}

