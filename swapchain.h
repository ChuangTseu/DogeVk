#pragma once

#include "doge_vulkan.h"

#include "custom_types.h"

#include <vector>

#include "physical_device.h"
#include "device.h"
#include "surface.h"

struct SwapChain {
	void initializeSwapchain(u32 desiredNumberOfCustomSwapchainImages) {
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		gVkLastRes = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gPhysicalDevice.VkHandle(), gSurface.VkHandle(), &surfaceCapabilities);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);

		u32 surfaceFormatCount;
		gVkLastRes = vkGetPhysicalDeviceSurfaceFormatsKHR(gPhysicalDevice.VkHandle(), gSurface.VkHandle(), &surfaceFormatCount, nullptr);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkGetPhysicalDeviceSurfaceFormatsKHR);

		if (surfaceFormatCount < 1) {
			exit_eprintf("Could not find any surface format for this physical device and surface\n");
		}

		std::vector<VkSurfaceFormatKHR> aSurfaceFormats(surfaceFormatCount);

		gVkLastRes = vkGetPhysicalDeviceSurfaceFormatsKHR(gPhysicalDevice.VkHandle(), gSurface.VkHandle(), &surfaceFormatCount, aSurfaceFormats.data());
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkGetPhysicalDeviceSurfaceFormatsKHR);

		if (aSurfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
			exit_eprintf("Surface format #0 is undefined\n");
		}

		////////

		u32 presentModeCount;
		gVkLastRes = vkGetPhysicalDeviceSurfacePresentModesKHR(gPhysicalDevice.VkHandle(), gSurface.VkHandle(), &presentModeCount, nullptr);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkGetPhysicalDeviceSurfacePresentModesKHR);

		if (presentModeCount < 1) {
			exit_eprintf("Could not find any persent mode for this physical device and surface\n");
		}

		std::vector<VkPresentModeKHR> aPresentModes(presentModeCount);

		gVkLastRes = vkGetPhysicalDeviceSurfacePresentModesKHR(gPhysicalDevice.VkHandle(), gSurface.VkHandle(), &presentModeCount, aPresentModes.data());
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkGetPhysicalDeviceSurfacePresentModesKHR);

		//////////

		// If mailbox mode is available, use it, as it is the lowest-latency non-
		// tearing mode.  If not, fall back to FIFO which is always available.
		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
		for (size_t i = 0; i < presentModeCount; ++i) {
			if (aPresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
				swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
		}

		mVkSwapchainFormat = aSurfaceFormats[0].format;
		mVkSwapchainColorSpace = aSurfaceFormats[0].colorSpace;

		VkSwapchainCreateInfoKHR swapchainCreateInfo;

		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.pNext = nullptr;
		swapchainCreateInfo.flags = 0;
		swapchainCreateInfo.surface = gSurface.VkHandle();
		swapchainCreateInfo.minImageCount = surfaceCapabilities.minImageCount + desiredNumberOfCustomSwapchainImages;
		swapchainCreateInfo.imageFormat = mVkSwapchainFormat;
		swapchainCreateInfo.imageColorSpace = mVkSwapchainColorSpace;
		swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
		swapchainCreateInfo.imageArrayLayers = 1u;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount = 0u;
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;
		swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = (surfaceCapabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) ?
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR : VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // TODO Again, find something better than this HC choice
		swapchainCreateInfo.presentMode = swapchainPresentMode;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		gVkLastRes = vkCreateSwapchainKHR(gDevice.VkHandle(), &swapchainCreateInfo, nullptr, &mVkSwapchain);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateSwapchainKHR);

		initializeSwapchainImageViews();
	}

	void initializeSwapchainImageViews() {
		gVkLastRes = vkGetSwapchainImagesKHR(gDevice.VkHandle(), mVkSwapchain, &mSwapchainImageCount, nullptr);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkGetSwapchainImagesKHR);

		if (mSwapchainImageCount < 1) {
			exit_eprintf("Could not find any image for this swapchain\n");
		}

		maSwapImages.resize(mSwapchainImageCount);

		gVkLastRes = vkGetSwapchainImagesKHR(gDevice.VkHandle(), mVkSwapchain, &mSwapchainImageCount, maSwapImages.data());
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkGetSwapchainImagesKHR);

		maSwapViews.resize(mSwapchainImageCount);

		VkImageSubresourceRange imageSubresourceRange;

		imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageSubresourceRange.baseMipLevel = 0;
		imageSubresourceRange.levelCount = 1;
		imageSubresourceRange.baseArrayLayer = 0;
		imageSubresourceRange.layerCount = 1;

		for (u32 i = 0; i < mSwapchainImageCount; ++i) {
			VkImageViewCreateInfo viewCreateInfo;

			viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewCreateInfo.pNext = nullptr;
			viewCreateInfo.flags = 0;
			viewCreateInfo.image = maSwapImages[i];
			viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewCreateInfo.format = mVkSwapchainFormat;
			viewCreateInfo.components = identityComponentMapping;
			viewCreateInfo.subresourceRange = imageSubresourceRange;

			gVkLastRes = vkCreateImageView(gDevice.VkHandle(), &viewCreateInfo, nullptr, &maSwapViews[i]);
			VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateImageView);
		}
	}

	void Destroy() {
		for (VkImageView& swapChainView : maSwapViews) {
			vkDestroyImageView(gDevice.VkHandle(), swapChainView, nullptr);
		}
	}

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
