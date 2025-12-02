
#include "VulkanRenderer.h"

int VulkanRenderer::init(GLFWwindow* newWindow)
{
	window = newWindow;

	try {
		CreateInstance();
	}
	catch (const std::runtime_error& e) {
		printf("ERROR: %s\n", e.what());
		return EXIT_FAILURE;
	}

	return 0;
}

void VulkanRenderer::CreateInstance()
{
	// program version, debug info, etc .. 
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan app";					// Custom name for application
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);		// Custom version of the application
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_4;

	// Creation information for vulkan instance
	VkInstanceCreateInfo createInfo = {};
	// structure type. how to handle this structure?
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	// extra info for extended version of this info.
	//createInfo.pnext = ;
	// flag is int, we use bit for flags.
	// createInfo.flags = ;
	// p means pointer.
	createInfo.pApplicationInfo = &appInfo;

	// Create list to hold instance extensions
	std::vector<const char*> instanceExtensions = std::vector<const char*>();
	
	uint32_t glfwExtensionCount = 0;		// GLFW may require multiple extensions
	const char** glfwExtensions;		// Extensions passed as array of cstrings.

	// Get GLFW Extensions
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// Add GLFW Extensions to list of extensions
	for (int i = 0; i < glfwExtensionCount; i++)
	{
		instanceExtensions.push_back(glfwExtensions[i]);
	}

	createInfo.enabledExtensionCount = /*static_cast<uint32_t>*/(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();

	// validation layer
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
	
	// AllocatorCallback : callback to allocator when creation.
	// VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

	
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create a Vulkan instance.");
	}
	
}
