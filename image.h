#pragma once

#include "doge_vulkan.h"

#include "device.h"

struct Image {
	VkImage image;
	VkImageView view;
	VkDeviceMemory memory;

	void Destroy() {
		vkFreeMemory(gDevice.VkHandle(), memory, nullptr);
		vkDestroyImageView(gDevice.VkHandle(), view, nullptr);
		vkDestroyImage(gDevice.VkHandle(), image, nullptr);
	}
};
