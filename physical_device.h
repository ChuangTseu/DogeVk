#pragma once

#include "doge_vulkan.h"

#include <vector>

#include "instance.h"

struct PhysicalDevice {
	typedef bool(*isValidPhysicalDeviceHeuristicFnType)(VkPhysicalDevice);

	void initializeMainPhysicalDevice(isValidPhysicalDeviceHeuristicFnType isValidPhysicalDeviceFn) {
		u32 physicalDeviceCount = 0;

		gVkLastRes = vkEnumeratePhysicalDevices(gInstance.VkHandle(), &physicalDeviceCount, nullptr);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkEnumeratePhysicalDevices);

		if (physicalDeviceCount < 1) {
			exit_eprintf("Could not find any physical device for this vkInstance\n");
		}

		std::vector<VkPhysicalDevice> aPhysicalDevices(physicalDeviceCount);

		gVkLastRes = vkEnumeratePhysicalDevices(gInstance.VkHandle(), &physicalDeviceCount, aPhysicalDevices.data());
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkEnumeratePhysicalDevices);

		printf("physicalDeviceCount : %d\n", physicalDeviceCount);

		u32 chosenPhysicalDeviceIndex = UINT32_MAX;

		for (u32 i = 0u; i < physicalDeviceCount; ++i) {
			VkPhysicalDeviceProperties physicalDeviceProperties;

			vkGetPhysicalDeviceProperties(aPhysicalDevices[i], &physicalDeviceProperties);

			u32 major, minor, patch;

			printf("Physical device #%u properties: \n", i);

			std::tie(major, minor, patch) = decomposeApiVersionNumber(physicalDeviceProperties.apiVersion);
			printf("\t apiVersion : %u or %u.%u.%u\n", physicalDeviceProperties.apiVersion, major, minor, patch);

			std::tie(major, minor, patch) = decomposeApiVersionNumber(physicalDeviceProperties.driverVersion);
			printf("\t driverVersion : %u or %u.%u.%u\n", physicalDeviceProperties.driverVersion, major, minor, patch);

			printf("\t vendorID : %u \n", physicalDeviceProperties.vendorID);
			printf("\t deviceID : %u \n", physicalDeviceProperties.deviceID);
			printf("\t deviceType : %u -> %s \n", physicalDeviceProperties.deviceType, VkEnumName(physicalDeviceProperties.deviceType));
			printf("\t deviceName : %s \n", physicalDeviceProperties.deviceName);

			// TODO
			physicalDeviceProperties.pipelineCacheUUID;
			physicalDeviceProperties.limits;
			physicalDeviceProperties.sparseProperties;

			printf("\n");

			u32 queueFamilyPropertyCount;

			vkGetPhysicalDeviceQueueFamilyProperties(aPhysicalDevices[i], &queueFamilyPropertyCount, nullptr);

			if (queueFamilyPropertyCount < 1) {
				exit_eprintf("Could not find any queue family properties for this physical device\n");
			}

			printf("\t nbQueueFamilyProperties : %d\n", queueFamilyPropertyCount);

			std::vector<VkQueueFamilyProperties> aQueueFamilyProperties(queueFamilyPropertyCount);

			vkGetPhysicalDeviceQueueFamilyProperties(aPhysicalDevices[i], &queueFamilyPropertyCount, aQueueFamilyProperties.data());

			for (u32 nProp = 0; nProp < queueFamilyPropertyCount; ++nProp) {
				const VkQueueFamilyProperties& prop = aQueueFamilyProperties[nProp];

				printf("\t\t queueFlags : %u\n", prop.queueFlags);
				printf("\t\t queueCount : %u\n", prop.queueCount);
				printf("\t\t timestampValidBits : %u\n", prop.timestampValidBits);

				const VkExtent3D& extent = prop.minImageTransferGranularity;
				printf("\t\t minImageTransferGranularity (width, height, depth) : (%u, %u, %u)\n", extent.width, extent.height, extent.depth);
			}

			if (isValidPhysicalDeviceFn(aPhysicalDevices[i]) && chosenPhysicalDeviceIndex == UINT32_MAX) { // Choose the first valid physical device
				chosenPhysicalDeviceIndex = i;

				mVkPhysicalDevice = aPhysicalDevices[chosenPhysicalDeviceIndex];
				mVkPhysicalDeviceProperties = physicalDeviceProperties;
			}
		}

		if (chosenPhysicalDeviceIndex == UINT32_MAX) {
			exit_eprintf("Could not find any valid physical device\n");
		}

		printf("Physical device #%u has been chosen\n", chosenPhysicalDeviceIndex);
	}

	VkPhysicalDevice VkHandle() { return mVkPhysicalDevice; }

	VkPhysicalDevice mVkPhysicalDevice;
	VkPhysicalDeviceProperties mVkPhysicalDeviceProperties;
};

extern PhysicalDevice gPhysicalDevice;
