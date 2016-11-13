#pragma once

#include "doge_vulkan.h"

#include "custom_types.h"

struct Instance {
	void initializeMainInstance(
		const char* appName, u32 appVersion, const char* engineName, u32 engineVersion,
		u32 enabledLayerCount, const char *const * ppEnabledLayerNames,
		u32 enabledExtensionCount, const char *const * ppEnabledExtensionNames) {

		VkApplicationInfo applicationInfo;

		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pNext = nullptr;
		applicationInfo.pApplicationName = appName;
		applicationInfo.applicationVersion = appVersion;
		applicationInfo.pEngineName = engineName;
		applicationInfo.engineVersion = engineVersion;
		applicationInfo.apiVersion = 0; // Ignore API version // WARNING : Nvidia drivers at this time do not respect this apparently
										//applicationInfo.apiVersion = makeApiVersionNumber(1u, 0u, 4u);

		VkInstanceCreateInfo instanceCreateInfo;

		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = nullptr;
		instanceCreateInfo.flags = 0;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;
		instanceCreateInfo.enabledLayerCount = enabledLayerCount;
		instanceCreateInfo.ppEnabledLayerNames = ppEnabledLayerNames;
		instanceCreateInfo.enabledExtensionCount = enabledExtensionCount;
		instanceCreateInfo.ppEnabledExtensionNames = ppEnabledExtensionNames;

		gVkLastRes = vkCreateInstance(&instanceCreateInfo, nullptr, &mVkInstance);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateInstance);
	}

	VkInstance VkHandle() { return mVkInstance; }

	VkInstance mVkInstance;
};

extern Instance gInstance;
