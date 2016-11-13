#pragma once

#include "doge_vulkan.h"

#include <vector>

#include "instance.h"
#include "physical_device.h"

struct Device {
	static constexpr u32 QUEUE_COUNT = 2;

	void initializeMainDevice(
		u32 enabledLayerCount, const char *const * ppEnabledLayerNames,
		u32 enabledExtensionCount, const char *const * ppEnabledExtensionNames) { // TODO : Query real queue families and properties from physical device and check validity before creating the device

		VkPhysicalDeviceFeatures physicalDeviceFeatures;
		vkGetPhysicalDeviceFeatures(gPhysicalDevice.VkHandle(), &physicalDeviceFeatures);

		VkDeviceQueueCreateInfo aQueueCreateInfo[1];

		float queuePriorities[QUEUE_COUNT] = { 1.0f, 1.0f };

		aQueueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		aQueueCreateInfo[0].pNext = nullptr;
		aQueueCreateInfo[0].flags = 0u; // Ignore, reserved for future use
		aQueueCreateInfo[0].queueFamilyIndex = 0u; // TODO No more hardcoding for my gpu here
		aQueueCreateInfo[0].queueCount = QUEUE_COUNT;
		aQueueCreateInfo[0].pQueuePriorities = queuePriorities;

		VkDeviceCreateInfo deviceCreateInfo;

		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pNext = nullptr;
		deviceCreateInfo.flags = 0u; // Ignore, reserved for future use
		deviceCreateInfo.queueCreateInfoCount = 1u;
		deviceCreateInfo.pQueueCreateInfos = aQueueCreateInfo;
		deviceCreateInfo.enabledLayerCount = enabledLayerCount;
		deviceCreateInfo.ppEnabledLayerNames = ppEnabledLayerNames;
		deviceCreateInfo.enabledExtensionCount = enabledExtensionCount;
		deviceCreateInfo.ppEnabledExtensionNames = ppEnabledExtensionNames;
		deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures; // Currently do not care about fine tuning this, BUT read the doc, some features may incur a useless runtime cost

		gVkLastRes = vkCreateDevice(gPhysicalDevice.VkHandle(), &deviceCreateInfo, nullptr, &mVkDevice);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateDevice);
	}

	VkDevice VkHandle() { return mVkDevice; }

	VkDevice mVkDevice;
};

extern Device gDevice;
