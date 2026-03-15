#pragma once

#define GLFW_INCLUDE_VULKAN
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "Utilities.h"

#include <stdexcept>
#include <vector>

class VulkanRenderer
{
public:

	int init(GLFWwindow* newWindow);

	void CleanUp();

private:
	 GLFWwindow* window;

	 // Vulkan Components
	 VkInstance instance;
	 struct {
		 VkPhysicalDevice PhysicalDevice;
		 VkDevice LogicalDevice;
	 } MainDevice;

	 VkQueue GraphicsQueue;
	 VkSurfaceKHR Surface;

	 // - Create Functions
	 void CreateInstance();
	 void CreateLogicalDevice();
	 void CreateSurface();

	 // Get Function. Get reference to device.
	 void GetPhysicalDevice();

	 bool CheckInstanceExtensionSupport(std::vector<const char*>* checkExtensions);
	 bool CheckDeviceSuitable(VkPhysicalDevice device);

	 // getter functions
	 QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);

};

