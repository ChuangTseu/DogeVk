#include "doge_vulkan.h"

VkResult gVkLastRes;

std::map<VkResult, const char*> VkResultNameMap{
	TO_ENUM_AND_NAME_PAIR(VK_SUCCESS),
	TO_ENUM_AND_NAME_PAIR(VK_NOT_READY),
	TO_ENUM_AND_NAME_PAIR(VK_TIMEOUT),
	TO_ENUM_AND_NAME_PAIR(VK_EVENT_SET),
	TO_ENUM_AND_NAME_PAIR(VK_EVENT_RESET),
	TO_ENUM_AND_NAME_PAIR(VK_INCOMPLETE),
	TO_ENUM_AND_NAME_PAIR(VK_ERROR_OUT_OF_HOST_MEMORY),
	TO_ENUM_AND_NAME_PAIR(VK_ERROR_OUT_OF_DEVICE_MEMORY),
	TO_ENUM_AND_NAME_PAIR(VK_ERROR_INITIALIZATION_FAILED),
	TO_ENUM_AND_NAME_PAIR(VK_ERROR_DEVICE_LOST),
	TO_ENUM_AND_NAME_PAIR(VK_ERROR_MEMORY_MAP_FAILED),
	TO_ENUM_AND_NAME_PAIR(VK_ERROR_LAYER_NOT_PRESENT),
	TO_ENUM_AND_NAME_PAIR(VK_ERROR_EXTENSION_NOT_PRESENT),
	TO_ENUM_AND_NAME_PAIR(VK_ERROR_FEATURE_NOT_PRESENT),
	TO_ENUM_AND_NAME_PAIR(VK_ERROR_INCOMPATIBLE_DRIVER),
	TO_ENUM_AND_NAME_PAIR(VK_ERROR_TOO_MANY_OBJECTS),
	TO_ENUM_AND_NAME_PAIR(VK_ERROR_FORMAT_NOT_SUPPORTED),
	TO_ENUM_AND_NAME_PAIR(VK_ERROR_SURFACE_LOST_KHR),
	TO_ENUM_AND_NAME_PAIR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR),
	TO_ENUM_AND_NAME_PAIR(VK_SUBOPTIMAL_KHR),
	TO_ENUM_AND_NAME_PAIR(VK_ERROR_OUT_OF_DATE_KHR),
	TO_ENUM_AND_NAME_PAIR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR),
	TO_ENUM_AND_NAME_PAIR(VK_ERROR_VALIDATION_FAILED_EXT),
	TO_ENUM_AND_NAME_PAIR(VK_RESULT_BEGIN_RANGE),
	TO_ENUM_AND_NAME_PAIR(VK_RESULT_END_RANGE),
	TO_ENUM_AND_NAME_PAIR(VK_RESULT_RANGE_SIZE),
	TO_ENUM_AND_NAME_PAIR(VK_RESULT_MAX_ENUM)
};

std::map<VkPhysicalDeviceType, const char*> VkPhysicalDeviceTypeNameMap{
	TO_ENUM_AND_NAME_PAIR(VK_PHYSICAL_DEVICE_TYPE_OTHER),
	TO_ENUM_AND_NAME_PAIR(VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU),
	TO_ENUM_AND_NAME_PAIR(VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU),
	TO_ENUM_AND_NAME_PAIR(VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU),
	TO_ENUM_AND_NAME_PAIR(VK_PHYSICAL_DEVICE_TYPE_CPU),
	TO_ENUM_AND_NAME_PAIR(VK_PHYSICAL_DEVICE_TYPE_BEGIN_RANGE),
	TO_ENUM_AND_NAME_PAIR(VK_PHYSICAL_DEVICE_TYPE_END_RANGE),
	TO_ENUM_AND_NAME_PAIR(VK_PHYSICAL_DEVICE_TYPE_RANGE_SIZE),
	TO_ENUM_AND_NAME_PAIR(VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM)
};
