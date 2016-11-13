#pragma once

#include <vulkan/vulkan.h>

#include <map>

#include "utils.h"

const VkComponentMapping identityComponentMapping = {
	VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };

#define TO_ENUM_AND_NAME_PAIR(e) {e, #e}

extern std::map<VkResult, const char*> VkResultNameMap;

extern std::map<VkPhysicalDeviceType, const char*> VkPhysicalDeviceTypeNameMap;

#define DECLARE_VkEnumName_OVERLOAD(VkEnumType) inline const char* VkEnumName(VkEnumType e) { \
	auto it = VkEnumType##NameMap.find(e); \
	if (it != VkEnumType##NameMap.end()) { \
		return VkEnumType##NameMap.at(e); \
	} \
	else { \
		return "VK_UNKNOWN_RESULT"; \
	} \
}

DECLARE_VkEnumName_OVERLOAD(VkResult)
DECLARE_VkEnumName_OVERLOAD(VkPhysicalDeviceType)

extern VkResult gVkLastRes;

#define VKFN_LAST_RES_SUCCESS_OR_QUIT(vkFn) if (gVkLastRes != VK_SUCCESS) { \
	exit_eprintf(#vkFn " error : %d -> %s\n", gVkLastRes, VkEnumName(gVkLastRes)); \
}

