
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if 0
extern "C" { _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }; // Force use of discrete GPU with Nvidia Optimus
#endif

#endif

//#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <memory>
#include <vector>
#include <map>
#include <tuple>
#include <cassert>

std::unique_ptr<char> readFileToCharBuffer(const char* szFilename, size_t* pOutBufferLength) {
	std::ifstream fileSource;
	fileSource.open(szFilename, std::ios::binary | std::ios::in | std::ios::ate);

	if (!fileSource.is_open()) {
		std::cout << "Failed to open source file: " << szFilename << std::endl;
		exit(EXIT_FAILURE);
	}

	*pOutBufferLength = static_cast<size_t>(fileSource.tellg());
	fileSource.seekg(0, std::ios::beg);

	std::unique_ptr<char> memblock(new char[*pOutBufferLength]);
	fileSource.read(memblock.get(), *pOutBufferLength);

	fileSource.close();

	return memblock;
}

constexpr uint32_t makeApiVersionNumber(uint32_t major, uint32_t minor, uint32_t patch) {
	return (major << 22u) | (minor << 12u) | patch;
}

constexpr uint32_t mask_22_31 = 0xFFC00000u;	// 0b11111111110000000000000000000000u
constexpr uint32_t mask_21_12 = 0x3FF000u;		// 0b00000000001111111111000000000000u
constexpr uint32_t mask_11_0 = 0xFFFu;			// 0b00000000000000000000111111111111u

std::tuple<uint32_t, uint32_t, uint32_t> decomposeApiVersionNumber(uint32_t apiVersionNumber) {
	return std::make_tuple(
		(apiVersionNumber & mask_22_31) >> 22u, 
		(apiVersionNumber & mask_21_12) >> 12u, 
		(apiVersionNumber & mask_11_0)
	);
}

constexpr bool hasBisSet(uint32_t bitsetVal, uint32_t bitIndex) {
	return (bitsetVal >> bitIndex) & 1;
}

#define eprintf(format, ...) fprintf(stderr, format, ##__VA_ARGS__)

#define exit_eprintf(format, ...) fprintf(stderr, format, ##__VA_ARGS__); exit(EXIT_FAILURE)

//constexpr uint32_t makeApiVersionNumber(uint32_t major, uint32_t minor, uint32_t patch) {	
//	return 
//		(major < 1024 && minor < 1024 && patch < 4096) 
//			? (major << 22u) | (minor << 12u) | patch
//			: throw std::logic_error("x must be > 0");
//}

template <int N> int bob() { return N; }

#define TO_ENUM_AND_NAME_PAIR(e) {e, #e}

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

#define DECLARE_VkEnumName_OVERLOAD(VkEnumType) const char* VkEnumName(VkEnumType e) { return VkEnumType##NameMap.at(e); }

DECLARE_VkEnumName_OVERLOAD(VkResult)
DECLARE_VkEnumName_OVERLOAD(VkPhysicalDeviceType)


const char* appName = "DogeVk";
const uint32_t appVersion = makeApiVersionNumber(0u, 0u, 0u);
const char* engineName = "DogeVk";
const uint32_t engineVersion = makeApiVersionNumber(0u, 0u, 0u);

const int WINDOW_WIDTH = 640; 
const int WINDOW_HEIGHT = 480;


#define VKFN_LAST_RES_SUCCESS_OR_QUIT(vkFn) if (gVkLastRes != VK_SUCCESS) { \
	exit_eprintf(#vkFn " error : %d -> %s\n", gVkLastRes, VkEnumName(gVkLastRes)); \
}

// GLFW Main Components

GLFWwindow* gGLFWwindow;

// Vulkan Main Components

VkResult gVkLastRes;

VkInstance gVkInstance;

VkPhysicalDevice gVkPhysicalDevice; // Assume a single VkPhysicalDevice for this simple demo, ignore others
VkPhysicalDeviceProperties gVkPhysicalDeviceProperties;
VkPhysicalDeviceMemoryProperties gVkPhysicalMemProperties;

VkDevice gVkDevice;

const uint32_t queueCount = 2u;
VkQueue gVkGraphicsQueue;
VkQueue gVkPresentQueue;

VkSurfaceKHR gVkSurface;

VkSwapchainKHR gVkSwapchain;
VkFormat gVkSwapchainFormat;
VkColorSpaceKHR gVkSwapchainColorSpace;

uint32_t gSwapchainImageCount;
std::vector<VkImage> gaSwapImages;
std::vector<VkImageView> gaSwapViews;


void initializeMainInstance(uint32_t enabledExtensionCount, const char *const * ppEnabledExtensionNames) {
	VkApplicationInfo applicationInfo;

	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = nullptr;
	applicationInfo.pApplicationName = appName;
	applicationInfo.applicationVersion = appVersion;
	applicationInfo.pEngineName = engineName;
	applicationInfo.engineVersion = engineVersion;
	//applicationInfo.apiVersion = 0; // Ignore API version // WARNING : Nvidia drivers at this time do not respect this apparently
	applicationInfo.apiVersion = makeApiVersionNumber(1u, 0u, 3u);

	VkInstanceCreateInfo instanceCreateInfo;

	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.flags = 0;
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	instanceCreateInfo.enabledLayerCount = 0;
	instanceCreateInfo.ppEnabledLayerNames = nullptr;
	instanceCreateInfo.enabledExtensionCount = enabledExtensionCount;
	instanceCreateInfo.ppEnabledExtensionNames = ppEnabledExtensionNames;

	gVkLastRes = vkCreateInstance(&instanceCreateInfo, nullptr, &gVkInstance);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateInstance);
}

typedef bool (*isValidPhysicalDeviceHeuristicFnType)(VkPhysicalDevice);

void initializeMainPhysicalDevice(isValidPhysicalDeviceHeuristicFnType isValidPhysicalDeviceFn) {
	uint32_t physicalDeviceCount = 0;

	gVkLastRes = vkEnumeratePhysicalDevices(gVkInstance, &physicalDeviceCount, nullptr);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkEnumeratePhysicalDevices);

	if (physicalDeviceCount < 1) {
		exit_eprintf("Could not find any physical device for this vkInstance\n");
	}

	std::vector<VkPhysicalDevice> aPhysicalDevices(physicalDeviceCount);

	gVkLastRes = vkEnumeratePhysicalDevices(gVkInstance, &physicalDeviceCount, aPhysicalDevices.data());
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkEnumeratePhysicalDevices);

	printf("physicalDeviceCount : %d\n", physicalDeviceCount);

	uint32_t chosenPhysicalDeviceIndex = UINT32_MAX;

	for (uint32_t i = 0u; i < physicalDeviceCount; ++i) {
		VkPhysicalDeviceProperties physicalDeviceProperties;

		vkGetPhysicalDeviceProperties(aPhysicalDevices[i], &physicalDeviceProperties);

		uint32_t major, minor, patch;

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

		uint32_t queueFamilyPropertyCount;

		vkGetPhysicalDeviceQueueFamilyProperties(aPhysicalDevices[i], &queueFamilyPropertyCount, nullptr);

		if (queueFamilyPropertyCount < 1) {
			exit_eprintf("Could not find any queue family properties for this physical device\n");
		}

		printf("\t nbQueueFamilyProperties : %d\n", queueFamilyPropertyCount);

		std::vector<VkQueueFamilyProperties> aQueueFamilyProperties(queueFamilyPropertyCount);

		vkGetPhysicalDeviceQueueFamilyProperties(aPhysicalDevices[i], &queueFamilyPropertyCount, aQueueFamilyProperties.data());

		for (uint32_t nProp = 0; nProp < queueFamilyPropertyCount; ++nProp) {
			const VkQueueFamilyProperties& prop = aQueueFamilyProperties[nProp];

			printf("\t\t queueFlags : %u\n", prop.queueFlags);
			printf("\t\t queueCount : %u\n", prop.queueCount);
			printf("\t\t timestampValidBits : %u\n", prop.timestampValidBits);

			const VkExtent3D& extent = prop.minImageTransferGranularity;
			printf("\t\t minImageTransferGranularity (width, height, depth) : (%u, %u, %u)\n", extent.width, extent.height, extent.depth);
		}

		if (isValidPhysicalDeviceFn(aPhysicalDevices[i]) && chosenPhysicalDeviceIndex == UINT32_MAX) { // Choose the first valid physical device
			chosenPhysicalDeviceIndex = i;

			gVkPhysicalDevice = aPhysicalDevices[chosenPhysicalDeviceIndex];
			gVkPhysicalDeviceProperties = physicalDeviceProperties;
		}
	}

	if (chosenPhysicalDeviceIndex == UINT32_MAX) {
		exit_eprintf("Could not find any valid physical device\n");
	}

	printf("Physical device #%u has been chosen\n", chosenPhysicalDeviceIndex);	
}

void initializeMainDevice() { // TODO : Query real queue families and properties from physical device and check validity before creating the device
	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	vkGetPhysicalDeviceFeatures(gVkPhysicalDevice, &physicalDeviceFeatures);

	VkDeviceQueueCreateInfo aQueueCreateInfo[1];

	float queuePriorities[queueCount] = { 1.0f, 1.0f };

	aQueueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	aQueueCreateInfo[0].pNext = nullptr;
	aQueueCreateInfo[0].flags = 0u; // Ignore, reserved for future use
	aQueueCreateInfo[0].queueFamilyIndex = 0u; // Assuming simple hc case
	aQueueCreateInfo[0].queueCount = queueCount; // Only one for now ?
	aQueueCreateInfo[0].pQueuePriorities = queuePriorities;

	VkDeviceCreateInfo deviceCreateInfo;

	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = 0u; // Ignore, reserved for future use
	deviceCreateInfo.queueCreateInfoCount = 1u;
	deviceCreateInfo.pQueueCreateInfos = aQueueCreateInfo;
	deviceCreateInfo.enabledLayerCount = 0u;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	deviceCreateInfo.enabledExtensionCount = 0u;
	deviceCreateInfo.ppEnabledExtensionNames = nullptr;
	deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures; // Currently do not care about fine tuning this, BUT read the doc, some features may incure a useless runtime cost

	gVkLastRes = vkCreateDevice(gVkPhysicalDevice, &deviceCreateInfo, nullptr, &gVkDevice);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateDevice);
}

void initializeSurface() {
#ifdef USE_GLFW
	// Let glfw initialize the KHR Surface for us
	gVkLastRes = glfwCreateWindowSurface(gVkInstance, gGLFWwindow, NULL, &gVkSurface);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(glfwCreateWindowSurface);
#else
	static_assert(false, "Manual surface creation not implemented.");
#endif
}

void initializeSwapchain(uint32_t desiredNumberOfCustomSwapchainImages) {
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	gVkLastRes = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gVkPhysicalDevice, gVkSurface, &surfaceCapabilities);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);

	uint32_t surfaceFormatCount;
	gVkLastRes = vkGetPhysicalDeviceSurfaceFormatsKHR(gVkPhysicalDevice, gVkSurface, &surfaceFormatCount, nullptr);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkGetPhysicalDeviceSurfaceFormatsKHR);

	if (surfaceFormatCount < 1) {
		exit_eprintf("Could not find any surface format for this physical device and surface\n");
	}

	std::vector<VkSurfaceFormatKHR> aSurfaceFormats(surfaceFormatCount);

	gVkLastRes = vkGetPhysicalDeviceSurfaceFormatsKHR(gVkPhysicalDevice, gVkSurface, &surfaceFormatCount, aSurfaceFormats.data());
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkGetPhysicalDeviceSurfaceFormatsKHR);

	if (aSurfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
		exit_eprintf("Surface format #0 is undefined\n");
	}

	////////

	uint32_t presentModeCount;
	gVkLastRes = vkGetPhysicalDeviceSurfacePresentModesKHR(gVkPhysicalDevice, gVkSurface, &presentModeCount, nullptr);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkGetPhysicalDeviceSurfacePresentModesKHR);

	if (presentModeCount < 1) {
		exit_eprintf("Could not find any persent mode for this physical device and surface\n");
	}

	std::vector<VkPresentModeKHR> aPresentModes(presentModeCount);

	gVkLastRes = vkGetPhysicalDeviceSurfacePresentModesKHR(gVkPhysicalDevice, gVkSurface, &presentModeCount, aPresentModes.data());
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

	gVkSwapchainFormat = aSurfaceFormats[0].format;
	gVkSwapchainColorSpace = aSurfaceFormats[0].colorSpace;

	VkSwapchainCreateInfoKHR swapchainCreateInfo;

	swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchainCreateInfo.pNext = nullptr;
	swapchainCreateInfo.flags = 0;
	swapchainCreateInfo.surface = gVkSurface;
	swapchainCreateInfo.minImageCount = surfaceCapabilities.minImageCount + desiredNumberOfCustomSwapchainImages;
	swapchainCreateInfo.imageFormat = gVkSwapchainFormat;
	swapchainCreateInfo.imageColorSpace = gVkSwapchainColorSpace;
	swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
	swapchainCreateInfo.imageArrayLayers = 1u;
	swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	swapchainCreateInfo.queueFamilyIndexCount = 0u;
	swapchainCreateInfo.pQueueFamilyIndices = nullptr;
	swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
	swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
	swapchainCreateInfo.presentMode = swapchainPresentMode;
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	gVkLastRes = vkCreateSwapchainKHR(gVkDevice, &swapchainCreateInfo, nullptr, &gVkSwapchain);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateSwapchainKHR);
}

void initializeSwapchainImageViews() {
	gVkLastRes = vkGetSwapchainImagesKHR(gVkDevice, gVkSwapchain, &gSwapchainImageCount, nullptr);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkGetSwapchainImagesKHR);

	if (gSwapchainImageCount < 1) {
		exit_eprintf("Could not find any image for this swapchain\n");
	}

	gaSwapImages.resize(gSwapchainImageCount);

	gVkLastRes = vkGetSwapchainImagesKHR(gVkDevice, gVkSwapchain, &gSwapchainImageCount, gaSwapImages.data());
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkGetSwapchainImagesKHR);

	gaSwapViews.resize(gSwapchainImageCount);

	const VkComponentMapping identityComponentMapping = { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY };

	VkImageSubresourceRange imageSubresourceRange;

	imageSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageSubresourceRange.baseMipLevel = 0;
	imageSubresourceRange.levelCount = 1;
	imageSubresourceRange.baseArrayLayer = 0;
	imageSubresourceRange.layerCount = 1;

	for (uint32_t i = 0; i < gSwapchainImageCount; ++i) {
		VkImageViewCreateInfo viewCreateInfo;

		viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewCreateInfo.pNext = nullptr;
		viewCreateInfo.flags = 0;
		viewCreateInfo.image = gaSwapImages[i];
		viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewCreateInfo.format = gVkSwapchainFormat;
		viewCreateInfo.components = identityComponentMapping;
		viewCreateInfo.subresourceRange = imageSubresourceRange;

		gVkLastRes = vkCreateImageView(gVkDevice, &viewCreateInfo, nullptr, &gaSwapViews[i]);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateImageView);
	}
}

bool isValidPhysicalDeviceHeuristic(VkPhysicalDevice physicalDevice) {
#ifdef USE_GLFW
	return glfwGetPhysicalDevicePresentationSupport(gVkInstance, physicalDevice, 0) ? true : false;
#else
	return false;
#endif
}

uint32_t getOptimalMemTypeIndex(uint32_t desiredMemTypeFlags, uint32_t compatibleTypesBitset) {
	for (uint32_t i = 0; i < gVkPhysicalMemProperties.memoryTypeCount; ++i) {
		if (hasBisSet(compatibleTypesBitset, i)) { // Memory type at index i is supported
			if ((gVkPhysicalMemProperties.memoryTypes[i].propertyFlags & desiredMemTypeFlags) == desiredMemTypeFlags) {
				return i;
			}
		}
	}

	return UINT32_MAX;
};

void createSimpleBuffer(VkBufferUsageFlags usage, VkDeviceSize size, VkBuffer* pOutBuffer) {
	VkBufferCreateInfo bufferCreateInfo;

	uint32_t bufferQueueFamilyIndex = 0u; // TODO as param or implicit if device member with single graphics queue family

	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = 0u;
	bufferCreateInfo.size = size;
	bufferCreateInfo.usage = usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 1u;
	bufferCreateInfo.pQueueFamilyIndices = &bufferQueueFamilyIndex;

	gVkLastRes = vkCreateBuffer(gVkDevice, &bufferCreateInfo, nullptr, pOutBuffer);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateBuffer);
};

void allocateNewBufferMemory(const VkBuffer* pBuffer, VkDeviceMemory* pOutBufferMemory, VkMemoryPropertyFlags desiredMemTypeFlags) {
	VkMemoryRequirements bufferMemReqs;

	vkGetBufferMemoryRequirements(gVkDevice, *pBuffer, &bufferMemReqs);

	uint32_t bufferMemTypeIndex = getOptimalMemTypeIndex(desiredMemTypeFlags, bufferMemReqs.memoryTypeBits);

	if (bufferMemTypeIndex == UINT32_MAX) {
		exit_eprintf("Could not find any suitable memory type for this buffer");
	}

	VkMemoryAllocateInfo bufferMemAllocInfo;

	bufferMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	bufferMemAllocInfo.pNext = nullptr;
	bufferMemAllocInfo.allocationSize = bufferMemReqs.size;
	bufferMemAllocInfo.memoryTypeIndex = bufferMemTypeIndex;

	gVkLastRes = vkAllocateMemory(gVkDevice, &bufferMemAllocInfo, nullptr, pOutBufferMemory);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkAllocateMemory);
}


int main()
{
	/* Initialize the library */
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	printf("glfwVulkanSupported : %d\n", glfwVulkanSupported());

	uint32_t nbRequiredVkExts = 0;
	const char** requiredVkExtsNames = nullptr;
	
	requiredVkExtsNames = glfwGetRequiredInstanceExtensions(&nbRequiredVkExts);

	printf("nbRequiredVkExts : %u\n", nbRequiredVkExts);

	for (uint32_t i = 0; i < nbRequiredVkExts; ++i) {
		printf("%s\n", requiredVkExtsNames[i]);
	}

	initializeMainInstance(nbRequiredVkExts, requiredVkExtsNames);
	initializeMainPhysicalDevice(isValidPhysicalDeviceHeuristic);

	initializeMainDevice();

	// TODO These queues should be properly created and retrieved via initializeMainDevice() through some parameterization
	vkGetDeviceQueue(gVkDevice, 0, 0, &gVkGraphicsQueue);
	vkGetDeviceQueue(gVkDevice, 0, 1, &gVkPresentQueue);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Do not support resizing for now

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	gGLFWwindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, appName, NULL, NULL);
	if (!gGLFWwindow)
	{
		eprintf("Failed to initialize glfw window\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	initializeSurface();
	initializeSwapchain(2u);

	initializeSwapchainImageViews();

	VkSemaphoreCreateInfo semCreateInfo;

	semCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semCreateInfo.pNext = nullptr;
	semCreateInfo.flags = 0;

	VkSemaphore imageAcquiredSemaphore;
	gVkLastRes = vkCreateSemaphore(gVkDevice, &semCreateInfo, nullptr, &imageAcquiredSemaphore);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateSemaphore);

	VkSemaphore renderingCompleteSemaphore;
	gVkLastRes = vkCreateSemaphore(gVkDevice, &semCreateInfo, nullptr, &renderingCompleteSemaphore);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateSemaphore);

	// RENDER PASS SETUP

	VkAttachmentDescription attachementDesc;

	attachementDesc.flags = 0;
	attachementDesc.format = gVkSwapchainFormat;
	attachementDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	attachementDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachementDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachementDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachementDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachementDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // TODO check these
	attachementDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // TODO check these

	VkAttachmentReference simpleOutColorAttachementReference;

	simpleOutColorAttachementReference.attachment = 0u;
	simpleOutColorAttachementReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDesc;

	subpassDesc.flags = 0;
	subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDesc.inputAttachmentCount = 0u;
	subpassDesc.pInputAttachments = nullptr;
	subpassDesc.colorAttachmentCount = 1u;
	subpassDesc.pColorAttachments = &simpleOutColorAttachementReference;
	subpassDesc.pResolveAttachments = nullptr;
	subpassDesc.pDepthStencilAttachment = nullptr;
	subpassDesc.preserveAttachmentCount = 0u;
	subpassDesc.pPreserveAttachments = nullptr;

	VkRenderPassCreateInfo renderPassCreateInfo;

	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext = nullptr;
	renderPassCreateInfo.flags = 0;
	renderPassCreateInfo.attachmentCount = 1u;
	renderPassCreateInfo.pAttachments = &attachementDesc;
	renderPassCreateInfo.subpassCount = 1u;
	renderPassCreateInfo.pSubpasses = &subpassDesc;
	renderPassCreateInfo.dependencyCount = 0u;
	renderPassCreateInfo.pDependencies = nullptr;

	VkRenderPass renderPass;

	gVkLastRes = vkCreateRenderPass(gVkDevice, &renderPassCreateInfo, nullptr, &renderPass);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateRenderPass);

	// MEMORY INFOS QUERY

	vkGetPhysicalDeviceMemoryProperties(gVkPhysicalDevice, &gVkPhysicalMemProperties);

	VkMemoryPropertyFlags desiredMemTypeFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

	// VERTEX & INDEX BUFFER SETUP

	const uint32_t vertexCount = 3u;
	const uint32_t indexCount = 3u;
	const VkDeviceSize sizeofVertex = 3*sizeof(float);

	float vertices[vertexCount][3] = { { -0.5f, -0.5f, 0.f },{ 0.5f, -0.5f, 0.f },{ 0.f, 0.5f, 0.f } };
	uint32_t indices[indexCount] = { 0u, 1u, 2u };

	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;

	const VkDeviceSize vertexBufferSize = vertexCount*sizeofVertex;
	const VkDeviceSize indexBufferSize = indexCount*sizeof(uint32_t);

	static_assert(vertexBufferSize == sizeof(vertices), "");
	static_assert(indexBufferSize == sizeof(indices), "");

	createSimpleBuffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertexBufferSize, &vertexBuffer);
	createSimpleBuffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indexBufferSize, &indexBuffer);

	VkDeviceMemory vertexBufferMemory; // TODO use same memory zone for all buffers
	VkDeviceMemory indexBufferMemory; // TODO use same memory zone for all buffers

	allocateNewBufferMemory(&vertexBuffer, &vertexBufferMemory, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
	allocateNewBufferMemory(&indexBuffer, &indexBufferMemory, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

	gVkLastRes = vkBindBufferMemory(gVkDevice, vertexBuffer, vertexBufferMemory, 0ul);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkBindBufferMemory);

	gVkLastRes = vkBindBufferMemory(gVkDevice, indexBuffer, indexBufferMemory, 0ul);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkBindBufferMemory);

	VkMappedMemoryRange vertexMappedMemoryRange;

	vertexMappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	vertexMappedMemoryRange.pNext = nullptr;
	vertexMappedMemoryRange.memory = vertexBufferMemory;
	vertexMappedMemoryRange.offset = 0u;
	vertexMappedMemoryRange.size = vertexBufferSize;

	VkMappedMemoryRange indexMappedMemoryRange;

	indexMappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	indexMappedMemoryRange.pNext = nullptr;
	indexMappedMemoryRange.memory = indexBufferMemory;
	indexMappedMemoryRange.offset = 0u;
	indexMappedMemoryRange.size = indexBufferSize;

	void* vertexMapAddr;
	gVkLastRes = vkMapMemory(gVkDevice, vertexMappedMemoryRange.memory, vertexMappedMemoryRange.offset, vertexMappedMemoryRange.size, 
		0, &vertexMapAddr);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkMapMemory);

	void* indexMapAddr;
	gVkLastRes = vkMapMemory(gVkDevice, indexMappedMemoryRange.memory, indexMappedMemoryRange.offset, indexMappedMemoryRange.size,
		0, &indexMapAddr);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkMapMemory);

	// Do Writes
	memcpy(vertexMapAddr, vertices, vertexBufferSize);
	memcpy(indexMapAddr, indices, indexBufferSize);

	VkMappedMemoryRange mappedMemoryRangeArray[] = { vertexMappedMemoryRange, indexMappedMemoryRange };

	gVkLastRes = vkFlushMappedMemoryRanges(gVkDevice, 2u, mappedMemoryRangeArray);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkFlushMappedMemoryRanges);

	vkUnmapMemory(gVkDevice, vertexBufferMemory);
	vkUnmapMemory(gVkDevice, indexBufferMemory);

	// FULL PIPELINE DESCRIPTION

	// PIPELINE STAGES DESCRIPTION

	VkPipelineShaderStageCreateInfo vertexStageCreateInfo;

	VkShaderModule vertexModule;

	VkShaderModuleCreateInfo vertexModuleCreateInfo;

	size_t vertexModuleCodeSize = size_t{ 0 };
	const uint32_t* vertexModuleCode = nullptr;

	std::unique_ptr<char> vertexFileBuffer = readFileToCharBuffer(SHADERS_FOLDER_PATH "simple_vert.spv", &vertexModuleCodeSize);
	vertexModuleCode = (uint32_t*)vertexFileBuffer.get();

	vertexModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	vertexModuleCreateInfo.pNext = nullptr;
	vertexModuleCreateInfo.flags = 0; // reserved for future use
	vertexModuleCreateInfo.codeSize = vertexModuleCodeSize;
	vertexModuleCreateInfo.pCode = vertexModuleCode;

	gVkLastRes = vkCreateShaderModule(gVkDevice, &vertexModuleCreateInfo, nullptr, &vertexModule);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateShaderModule);

	vertexFileBuffer.release();

	//VkSpecializationInfo vertexStageSpecializationInfo;

	//vertexStageSpecializationInfo.mapEntryCount = ;
	//vertexStageSpecializationInfo.pMapEntries = ;
	//vertexStageSpecializationInfo.dataSize = ;
	//vertexStageSpecializationInfo.pData = ;

	vertexStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexStageCreateInfo.pNext = nullptr;
	vertexStageCreateInfo.flags = 0; // reserved for future use
	vertexStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertexStageCreateInfo.module = vertexModule;
	vertexStageCreateInfo.pName = "main";
	vertexStageCreateInfo.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo fragmentStageCreateInfo;

	VkShaderModule fragmentModule;

	VkShaderModuleCreateInfo fragmentModuleCreateInfo;

	size_t fragmentModuleCodeSize = size_t{ 0 };
	const uint32_t* fragmentModuleCode = nullptr;

	std::unique_ptr<char> fragmentFileBuffer = readFileToCharBuffer(SHADERS_FOLDER_PATH "simple_frag.spv", &fragmentModuleCodeSize);
	fragmentModuleCode = (uint32_t*)fragmentFileBuffer.get();

	fragmentModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	fragmentModuleCreateInfo.pNext = nullptr;
	fragmentModuleCreateInfo.flags = 0; // reserved for future use
	fragmentModuleCreateInfo.codeSize = fragmentModuleCodeSize;
	fragmentModuleCreateInfo.pCode = fragmentModuleCode;

	gVkLastRes = vkCreateShaderModule(gVkDevice, &fragmentModuleCreateInfo, nullptr, &fragmentModule);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateShaderModule);

	fragmentFileBuffer.release();

	fragmentStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentStageCreateInfo.pNext = nullptr;
	fragmentStageCreateInfo.flags = 0; // reserved for future use
	fragmentStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragmentStageCreateInfo.module = fragmentModule;
	fragmentStageCreateInfo.pName = "main";
	fragmentStageCreateInfo.pSpecializationInfo = nullptr;

	VkPipelineShaderStageCreateInfo shaderStagesCreateInfo[] = { vertexStageCreateInfo, fragmentStageCreateInfo };

	// PIPELINE VERTEX INPUT DESCRIPTION

	VkVertexInputBindingDescription inputBindingDesc;

	inputBindingDesc.binding = 0u;
	inputBindingDesc.stride = sizeofVertex; // Attributes are interleaved for now
	inputBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // TODO check the real meaning of this

	VkVertexInputAttributeDescription positionInputAttribDesc;
	//VkVertexInputAttributeDescription normalInputAttribDesc{}; // None currently

	positionInputAttribDesc.location = 0u;
	positionInputAttribDesc.binding = 0u;
	positionInputAttribDesc.format = vertexCount == 3 ? VK_FORMAT_R32G32B32_SFLOAT : VK_FORMAT_R32G32B32A32_SFLOAT;
	positionInputAttribDesc.offset = 0u;

	VkVertexInputAttributeDescription vertexInputAttribDescs[] = { positionInputAttribDesc };

	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;

	vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputStateCreateInfo.pNext = nullptr;
	vertexInputStateCreateInfo.flags = 0; // reserved for future use
	vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1u;
	vertexInputStateCreateInfo.pVertexBindingDescriptions = &inputBindingDesc;
	vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 1u;
	vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttribDescs;

	// PIPELINE INPUT ASSEMBLY DESCRIPTION

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;

	inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyStateCreateInfo.pNext = nullptr;
	inputAssemblyStateCreateInfo.flags = 0; // reserved for future use
	inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

	// PIPELINE VIEWPORT DESCRIPTION

	VkPipelineViewportStateCreateInfo viewportStateCreateInfo;

	VkViewport viewport;

	viewport.x = 0.f;
	viewport.y = 0.f;
	viewport.width = float{ WINDOW_WIDTH };
	viewport.height = float{ WINDOW_HEIGHT };
	viewport.minDepth = 0.f;
	viewport.maxDepth = 1.f;

	VkRect2D scissor;

	scissor.offset = { 0, 0 };
	scissor.extent = { uint32_t{ WINDOW_WIDTH }, uint32_t{ WINDOW_HEIGHT } };

	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.pNext = nullptr;
	viewportStateCreateInfo.flags = 0; // reserved for future use
	viewportStateCreateInfo.viewportCount = 1u;
	viewportStateCreateInfo.pViewports = &viewport;
	viewportStateCreateInfo.scissorCount = 1u;
	viewportStateCreateInfo.pScissors = &scissor;

	// PIPELINE RASTERIZATION DESCRIPTION

	VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;

	rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationStateCreateInfo.pNext = nullptr;
	rasterizationStateCreateInfo.flags = 0; // reserved for future use
	rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
	rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
	rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizationStateCreateInfo.depthBiasConstantFactor = 0.f;
	rasterizationStateCreateInfo.depthBiasClamp = 0.0f;
	rasterizationStateCreateInfo.depthBiasSlopeFactor = 0.f;
	rasterizationStateCreateInfo.lineWidth = 0.f;

	// PIPELINE MULTISAMPLE DESCRIPTION

	VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;

	multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleStateCreateInfo.pNext = nullptr;
	multisampleStateCreateInfo.flags = 0; // reserved for future use
	multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleStateCreateInfo.minSampleShading = 0.f; // ignored here (no sampleRateShading)
	multisampleStateCreateInfo.pSampleMask = nullptr;
	multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

	// PIPELINE DEPTHSTENCIL DESCRIPTION

	VkPipelineDepthStencilStateCreateInfo depthstencilStateCreateInfo;

	depthstencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthstencilStateCreateInfo.pNext = nullptr;
	depthstencilStateCreateInfo.flags = 0; // reserved for future use
	depthstencilStateCreateInfo.depthTestEnable = VK_FALSE;
	depthstencilStateCreateInfo.depthWriteEnable = VK_FALSE;
	depthstencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthstencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
	depthstencilStateCreateInfo.stencilTestEnable = VK_FALSE;
	depthstencilStateCreateInfo.front = {};
	depthstencilStateCreateInfo.back = {};
	depthstencilStateCreateInfo.minDepthBounds = 0.f;
	depthstencilStateCreateInfo.maxDepthBounds = 0.f;

	// PIPELINE COLORBLEND DESCRIPTION

	VkPipelineColorBlendAttachmentState singleColorBlendState;

	singleColorBlendState.blendEnable = VK_FALSE;
	singleColorBlendState.srcColorBlendFactor = {};
	singleColorBlendState.dstColorBlendFactor = {};
	singleColorBlendState.colorBlendOp = {};
	singleColorBlendState.srcAlphaBlendFactor = {};
	singleColorBlendState.dstAlphaBlendFactor = {};
	singleColorBlendState.alphaBlendOp = {};
	singleColorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo colorblendStateCreateInfo;

	colorblendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorblendStateCreateInfo.pNext = nullptr;
	colorblendStateCreateInfo.flags = 0; // reserved for future use
	colorblendStateCreateInfo.logicOpEnable = VK_FALSE;
	colorblendStateCreateInfo.logicOp = {};
	colorblendStateCreateInfo.attachmentCount = 1u;
	colorblendStateCreateInfo.pAttachments = &singleColorBlendState;
	colorblendStateCreateInfo.blendConstants; // ignore



	// PIPELINE DYNAMIC DESCRIPTION

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
	(void)dynamicStateCreateInfo;

	// PIPELINE LAYOUT

	VkPipelineLayout pipelineLayout;

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;

	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.pNext = nullptr;
	pipelineLayoutCreateInfo.flags = 0; // reserved for future use
	pipelineLayoutCreateInfo.setLayoutCount = 0u;
	pipelineLayoutCreateInfo.pSetLayouts = nullptr;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0u;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;


	gVkLastRes = vkCreatePipelineLayout(gVkDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreatePipelineLayout);

	// GRAPHICS PIPELINE

	VkPipeline pipeline;

	VkGraphicsPipelineCreateInfo pipelineCreateInfo;

	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.pNext = nullptr;
	pipelineCreateInfo.flags = 0u;
	pipelineCreateInfo.stageCount = 2u;
	pipelineCreateInfo.pStages = shaderStagesCreateInfo;
	pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
	pipelineCreateInfo.pTessellationState = nullptr;
	pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
	pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
	pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
	pipelineCreateInfo.pDepthStencilState = &depthstencilStateCreateInfo;
	pipelineCreateInfo.pColorBlendState = &colorblendStateCreateInfo;
	pipelineCreateInfo.pDynamicState = nullptr; // No dynamic state for this simple pipeline
	pipelineCreateInfo.layout = pipelineLayout;
	pipelineCreateInfo.renderPass = renderPass;
	pipelineCreateInfo.subpass = 0u;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // No pipeline inheritance here
	pipelineCreateInfo.basePipelineIndex = -1;

	gVkLastRes = vkCreateGraphicsPipelines(gVkDevice, VK_NULL_HANDLE, 1u, &pipelineCreateInfo, nullptr, &pipeline);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateGraphicsPipelines);

#if 0
	uint32_t gVkMainDeviceHeapIndex = UINT32_MAX;
	uint32_t gVkMainDeviceMemTypeIndex = UINT32_MAX;

	for (uint32_t i = 0; i < gVkPhysicalMemProperties.memoryTypeCount; ++i) {
		if ((gVkPhysicalMemProperties.memoryTypes[i].propertyFlags & desiredMemTypeFlags) == desiredMemTypeFlags) {
			gVkMainDeviceMemTypeIndex = i;
			gVkMainDeviceHeapIndex = gVkPhysicalMemProperties.memoryTypes[i].heapIndex;

			break;
		}
	}

	if (gVkMainDeviceHeapIndex != UINT32_MAX && gVkMainDeviceMemTypeIndex != UINT32_MAX 
		&& (gVkPhysicalMemProperties.memoryHeaps[gVkMainDeviceHeapIndex].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) != 0) {
		printf("Using main device memory type #%u at heap index #%u\n", gVkMainDeviceMemTypeIndex, gVkMainDeviceHeapIndex);
	}
	else {
		exit_eprintf("Could not find any suitable device memory\n");
	}
#endif

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(gGLFWwindow))
	{
		/* Render here */
		uint32_t currentSwapImageIndex = UINT32_MAX;
		gVkLastRes = vkAcquireNextImageKHR(gVkDevice, gVkSwapchain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE, &currentSwapImageIndex);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkAcquireNextImageKHR);

		//////////////////////////////////////////////////////////////////

		// DESCRIPTOR SET

		// None currently

		// FRAMEBUFFER

		VkFramebufferCreateInfo framebufferCreateInfo;

		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.pNext = nullptr;
		framebufferCreateInfo.flags = 0;
		framebufferCreateInfo.renderPass = renderPass;
		framebufferCreateInfo.attachmentCount = 1u;
		framebufferCreateInfo.pAttachments = &gaSwapViews[currentSwapImageIndex];
		framebufferCreateInfo.width = static_cast<uint32_t>(WINDOW_WIDTH);
		framebufferCreateInfo.height = static_cast<uint32_t>(WINDOW_HEIGHT);
		framebufferCreateInfo.layers = 1u;

		VkFramebuffer framebuffer;

		gVkLastRes = vkCreateFramebuffer(gVkDevice, &framebufferCreateInfo, nullptr, &framebuffer);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateFramebuffer);

		// COMMAND BUFFER (POOL)

		VkCommandPool cmdPool;

		VkCommandPoolCreateInfo cmdPoolCreateInfo;

		cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolCreateInfo.pNext = nullptr;
		cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // TODO create diverse pools with optimal support for either pure static cmd buffers or dynamic ones
		cmdPoolCreateInfo.queueFamilyIndex = 0u;

		gVkLastRes = vkCreateCommandPool(gVkDevice, &cmdPoolCreateInfo, nullptr, &cmdPool);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateCommandPool);

		// COMMAND BUFFER

		VkCommandBuffer drawTriangleCmdBuffer;

		VkCommandBufferAllocateInfo drawTriangleCmdBufferAllocInfo;

		drawTriangleCmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		drawTriangleCmdBufferAllocInfo.pNext = nullptr;
		drawTriangleCmdBufferAllocInfo.commandPool = cmdPool;
		drawTriangleCmdBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		drawTriangleCmdBufferAllocInfo.commandBufferCount = 1;

		gVkLastRes = vkAllocateCommandBuffers(gVkDevice, &drawTriangleCmdBufferAllocInfo, &drawTriangleCmdBuffer);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkAllocateCommandBuffers);

		VkCommandBufferBeginInfo drawTriangleCmdBufferBeginInfo;

		drawTriangleCmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		drawTriangleCmdBufferBeginInfo.pNext = nullptr;
		drawTriangleCmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // TODO learn more about flags in case of a more elaborate use
		drawTriangleCmdBufferBeginInfo.pInheritanceInfo = nullptr;

		// RENDER PASS LAUNCH

		VkClearValue clearValue;

		clearValue.color = { 0.3f, 0.3f, 0.3f, 1.f };

		VkRenderPassBeginInfo renderPassBeginInfo;

		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = framebuffer;
		renderPassBeginInfo.renderArea = { {0, 0}, {WINDOW_WIDTH, WINDOW_HEIGHT} };
		renderPassBeginInfo.clearValueCount = 1u;
		renderPassBeginInfo.pClearValues = &clearValue;

		// REGISTER COMMAND BUFFER

		gVkLastRes = vkBeginCommandBuffer(drawTriangleCmdBuffer, &drawTriangleCmdBufferBeginInfo);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkBeginCommandBuffer);


		/* RECORD DRAW CALL CMDS HERE */
		vkCmdBeginRenderPass(drawTriangleCmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(drawTriangleCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

		// TODO activate this once descriptors are needed
		//vkCmdBindDescriptorSets(drawTriangleCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0u, 0u, nullptr, 0u, nullptr);

		vkCmdBindIndexBuffer(drawTriangleCmdBuffer, indexBuffer, 0u, VK_INDEX_TYPE_UINT32);

		VkDeviceSize vertexBufferOffsetArray[] = { 0 };
		vkCmdBindVertexBuffers(drawTriangleCmdBuffer, 0u, 1u, &vertexBuffer, vertexBufferOffsetArray);

		vkCmdDrawIndexed(drawTriangleCmdBuffer, 3u, 1u, 0u, 0u, 0u);

		vkCmdEndRenderPass(drawTriangleCmdBuffer);


		gVkLastRes = vkEndCommandBuffer(drawTriangleCmdBuffer);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkEndCommandBuffer);

		//////////////////////////////////////////////////////////////////

		VkSubmitInfo submitInfo;

		VkPipelineStageFlags waitSwapchainAcquiredAtStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 1u;
		submitInfo.pWaitSemaphores = &imageAcquiredSemaphore;
		submitInfo.pWaitDstStageMask = &waitSwapchainAcquiredAtStage;
		submitInfo.commandBufferCount = 1u;
		submitInfo.pCommandBuffers = &drawTriangleCmdBuffer;
		submitInfo.signalSemaphoreCount = 1u;
		submitInfo.pSignalSemaphores = &renderingCompleteSemaphore;

		gVkLastRes = vkQueueSubmit(gVkGraphicsQueue, 1u, &submitInfo, VK_NULL_HANDLE);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkQueueSubmit);

		// Submit present operation to present queue
		VkPresentInfoKHR presentInfo;

		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1u;
		presentInfo.pWaitSemaphores = &renderingCompleteSemaphore;
		presentInfo.swapchainCount = 1u;
		presentInfo.pSwapchains = &gVkSwapchain;
		presentInfo.pImageIndices = &currentSwapImageIndex;
		presentInfo.pResults = nullptr;


		gVkLastRes = vkQueuePresentKHR(gVkPresentQueue, &presentInfo);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkQueuePresentKHR);

		/* End of Rendering */

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(16ms);

		//printf("Rendered\n");

		/* Swap front and back buffers */

		/* Poll for and process events */
		glfwPollEvents();
	}

	gVkLastRes = vkDeviceWaitIdle(gVkDevice);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkDeviceWaitIdle);

	// Destroy children here

	// WARNING, all child ressource MUST be freed before destroying the device
	vkDestroyDevice(gVkDevice, nullptr);

	glfwTerminate();

	vkDestroySurfaceKHR(gVkInstance, gVkSurface, nullptr);

	vkDestroyInstance(gVkInstance, nullptr);

	return 0;
}

