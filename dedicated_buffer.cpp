#pragma once

#include "dedicated_buffer.h"

#include "doge_vulkan.h"
#include "vulkan_helpers.h"
#include "memory.h"

#include <assert.h>

void createSimpleBuffer(VkBufferUsageFlags usage, VkDeviceSize size, VkBuffer* pOutBuffer) {
	u32 bufferQueueFamilyIndex = 0u; // TODO as param or implicit if device member with single graphics queue family

	auto bufferCreateInfo = vkstruct<VkBufferCreateInfo>();
	bufferCreateInfo.flags = 0;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 1u;
	bufferCreateInfo.pQueueFamilyIndices = &bufferQueueFamilyIndex;

	gVkLastRes = vkCreateBuffer(gDevice.VkHandle(), &bufferCreateInfo, nullptr, pOutBuffer);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateBuffer);
};

void allocateNewBufferMemory(const VkBuffer* pBuffer, VkDeviceMemory* pOutBufferMemory, VkMemoryPropertyFlags desiredMemTypeFlags) {
	VkMemoryRequirements bufferMemReqs;

	vkGetBufferMemoryRequirements(gDevice.VkHandle(), *pBuffer, &bufferMemReqs);

	u32 bufferMemTypeIndex = getOptimalMemTypeIndex(desiredMemTypeFlags, bufferMemReqs.memoryTypeBits);

	if (bufferMemTypeIndex == UINT32_MAX) {
		exit_eprintf("Could not find any suitable memory type for this buffer");
	}

	auto bufferMemAllocInfo = vkstruct<VkMemoryAllocateInfo>();
	bufferMemAllocInfo.allocationSize = bufferMemReqs.size;
	bufferMemAllocInfo.memoryTypeIndex = bufferMemTypeIndex;

	gVkLastRes = vkAllocateMemory(gDevice.VkHandle(), &bufferMemAllocInfo, nullptr, pOutBufferMemory);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkAllocateMemory);
}


void DedicatedBuffer::Create(VkBufferUsageFlags usage, VkDeviceSize size, HostTranferType eTransferType)
{
	createSimpleBuffer(usage, size, &m_buffer);
	allocateNewBufferMemory(&m_buffer, &m_memory, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

	gVkLastRes = vkBindBufferMemory(gDevice.VkHandle(), m_buffer, m_memory, 0ul);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkBindBufferMemory);
}

void DedicatedBuffer::Destroy()
{
	vkFreeMemory(gDevice.VkHandle(), m_memory, nullptr);
	vkDestroyBuffer(gDevice.VkHandle(), m_buffer, nullptr);
}

void DedicatedBuffer::UploadData(VkDeviceSize offset, VkDeviceSize size, const void* data)
{
	assert(size != 0);
	assert(data != nullptr);

	auto mappedMemoryRange = vkstruct<VkMappedMemoryRange>();
	mappedMemoryRange.memory = m_memory;
	mappedMemoryRange.offset = offset;
	mappedMemoryRange.size = size;

	void* mapAddr;
	gVkLastRes = vkMapMemory(gDevice.VkHandle(), mappedMemoryRange.memory, mappedMemoryRange.offset, mappedMemoryRange.size,
		0, &mapAddr);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkMapMemory);

	memcpy(mapAddr, data, static_cast<size_t>(size));

	gVkLastRes = vkFlushMappedMemoryRanges(gDevice.VkHandle(), 1u, &mappedMemoryRange);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkFlushMappedMemoryRanges);

	vkUnmapMemory(gDevice.VkHandle(), m_memory);
}
