
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

#define VKFN_SUCCESS_OR_QUIT(vkFn, vkRes) if (vkRes != VK_SUCCESS) { \
	eprintf(#vkFn " error : %d -> %s\n", vkRes, VkEnumName(vkRes)); \
	exit(EXIT_FAILURE); \
}

//const char* VkResultName[]

//int WINAPI WinMain(HINSTANCE hInstance,
//	HINSTANCE hPrevInstance,
//	LPSTR     lpCmdLine,
//	int       nCmdShow)
//{
int main()
{
	printf("glfwVulkanSupported : %d\n", glfwVulkanSupported());

	int nbRequiredVkExts;
	const char** requiredVkExtsNames;
	
	requiredVkExtsNames = glfwGetRequiredInstanceExtensions(&nbRequiredVkExts);

	printf("nbRequiredVkExts : %d\n", nbRequiredVkExts);

	for (int i = 0; i < nbRequiredVkExts; ++i) {
		printf("%s\n", requiredVkExtsNames[i]);
	}


	VkApplicationInfo applicationInfo;

	applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	applicationInfo.pNext = nullptr;
	applicationInfo.pApplicationName = "DogeVk";
	applicationInfo.applicationVersion = makeApiVersionNumber(0u, 0u, 0u);
	applicationInfo.pEngineName = "DogeVk";
	applicationInfo.engineVersion = makeApiVersionNumber(0u, 0u, 0u);
	//applicationInfo.apiVersion = 0; // Ignore API version // WARNING : Nvidia drivers at this time do not respect this apparently
	applicationInfo.apiVersion = makeApiVersionNumber(1u, 0u, 3u);

	VkInstanceCreateInfo instanceCreateInfo;

	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceCreateInfo.pNext = nullptr;
	instanceCreateInfo.flags = 0;
	instanceCreateInfo.pApplicationInfo = &applicationInfo;
	instanceCreateInfo.enabledLayerCount = 0;
	instanceCreateInfo.ppEnabledLayerNames = nullptr;
	instanceCreateInfo.enabledExtensionCount = nbRequiredVkExts;
	instanceCreateInfo.ppEnabledExtensionNames = requiredVkExtsNames;

	VkInstance instance;
	VkResult res;
		
	res = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
	VKFN_SUCCESS_OR_QUIT(vkCreateInstance, res);

	uint32_t nbPhysicalDevices = 0;

	res = vkEnumeratePhysicalDevices(instance, &nbPhysicalDevices, nullptr);
	VKFN_SUCCESS_OR_QUIT(vkEnumeratePhysicalDevices, res);

	if (nbPhysicalDevices < 1) {
		eprintf("Could not find any physical device for this vkInstance\n");
		exit(EXIT_FAILURE);
	}

	std::vector<VkPhysicalDevice> physicalDevices(nbPhysicalDevices);

	res = vkEnumeratePhysicalDevices(instance, &nbPhysicalDevices, physicalDevices.data());
	VKFN_SUCCESS_OR_QUIT(vkEnumeratePhysicalDevices, res);

	printf("nbPhysicalDevices : %d\n", nbPhysicalDevices);

	for (uint32_t i = 0u; i < nbPhysicalDevices; ++i) {
		VkPhysicalDeviceProperties physicalDeviceProperties;

		vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalDeviceProperties);

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

		uint32_t nbQueueFamilyProperties;

		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &nbQueueFamilyProperties, nullptr);

		if (nbQueueFamilyProperties < 1) {
			eprintf("Could not find any queue family properties for this physical device\n");
			exit(EXIT_FAILURE);
		}

		printf("\t nbQueueFamilyProperties : %d\n", nbQueueFamilyProperties);

		std::vector<VkQueueFamilyProperties> aQueueFamilyProperties(nbQueueFamilyProperties);

		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &nbQueueFamilyProperties, aQueueFamilyProperties.data());

		//typedef struct VkQueueFamilyProperties {
		//	VkQueueFlags    queueFlags;
		//	uint32_t        queueCount;
		//	uint32_t        timestampValidBits;
		//	VkExtent3D      minImageTransferGranularity;
		//} VkQueueFamilyProperties;

		for (uint32_t nProp = 0; nProp < nbQueueFamilyProperties; ++nProp) {
			const VkQueueFamilyProperties& prop = aQueueFamilyProperties[nProp];

			printf("\t\t queueFlags : %u\n", prop.queueFlags);
			printf("\t\t queueCount : %u\n", prop.queueCount);
			printf("\t\t timestampValidBits : %u\n", prop.timestampValidBits);

			const VkExtent3D& extent = prop.minImageTransferGranularity;
			printf("\t\t minImageTransferGranularity (width, height, depth) : (%u, %u, %u)\n", extent.width, extent.height, extent.depth);
		}
	}

	VkDeviceCreateInfo deviceCreateInfo;

	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = 0u; // Ignore, reserved for future use
	//deviceCreateInfo.queueCreateInfoCount = ;
	//deviceCreateInfo.pQueueCreateInfos = ;
	//deviceCreateInfo.enabledLayerCount = ;
	//deviceCreateInfo.ppEnabledLayerNames = ;
	//deviceCreateInfo.enabledExtensionCount = ;
	//deviceCreateInfo.ppEnabledExtensionNames = ;
	//deviceCreateInfo.pEnabledFeatures = ;

	//VkDevice device;
	//vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device);



	//if (glfwGetPhysicalDevicePresentationSupport(instance, physical_device, queue_family_index))
	//{
	//	// Queue family supports image presentation
	//}



	//vkCreateInstance()

	return 0;

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to initialize glfw window" << '\n';
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(16ms);

		std::cout << "Rendered" << '\n';

		/* Swap front and back buffers */
		// glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}

