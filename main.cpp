
#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Already set up by the glfw lib
#if 0
extern "C" { _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001; }; // Force use of discrete GPU with Nvidia Optimus
#endif

#endif

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

#include "camera.h"

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

#include "custom_types.h"
#include "utils.h"
#include "config.h"

#include "doge_vulkan.h"

#include "vulkan_helpers.h"

#include "instance.h"
#include "physical_device.h"
#include "device.h"
#include "glfw_window.h"
#include "surface.h"
#include "swapchain.h"
#include "memory.h"
#include "dedicated_buffer.h"
#include "debug_report.h"
#include "global_descriptor_sets.h"

#include "forward.h"
#include "shadow.h"

float fakeTime = 0.f;

// Vulkan Main Components

VkQueue gVkGraphicsQueue;
VkQueue gVkPresentQueue;

bool isValidPhysicalDeviceHeuristic(VkPhysicalDevice physicalDevice) {
	return glfwGetPhysicalDevicePresentationSupport(gInstance.VkHandle(), physicalDevice, 0) ? true : false;
}

///////// BASIC APP DATA

Camera gCamera;

enum GLFW_WASD {
	UP = GLFW_KEY_W,
	LEFT = GLFW_KEY_A,
	DOWN = GLFW_KEY_S,
	RIGHT = GLFW_KEY_D
};

bool gEscapePressed = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		if (key == GLFW_KEY_ESCAPE) {
			gEscapePressed = true;
			return;
		}

		if (key == GLFW_WASD::UP) {
			//std::cout << "UP\n";
			gCamera.StepAdvance(Camera::EStepAdvance_FORWARD);
		}
		else if (key == GLFW_WASD::LEFT) {
			//std::cout << "LEFT\n";
			gCamera.StepAdvance(Camera::EStepAdvance_LEFT);
		}
		else if (key == GLFW_WASD::DOWN) {
			//std::cout << "DOWN\n";
			gCamera.StepAdvance(Camera::EStepAdvance_BACKWARD);
		}
		else if (key == GLFW_WASD::RIGHT) {
			//std::cout << "RIGHT\n";
			gCamera.StepAdvance(Camera::EStepAdvance_RIGHT);
		}
	}

	if (action == GLFW_RELEASE) {
		if (key == GLFW_WASD::UP) {
			//std::cout << "RELEASE UP\n";
		}
		else if (key == GLFW_WASD::LEFT) {
			//std::cout << "RELEASE LEFT\n";
		}
		else if (key == GLFW_WASD::DOWN) {
			//std::cout << "RELEASE DOWN\n";
		}
		else if (key == GLFW_WASD::RIGHT) {
			//std::cout << "RELEASE RIGHT\n";
		}
	}
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	static double prev_x = xpos, prev_y = ypos;

	double rel_x = xpos - prev_x;
	double rel_y = ypos - prev_y;

	prev_x = xpos;
	prev_y = ypos;

	gCamera.RotateByMouseRelCoords((int)rel_x, (int)-rel_y);
}

/////////////////////////////////////////////// MAIN ///////////////////////////////////////////////

int main()
{
	/* Initialize the glfw library */
	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	printf("glfwVulkanSupported : %d\n", glfwVulkanSupported());

	u32 requiredGlfwVkExtensionsCount = 0;
	const char** requiredGlfwVkExtensionNames = nullptr;
	
	requiredGlfwVkExtensionNames = glfwGetRequiredInstanceExtensions(&requiredGlfwVkExtensionsCount);

	printf("nbRequiredVkExts : %u\n", requiredGlfwVkExtensionsCount);

	for (u32 i = 0; i < requiredGlfwVkExtensionsCount; ++i) {
		printf("%s\n", requiredGlfwVkExtensionNames[i]);
	}

	std::vector<const char*> enabledInstanceLayers;

#if !NDEBUG
	enabledInstanceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif

	std::vector<const char*> enabledInstanceExtensions(requiredGlfwVkExtensionNames, requiredGlfwVkExtensionNames + requiredGlfwVkExtensionsCount);

#if !NDEBUG
	enabledInstanceExtensions.push_back("VK_EXT_debug_report");
#endif

	u32 propertyCount = 0;
	std::vector<VkLayerProperties> layerProperties;
	APPLY_2STEPS_QUERY_TO_COUNT_AND_VECTOR_0PARAM(vkEnumerateInstanceLayerProperties, propertyCount, layerProperties);

	u32 validationLayerExtCount = 0;
	std::vector<VkExtensionProperties> validationLayerExtProperties;
	APPLY_2STEPS_QUERY_TO_COUNT_AND_VECTOR_1PARAM(vkEnumerateInstanceExtensionProperties, "VK_LAYER_LUNARG_standard_validation",
		validationLayerExtCount, validationLayerExtProperties);

	u32 globalLoaderExtCount = 0;
	std::vector<VkExtensionProperties> globalLoaderExtProperties;
	APPLY_2STEPS_QUERY_TO_COUNT_AND_VECTOR_1PARAM(vkEnumerateInstanceExtensionProperties, nullptr,
		globalLoaderExtCount, globalLoaderExtProperties);

	gInstance.initializeMainInstance(
		appName, appVersion, engineName, engineVersion,
		len32(enabledInstanceLayers), enabledInstanceLayers.data(),
		len32(enabledInstanceExtensions), enabledInstanceExtensions.data());

#if !NDEBUG
	gDebugReport.setupDebugReportCallback();
#endif

	gPhysicalDevice.initializeMainPhysicalDevice(isValidPhysicalDeviceHeuristic);

	u32 deviceLayerCount = 0;
	std::vector<VkLayerProperties> deviceLayerProperties;
	APPLY_2STEPS_QUERY_TO_COUNT_AND_VECTOR_1PARAM(vkEnumerateDeviceLayerProperties, gPhysicalDevice.VkHandle(),
		deviceLayerCount, deviceLayerProperties);

	u32 deviceLayerExtCount = 0;
	std::vector<VkExtensionProperties> deviceLayerExtProperties;
	APPLY_2STEPS_QUERY_TO_COUNT_AND_VECTOR_2PARAM(vkEnumerateDeviceExtensionProperties, gPhysicalDevice.VkHandle(), "VK_LAYER_LUNARG_standard_validation",
		deviceLayerExtCount, deviceLayerExtProperties);

	u32 deviceGlobalExtCount = 0;
	std::vector<VkExtensionProperties> deviceGlobalExtProperties;
	APPLY_2STEPS_QUERY_TO_COUNT_AND_VECTOR_2PARAM(vkEnumerateDeviceExtensionProperties, gPhysicalDevice.VkHandle(), "VK_LAYER_LUNARG_standard_validation",
		deviceGlobalExtCount, deviceGlobalExtProperties);

	std::vector<const char*> enabledDeviceExtensions;

	enabledDeviceExtensions.push_back("VK_KHR_swapchain");

	std::vector<const char*> enabledDeviceLayers;

#if !NDEBUG
	enabledDeviceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif

	gDevice.initializeMainDevice(len32(enabledDeviceLayers), enabledDeviceLayers.data(),
		len32(enabledDeviceExtensions), enabledDeviceExtensions.data());

	// TODO These queues should be properly created and retrieved via initializeMainDevice() through some parameterization
	vkGetDeviceQueue(gDevice.VkHandle(), 0, 0, &gVkGraphicsQueue);
	vkGetDeviceQueue(gDevice.VkHandle(), 0, 1, &gVkPresentQueue);

	gWindow.initGLFWWindow(WINDOW_WIDTH, WINDOW_HEIGHT, appName);
	gSurface.initializeSurface();
	gSwapChain.initializeSwapchain(2u);

	// MEMORY INFOS QUERY

	vkGetPhysicalDeviceMemoryProperties(gPhysicalDevice.VkHandle(), &gVkPhysicalMemProperties);

	Scene scene;
	scene.PrepareScene();

	GlobalDescriptorSets::Initialize();

	gShadow.SetupPipeline(scene);
	gForward.SetupPipeline(scene);

	// Init camera
	gCamera.SetPositionProperties(glm::vec3(0.f, 0.f, -4.f), glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f));
	gCamera.SetProjectionProperties(45.f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.f);
	gCamera.UpdateLookAt();

	glm::mat4 initialCameraViewProj = gCamera.GetViewProjection();

	//// SETUP GLFW events handling
	glfwSetKeyCallback(gWindow.GLFWHandle(), key_callback);
	glfwSetInputMode(gWindow.GLFWHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(gWindow.GLFWHandle(), cursor_pos_callback);

	AppConfigCB& appConfigCB = GlobalDescriptorSets::GetMutableCBuffer<AppConfigCB>();
	appConfigCB.g_globalColor = glm::vec3(1.f, 1.f, 0.f);
	GlobalDescriptorSets::CommitCBuffer<AppConfigCB>();

	PerObjectCB& perObjectCB = GlobalDescriptorSets::GetMutableCBuffer<PerObjectCB>();
	perObjectCB.g_world[2] = glm::translate(glm::mat4(1.f), glm::vec3(2.5f, 0.f, 0.f));
	GlobalDescriptorSets::CommitCBuffer<PerObjectCB>();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(gWindow.GLFWHandle()) && !gEscapePressed)
	{
		gShadow.SubmitFrameRender(gVkGraphicsQueue);
		gForward.SubmitFrameRender(gVkGraphicsQueue, gVkPresentQueue);

		/* End of Rendering */

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(16ms);

		/* Swap front and back buffers */

		/* Poll for and process events */
		glfwPollEvents();

		// INNER LOOP RESOURCES FREEING (TODO Move most of them outside, reuse pool, cmd buffer and predeclared per swap image framebuffer)

		gVkLastRes = vkDeviceWaitIdle(gDevice.VkHandle());
		VKFN_LAST_RES_SUCCESS_OR_QUIT(vkDeviceWaitIdle);

		// UPDATE WVP MATRIX

		fakeTime += 0.1666f; // Fake time assuming 60 FPS

		const float timeFactor = 0.1f;

		scene.UpdateScene(fakeTime);

		glm::mat4 world = glm::translate(glm::mat4(1.f), glm::vec3(0.f, glm::sin(fakeTime*timeFactor), 0.f));

		gCamera.UpdateLookAt();

		PerViewPointCB& perViewPointCB = GlobalDescriptorSets::GetMutableCBuffer<PerViewPointCB>();
		perViewPointCB.eyePosition = gCamera.GetPosition();
		perViewPointCB.g_proj[0] = gCamera.GetProjection();
		perViewPointCB.g_view[0] = gCamera.GetView();
		perViewPointCB.g_viewProj[0] = gCamera.GetViewProjection();
		GlobalDescriptorSets::CommitCBuffer<PerViewPointCB>();

		LightsCB& lightsCB = GlobalDescriptorSets::GetMutableCBuffer<LightsCB>();
		lightsCB.g_dirLights[0].m_color = glm::vec3(1.f, 1.f, 1.f);
		lightsCB.g_dirLights[0].m_direction = glm::vec3(glm::sin(fakeTime*timeFactor), 0.f, glm::cos(fakeTime*timeFactor));
		lightsCB.g_lightViewProj[0] = initialCameraViewProj;
		lightsCB.g_numDirLights = 1;
		GlobalDescriptorSets::CommitCBuffer<LightsCB>();

		// Test per frame fragUniformBuffer modification
		static bool bFirstRandomColor = true;
		if (bFirstRandomColor) {
			glm::vec3 triangleColor{ ((float)rand()) / (float)RAND_MAX, ((float)rand()) / (float)RAND_MAX, ((float)rand()) / (float)RAND_MAX };

			PerFrameCB& perFrameCB = GlobalDescriptorSets::GetMutableCBuffer<PerFrameCB>();
			perFrameCB.g_randRgbColor = triangleColor;
			GlobalDescriptorSets::CommitCBuffer<PerFrameCB>();

			bFirstRandomColor = false;
		}
	}

	gVkLastRes = vkDeviceWaitIdle(gDevice.VkHandle());
	VKFN_LAST_RES_SUCCESS_OR_QUIT(vkDeviceWaitIdle);

#if !NDEBUG
	gDebugReport.Destroy();
#endif

	// Destroy children here

	scene.Destroy();
	gForward.Destroy();
	gSwapChain.Destroy();

	vkDestroySwapchainKHR(gDevice.VkHandle(), gSwapChain.VkHandle(), nullptr);

	// WARNING, all child resources MUST be freed before destroying the device
	vkDestroyDevice(gDevice.VkHandle(), nullptr);

	glfwTerminate();
	vkDestroySurfaceKHR(gInstance.VkHandle(), gSurface.VkHandle(), nullptr);

	vkDestroyInstance(gInstance.VkHandle(), nullptr);

	return 0;
}

