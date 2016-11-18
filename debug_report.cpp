#include "debug_report.h"

VKAPI_ATTR VkBool32 VKAPI_CALL MyDebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, u64 object, size_t location, i32 messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
	if (objectType == VK_DEBUG_REPORT_OBJECT_TYPE_COMMAND_BUFFER_EXT &&
		location == 4658 &&
		messageCode == 6) {
		return VK_FALSE;
	}

	std::cerr <<
		"*** DEBUG REPORT ***\n" <<
		"VkDebugReportFlagsEXT: " << NamesFromVkDebugReportFlagsEXT(flags).c_str() << '\n' <<
		"VkDebugReportObjectTypeEXT: " << VkEnumName(objectType) << '\n' <<
		"object: " << object << '\n' <<
		"location: " << location << '\n' <<
		"messageCode: " << messageCode << '\n' <<
		"pLayerPrefix: " << pLayerPrefix << '\n' <<
		"Message: " << pMessage << "\n\n";

	return VK_FALSE;
}

DebugReport gDebugReport;
