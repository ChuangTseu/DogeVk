#include "memory.h"

#include "doge_vulkan.h"
#include "device.h"
#include "vulkan_helpers.h"

VkPhysicalDeviceMemoryProperties gVkPhysicalMemProperties;

u32 getOptimalMemTypeIndex(u32 desiredMemTypeFlags, u32 compatibleTypesBitset) {

	for (u32 i = 0; i < gVkPhysicalMemProperties.memoryTypeCount; ++i) {
		if (hasBisSet(compatibleTypesBitset, i)) { // Memory type at index i is supported
			if ((gVkPhysicalMemProperties.memoryTypes[i].propertyFlags & desiredMemTypeFlags) == desiredMemTypeFlags) {
				return i;
			}
		}
	}

	return UINT32_MAX;
};

void allocateNewImageMemory(const VkImage* pImage, VkDeviceMemory* pOutImageMemory, VkMemoryPropertyFlags desiredMemTypeFlags)
{
	VkMemoryRequirements imageMemReqs;

	vkGetImageMemoryRequirements(gDevice.VkHandle(), *pImage, &imageMemReqs);

	u32 imageMemTypeIndex = getOptimalMemTypeIndex(desiredMemTypeFlags, imageMemReqs.memoryTypeBits);

	if (imageMemTypeIndex == UINT32_MAX) {
		exit_eprintf("Could not find any suitable memory type for this image");
	}

	auto imageMemAllocInfo = vkstruct<VkMemoryAllocateInfo>();
	imageMemAllocInfo.allocationSize = imageMemReqs.size;
	imageMemAllocInfo.memoryTypeIndex = imageMemTypeIndex;

	gVkLastRes = vkAllocateMemory(gDevice.VkHandle(), &imageMemAllocInfo, nullptr, pOutImageMemory);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkAllocateMemory);
}
