#pragma once

#include "doge_vulkan.h"

extern VkPhysicalDeviceMemoryProperties gVkPhysicalMemProperties;

u32 getOptimalMemTypeIndex(u32 desiredMemTypeFlags, u32 compatibleTypesBitset);

void allocateNewImageMemory(const VkImage* pImage, VkDeviceMemory* pOutImageMemory, VkMemoryPropertyFlags desiredMemTypeFlags);
