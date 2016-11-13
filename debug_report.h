#pragma once

#include "doge_vulkan.h"

#include "custom_types.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>

#include "instance.h"

#define APPEND_FLAG_NAME_TO_STRING_IF_PRESENT(flags, flagBitDefine, flagsString) \
	if (flags & flagBitDefine) flagsString += #flagBitDefine;

#define APPEND_FLAG_NAME_TO_STRING_IF_PRESENT_INTERMEDIATE(flags, flagBitDefine, flagsString) \
	if (flags & flagBitDefine) (flagsString.empty() ? flagsString : (flagsString += " | ")) += #flagBitDefine;

inline std::string NamesFromVkDebugReportFlagsEXT(VkDebugReportFlagsEXT flags) {
	std::string reportTypeStr;

	APPEND_FLAG_NAME_TO_STRING_IF_PRESENT(flags, VK_DEBUG_REPORT_INFORMATION_BIT_EXT, reportTypeStr);
	APPEND_FLAG_NAME_TO_STRING_IF_PRESENT_INTERMEDIATE(flags, VK_DEBUG_REPORT_WARNING_BIT_EXT, reportTypeStr);
	APPEND_FLAG_NAME_TO_STRING_IF_PRESENT_INTERMEDIATE(flags, VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT, reportTypeStr);
	APPEND_FLAG_NAME_TO_STRING_IF_PRESENT_INTERMEDIATE(flags, VK_DEBUG_REPORT_ERROR_BIT_EXT, reportTypeStr);
	APPEND_FLAG_NAME_TO_STRING_IF_PRESENT_INTERMEDIATE(flags, VK_DEBUG_REPORT_DEBUG_BIT_EXT, reportTypeStr);

	return reportTypeStr;
}

const std::map<VkDebugReportObjectTypeEXT, const char*> VkDebugReportObjectTypeEXTNameMap{
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_UNKNOWN_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_INSTANCE_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_PHYSICAL_DEVICE_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_QUEUE_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_SEMAPHORE_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_FENCE_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_DEVICE_MEMORY_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_EVENT_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_QUERY_POOL_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_VIEW_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_VIEW_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_SHADER_MODULE_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_CACHE_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_LAYOUT_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_RENDER_PASS_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_PIPELINE_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_SAMPLER_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_POOL_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_DESCRIPTOR_SET_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_FRAMEBUFFER_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_POOL_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_SURFACE_KHR_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_SWAPCHAIN_KHR_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_DEBUG_REPORT_OBJECT_TYPE_DEBUG_REPORT_EXT)
};

DECLARE_VkEnumName_OVERLOAD(VkDebugReportObjectTypeEXT)

VKAPI_ATTR VkBool32 VKAPI_CALL MyDebugReportCallback(
	VkDebugReportFlagsEXT       flags,
	VkDebugReportObjectTypeEXT  objectType,
	u64							object,
	size_t                      location,
	i32							messageCode,
	const char*                 pLayerPrefix,
	const char*                 pMessage,
	void*                       pUserData);

struct DebugReport {
	void setupDebugReportCallback() {
		/* Load VK_EXT_debug_report entry points in debug builds */
		pfn_vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(gInstance.VkHandle(), "vkCreateDebugReportCallbackEXT");
		pfn_vkDebugReportMessageEXT = (PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(gInstance.VkHandle(), "vkDebugReportMessageEXT");
		pfn_vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(gInstance.VkHandle(), "vkDestroyDebugReportCallbackEXT");

		/* Setup callback creation information */
		VkDebugReportCallbackCreateInfoEXT callbackCreateInfo;
		callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
		callbackCreateInfo.pNext = nullptr;
		//callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
		//	VK_DEBUG_REPORT_WARNING_BIT_EXT |
		//	VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
			VK_DEBUG_REPORT_WARNING_BIT_EXT;
		callbackCreateInfo.pfnCallback = &MyDebugReportCallback;
		callbackCreateInfo.pUserData = nullptr;

		/* Register the callback */
		gVkLastRes = pfn_vkCreateDebugReportCallbackEXT(gInstance.VkHandle(), &callbackCreateInfo, nullptr, &gDebugReportCallback);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(pfn_vkCreateDebugReportCallbackEXT);
	}

	void Destroy() {
		pfn_vkDestroyDebugReportCallbackEXT(gInstance.VkHandle(), gDebugReportCallback, nullptr);
	}

	// Debug report
	VkDebugReportCallbackEXT gDebugReportCallback;

	// Dynamically loaded debug function pointers
	PFN_vkCreateDebugReportCallbackEXT pfn_vkCreateDebugReportCallbackEXT;
	PFN_vkDebugReportMessageEXT pfn_vkDebugReportMessageEXT;
	PFN_vkDestroyDebugReportCallbackEXT pfn_vkDestroyDebugReportCallbackEXT;
};

extern DebugReport gDebugReport;
