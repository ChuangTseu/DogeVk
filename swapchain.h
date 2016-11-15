#pragma once

#include "doge_vulkan.h"

#include "custom_types.h"

#include <vector>

#include "physical_device.h"
#include "device.h"
#include "surface.h"

struct SwapChain {
	void initializeSwapchain(u32 desiredNumberOfCustomSwapchainImages);

	void initializeSwapchainImageViews();

	void Destroy();

	u32 ImageCount() {
		return mSwapchainImageCount;
	}

	VkFormat Format() {
		return mVkSwapchainFormat;
	}

	VkColorSpaceKHR ColorSpace() {
		return mVkSwapchainColorSpace;
	}

	const std::vector<VkImage>& Images() {
		return maSwapImages;
	}

	const std::vector<VkImageView>& Views() {
		return maSwapViews;
	}

	VkSwapchainKHR VkHandle() { return mVkSwapchain; }
	VkSwapchainKHR* VkHandleAddress() { return &mVkSwapchain; }

	VkSwapchainKHR mVkSwapchain;
	VkFormat mVkSwapchainFormat;
	VkColorSpaceKHR mVkSwapchainColorSpace;

	u32 mSwapchainImageCount;
	std::vector<VkImage> maSwapImages;
	std::vector<VkImageView> maSwapViews;
};

extern SwapChain gSwapChain;
