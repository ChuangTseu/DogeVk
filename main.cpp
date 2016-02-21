
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#if 0
extern "C" {
	_declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
};
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

//constexpr uint32_t mask_22_31 = 0b11111111110000000000000000000000u;
constexpr uint32_t mask_22_31 = 0xFFC00000u;
//constexpr uint32_t mask_21_12 = 0b00000000001111111111000000000000u;
constexpr uint32_t mask_21_12 = 0x3FF000u;
//constexpr uint32_t mask_11_0 = 0b00000000000000000000111111111111u;
constexpr uint32_t mask_11_0 = 0xFFFu;

std::tuple<uint32_t, uint32_t, uint32_t> decomposeApiVersionNumber(uint32_t apiVersionNumber) {
	return std::make_tuple(
		(apiVersionNumber & mask_22_31) >> 22u, 
		(apiVersionNumber & mask_21_12) >> 12u, 
		(apiVersionNumber & mask_11_0)
	);
}

#define eprintf(format, ...) fprintf(stderr, format, ##__VA_ARGS__)

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

//
//#define BOB_COUNT_THEN(vkFn, obj) \
//	uint32_t obj##Count; \
//	gRes = vkFn()
//
//void bob() {
//	VkResult gRes;
//	VkDevice gDevice;
//	VkSwapchainKHR gSwapchain;
//
//	uint32_t swapchainImageCount;
//	gRes = vkGetSwapchainImagesKHR(gDevice, gSwapchain, &swapchainImageCount, nullptr);
//	VKFN_SUCCESS_OR_QUIT(vkGetSwapchainImagesKHR, gRes);
//
//	if (swapchainImageCount < 1) {
//		eprintf("Could not find any image for this swapchain\n");
//		exit(EXIT_FAILURE);
//	}
//
//	std::vector<VkImage> aSwapImages(swapchainImageCount);
//
//	gRes = vkGetSwapchainImagesKHR(gDevice, gSwapchain, &swapchainImageCount, aSwapImages.data());
//	VKFN_SUCCESS_OR_QUIT(vkGetSwapchainImagesKHR, gRes);
//}

//const char* VkResultName[]

const char* appName = "DogeVk";
const uint32_t appVersion = makeApiVersionNumber(0u, 0u, 0u);
const char* engineName = "DogeVk";
const uint32_t engineVersion = makeApiVersionNumber(0u, 0u, 0u);

VkResult gVkLastRes;

#define VKFN_LAST_RES_SUCCESS_OR_QUIT(vkFn) if (gVkLastRes != VK_SUCCESS) { \
	eprintf(#vkFn " error : %d -> %s\n", gVkLastRes, VkEnumName(gVkLastRes)); \
	exit(EXIT_FAILURE); \
}

// GLFW Main Components

GLFWwindow* gGLFWwindow;

// Vulkan Main Components

VkInstance gVkInstance;

VkPhysicalDevice gVkPhysicalDevice; // Assume a single VkPhysicalDevice for this simple demo, ignore others
VkPhysicalDeviceProperties gVkPhysicalDeviceProperties;

VkDevice gVkDevice;

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
		eprintf("Could not find any physical device for this vkInstance\n");
		exit(EXIT_FAILURE);
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
			eprintf("Could not find any queue family properties for this physical device\n");
			exit(EXIT_FAILURE);
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
		eprintf("Could not find any valid physical device\n");
		exit(EXIT_FAILURE);
	}

	printf("Physical device #%u has been chosen\n", chosenPhysicalDeviceIndex);	
}

void initializeMainDevice() { // TODO : Query real queue families and properties from physical device and check validity before creating the device
	VkPhysicalDeviceFeatures physicalDeviceFeatures;
	vkGetPhysicalDeviceFeatures(gVkPhysicalDevice, &physicalDeviceFeatures);

	VkDeviceQueueCreateInfo aQueueCreateInfo[1];

	float queuePriorities[1u] = { 1.0f };

	aQueueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	aQueueCreateInfo[0].pNext = nullptr;
	aQueueCreateInfo[0].flags = 0u; // Ignore, reserved for future use
	aQueueCreateInfo[0].queueFamilyIndex = 0u; // Assuming simple hc case
	aQueueCreateInfo[0].queueCount = 1u; // Only one for now ?
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
		eprintf("Could not find any surface format for this physical device and surface\n");
		exit(EXIT_FAILURE);
	}

	std::vector<VkSurfaceFormatKHR> aSurfaceFormats(surfaceFormatCount);

	gVkLastRes = vkGetPhysicalDeviceSurfaceFormatsKHR(gVkPhysicalDevice, gVkSurface, &surfaceFormatCount, aSurfaceFormats.data());
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkGetPhysicalDeviceSurfaceFormatsKHR);

	if (aSurfaceFormats[0].format == VK_FORMAT_UNDEFINED) {
		eprintf("Surface format #0 is undefined\n");
		exit(EXIT_FAILURE);
	}

	////////

	uint32_t presentModeCount;
	gVkLastRes = vkGetPhysicalDeviceSurfacePresentModesKHR(gVkPhysicalDevice, gVkSurface, &presentModeCount, nullptr);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkGetPhysicalDeviceSurfacePresentModesKHR);

	if (presentModeCount < 1) {
		eprintf("Could not find any persent mode for this physical device and surface\n");
		exit(EXIT_FAILURE);
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
		eprintf("Could not find any image for this swapchain\n");
		exit(EXIT_FAILURE);
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

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	gGLFWwindow = glfwCreateWindow(640, 480, appName, NULL, NULL);
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

	VkSemaphore acquireNextImageSem;
	gVkLastRes = vkCreateSemaphore(gVkDevice, &semCreateInfo, nullptr, &acquireNextImageSem);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkCreateSemaphore);

	uint32_t currentSwapImage = UINT32_MAX;
	gVkLastRes = vkAcquireNextImageKHR(gVkDevice, gVkSwapchain, UINT64_MAX, acquireNextImageSem, VK_NULL_HANDLE, &currentSwapImage);
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkAcquireNextImageKHR);


	VkQueue queue;
	vkGetDeviceQueue(gVkDevice, 0, 0, &queue);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(gGLFWwindow))
	{
		/* Render here */

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

