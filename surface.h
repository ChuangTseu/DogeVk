#pragma once

#include "doge_vulkan.h"

#include "custom_types.h"

#include "glfw_window.h"

struct Surface {
	void initializeSurface() {
		// Let glfw initialize the KHR Surface for us
		gVkLastRes = glfwCreateWindowSurface(gInstance.VkHandle(), gWindow.GLFWHandle(), NULL, &mVkSurface);
		VKFN_LAST_RES_SUCCESS_OR_QUIT(glfwCreateWindowSurface);
	}

	VkSurfaceKHR VkHandle() { return mVkSurface; }

	VkSurfaceKHR mVkSurface;
};

extern Surface gSurface;
