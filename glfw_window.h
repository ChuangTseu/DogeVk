#pragma once

#include "doge_vulkan.h"

#include "custom_types.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "instance.h"

struct Window {
	void initGLFWWindow(int width, int height, const char* windowName) {
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Do not support resizing for now

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		mGLFWwindow = glfwCreateWindow(width, height, windowName, NULL, NULL);
		if (!mGLFWwindow)
		{
			eprintf("Failed to initialize glfw window\n");
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
	}

	GLFWwindow* GLFWHandle() { return mGLFWwindow; }

	GLFWwindow* mGLFWwindow;

};

extern Window gWindow;
