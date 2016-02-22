
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

std::unique_ptr<char> readFileToCharBuffer(const char* szFilename, size_t& bufferLength) {
	std::ifstream fileSource;
	fileSource.open(szFilename, std::ios::binary | std::ios::in | std::ios::ate);

	if (!fileSource.is_open()) {
		std::cout << "Failed to open source file: " << szFilename << std::endl;
		exit(EXIT_FAILURE);
	}

	bufferLength = static_cast<size_t>(fileSource.tellg());
	fileSource.seekg(0, std::ios::beg);

	std::unique_ptr<char> memblock(new char[bufferLength]);
	fileSource.read(memblock.get(), bufferLength);

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

//int WINAPI WinMain(HINSTANCE hInstance,
//	HINSTANCE hPrevInstance,
//	LPSTR     lpCmdLine,
//	int       nCmdShow)
//{
int main()
{
	/* Initialize the library */
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	printf("glfwVulkanSupported : %d\n", glfwVulkanSupported());

	uint32_t nbRequiredVkExts = 0;
	int nbRequiredVkExtsInt = 0;
	const char** requiredVkExtsNames = nullptr;
	
	requiredVkExtsNames = glfwGetRequiredInstanceExtensions(&nbRequiredVkExtsInt);

	nbRequiredVkExts = static_cast<uint32_t>(nbRequiredVkExtsInt);

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

	// RENDER PASS SETUP

	VkAttachmentDescription attachementDescription;

	attachementDescription.flags = 0;
	attachementDescription.format = gVkSwapchainFormat;
	attachementDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	attachementDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachementDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachementDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachementDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachementDescription.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // TODO check these
	attachementDescription.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // TODO check these

	VkAttachmentReference simpleOutColorAttachementReference;

	simpleOutColorAttachementReference.attachment = 0u;
	simpleOutColorAttachementReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription;

	subpassDescription.flags = 0;
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.inputAttachmentCount = 0u;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.colorAttachmentCount = 1u;
	subpassDescription.pColorAttachments = &simpleOutColorAttachementReference;
	subpassDescription.pResolveAttachments = nullptr;
	subpassDescription.pDepthStencilAttachment = nullptr;
	subpassDescription.preserveAttachmentCount = 0u;
	subpassDescription.pPreserveAttachments = nullptr;

	VkRenderPassCreateInfo renderPassCreateInfo;

	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.pNext = nullptr;
	renderPassCreateInfo.flags = 0;
	renderPassCreateInfo.attachmentCount = 1u;
	renderPassCreateInfo.pAttachments = &attachementDescription;
	renderPassCreateInfo.subpassCount = 1u;
	renderPassCreateInfo.pSubpasses = &subpassDescription;
	renderPassCreateInfo.dependencyCount = 0u;
	renderPassCreateInfo.pDependencies = nullptr;

	VkRenderPass renderPass;

	vkCreateRenderPass(gVkDevice, &renderPassCreateInfo, nullptr, &renderPass);

	// MEMORY INFOS QUERY

	vkGetPhysicalDeviceMemoryProperties(gVkPhysicalDevice, &gVkPhysicalMemProperties);

	VkMemoryPropertyFlags desiredMemTypeFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

	// VERTEX BUFFER SETUP

	VkBuffer vertexBuffer;

	VkBufferCreateInfo vertexBufferCreateInfo;

	const uint32_t vertexCount = 3u;
	const VkDeviceSize sizeofVertex = 3*sizeof(float);

	uint32_t vertexBufferQueueFamilyIndex = 0u;

	vertexBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vertexBufferCreateInfo.pNext = nullptr;
	vertexBufferCreateInfo.flags = 0u;
	vertexBufferCreateInfo.size = vertexCount*sizeofVertex;
	vertexBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	vertexBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	vertexBufferCreateInfo.queueFamilyIndexCount = 1u;
	vertexBufferCreateInfo.pQueueFamilyIndices = &vertexBufferQueueFamilyIndex;

	gVkLastRes = vkCreateBuffer(gVkDevice, &vertexBufferCreateInfo, nullptr, &vertexBuffer);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateBuffer);

	VkMemoryRequirements vertexBufferMemReqs;

	vkGetBufferMemoryRequirements(gVkDevice, vertexBuffer, &vertexBufferMemReqs);

	auto lambdaGetOptimalMemTypeIndex = [](uint32_t desiredMemTypeFlags, uint32_t compatibleTypesBitset) -> uint32_t {
		for (uint32_t i = 0; i < gVkPhysicalMemProperties.memoryTypeCount; ++i) {
			if ((compatibleTypesBitset >> i) & 1u) { // Memory type at index i is supported
				if ((gVkPhysicalMemProperties.memoryTypes[i].propertyFlags & desiredMemTypeFlags) == desiredMemTypeFlags) {
					return i;
				}
			}			
		}

		return UINT32_MAX;
	};

	uint32_t vertexBufferMemTypeIndex = lambdaGetOptimalMemTypeIndex(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, vertexBufferMemReqs.memoryTypeBits);

	if (vertexBufferMemTypeIndex == UINT32_MAX) {
		exit_eprintf("Could not find any suitable memory type for this vertex buffer");
	}

	VkDeviceMemory vertexBufferMemory; // TODO use same memory zone for all buffers

	VkMemoryAllocateInfo vertexBufferMemAllocInfo;

	vertexBufferMemAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vertexBufferMemAllocInfo.pNext = nullptr;
	vertexBufferMemAllocInfo.allocationSize = vertexBufferMemReqs.size;
	vertexBufferMemAllocInfo.memoryTypeIndex = vertexBufferMemTypeIndex;

	gVkLastRes = vkAllocateMemory(gVkDevice, &vertexBufferMemAllocInfo, nullptr, &vertexBufferMemory);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkAllocateMemory);

	gVkLastRes = vkBindBufferMemory(gVkDevice, vertexBuffer, vertexBufferMemory, 0ul);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkBindBufferMemory);

	exit(EXIT_SUCCESS);

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

	exit(EXIT_SUCCESS);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(gGLFWwindow))
	{
		/* Render here */
		uint32_t currentSwapImageIndex = UINT32_MAX;
		gVkLastRes = vkAcquireNextImageKHR(gVkDevice, gVkSwapchain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE, &currentSwapImageIndex);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkAcquireNextImageKHR);

		// Submit present operation to present queue
		VkPresentInfoKHR presentInfo;

		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1u;
		presentInfo.pWaitSemaphores = &imageAcquiredSemaphore; // TODO wait on rendering done once doing true rendering
		presentInfo.swapchainCount = 1u;
		presentInfo.pSwapchains = &gVkSwapchain;
		presentInfo.pImageIndices = &currentSwapImageIndex;
		presentInfo.pResults = nullptr;

		//////////////////////////////////////////////////////////////////

		// DESCRIPTOR SET



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

		vkCreateFramebuffer(gVkDevice, &framebufferCreateInfo, nullptr, &framebuffer);

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

		clearValue.color = { 1.0f, 0.f, 0.f, 1.f };

		VkRenderPassBeginInfo renderPassBeginInfo;

		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.renderPass = renderPass;
		renderPassBeginInfo.framebuffer = framebuffer;
		renderPassBeginInfo.renderArea = { {0, 0}, {WINDOW_WIDTH, WINDOW_HEIGHT} };
		renderPassBeginInfo.clearValueCount = 1u;
		renderPassBeginInfo.pClearValues = &clearValue;

#ifdef DATA_SETUP_DONE
		gVkLastRes = vkBeginCommandBuffer(drawTriangleCmdBuffer, &drawTriangleCmdBufferBeginInfo);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkBeginCommandBuffer);


		/* RECORD DRAW CALL CMDS HERE */
		vkCmdBeginRenderPass(drawTriangleCmdBuffer, &renderpassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		// bind the pipeline
		vkCmdBindPipeline(drawTriangleCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		// bind the descriptor set
		vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, descSetLayout, 1, &descSet, 0, NULL);
		// set the viewport
		vkCmdSetViewport(drawTriangleCmdBuffer, 1, &viewport);
		// draw the triangle
		vkCmdDraw(drawTriangleCmdBuffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(drawTriangleCmdBuffer);


		gVkLastRes = vkEndCommandBuffer(drawTriangleCmdBuffer);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkEndCommandBuffer);
#endif

		//////////////////////////////////////////////////////////////////

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

	vkDeviceWaitIdle(gVkDevice);

	// Destroy children here

	// WARNING, all child ressource MUST be freed before destroying the device
	vkDestroyDevice(gVkDevice, nullptr);

	glfwTerminate();

	vkDestroySurfaceKHR(gVkInstance, gVkSurface, nullptr);

	vkDestroyInstance(gVkInstance, nullptr);

	return 0;
}

