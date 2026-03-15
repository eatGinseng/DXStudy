
#include "VulkanRenderer.h"

int VulkanRenderer::init(GLFWwindow* newWindow)
{
	window = newWindow;

	try {
		CreateInstance();
		CreateSurface();
		GetPhysicalDevice();
		CreateLogicalDevice();
	}
	catch (const std::runtime_error& e) {
		printf("ERROR: %s\n", e.what());
		return EXIT_FAILURE;
	}

	return 0;
}

void VulkanRenderer::CleanUp()
{
	vkDestroyDevice(MainDevice.LogicalDevice, nullptr);
	vkDestroyInstance(instance, nullptr);
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

	// Check instance Extension
	if (!CheckInstanceExtensionSupport(&instanceExtensions))
	{
		throw std::runtime_error("VkInstance does not support required extensions!");
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
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

void VulkanRenderer::CreateLogicalDevice()
{
	// ge tthe queue family indices for the chosen physical device
	QueueFamilyIndices Indices = getQueueFamilies(MainDevice.PhysicalDevice);

	// Queue the logical device needs tos create and info to do so. (Only 1 for now, will add more later.
	VkDeviceQueueCreateInfo QueueCreateInfo = {};
	QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	QueueCreateInfo.queueFamilyIndex = Indices.GraphicsFamily;
	QueueCreateInfo.queueCount = 1;
	float Priority = 1.0f;
	QueueCreateInfo.pQueuePriorities = &Priority;				// multiple queue handling - 1 is highest

	// information to create logical device
	VkDeviceCreateInfo DeviceCreateInfo = {};
	DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	DeviceCreateInfo.queueCreateInfoCount = 1;
	DeviceCreateInfo.pQueueCreateInfos = &QueueCreateInfo;			// List of queue create infos for device
	DeviceCreateInfo.enabledExtensionCount = 0;
	DeviceCreateInfo.ppEnabledExtensionNames = nullptr;

	// shaders for stages for logical device use. store to logical device
	VkPhysicalDeviceFeatures DeviceFeatures = {};
	DeviceCreateInfo.pEnabledFeatures = &DeviceFeatures;			

	// create logical device from physical device
	VkResult result = vkCreateDevice(MainDevice.PhysicalDevice, &DeviceCreateInfo, nullptr, &MainDevice.LogicalDevice);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device.");
	}

	// Queues are created at the same time as the device, so we want handle to queues. store in GraphicsQueue
	vkGetDeviceQueue(MainDevice.LogicalDevice, Indices.GraphicsFamily, 0, &GraphicsQueue);
}

void VulkanRenderer::CreateSurface()
{
	VkWin32SurfaceCreateInfoKHR CreateInfo;
	CreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	CreateInfo.hwnd = glfwGetWin32Window(window);
	CreateInfo.hinstance = GetModuleHandle(nullptr);

	if (vkCreateWin32SurfaceKHR(instance, &CreateInfo, nullptr, &Surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}

}

void VulkanRenderer::GetPhysicalDevice()
{
	// Enumerate Physical device the vkinstance can access
	uint32_t DeviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &DeviceCount, nullptr);

	if (DeviceCount == 0)
	{
		throw std::runtime_error("Can't find GPUs that support vulkan instance.");
	}

	// Get list of physical device
	std::vector<VkPhysicalDevice> DeviceList(DeviceCount);
	vkEnumeratePhysicalDevices(instance, &DeviceCount, DeviceList.data());

	// just pick first device
	for (const auto& device : DeviceList)
	{
		if (CheckDeviceSuitable(device))
		{
			MainDevice.PhysicalDevice = device;
			break;
		}
	}

}

bool VulkanRenderer::CheckInstanceExtensionSupport(std::vector<const char*>* checkExtensions)
{
	uint32_t ExtensionCount = 0;

	// Neet to get numbers of extensions for array
	vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);

	// create a list of vkExtensinoProperties using count
	std::vector<VkExtensionProperties> Extensions(ExtensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, Extensions.data());

	// check if given extensions are in list of avaliable extensions
	for (const auto &checkExtension : *checkExtensions)
	{
		bool hasExtension = false;
		for (const auto& Extension : Extensions)
		{
			// check with name
			if (strcmp(checkExtension, Extension.extensionName))
			{
				hasExtension = true;
				break;
			}
		}

		if (!hasExtension)
		{
			return false;
		}

		return true;
	}


	return false;
}

bool VulkanRenderer::CheckDeviceSuitable(VkPhysicalDevice device)
{

	// information about the device itself
	VkPhysicalDeviceProperties DeviceProeprties;
	vkGetPhysicalDeviceProperties(device, &DeviceProeprties);

	// informatino abount what the device can do
	VkPhysicalDeviceFeatures DeviceFeatures;

	// queue family check
	QueueFamilyIndices indices = getQueueFamilies(device);

	return indices.IsValid();
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &QueueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> QueueFamilyList;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &QueueFamilyCount, QueueFamilyList.data());

	// go through each queue family and check if it has at leat 1 of the required types of queue
	// some queue family is not valid, or has no queue in it
	int i = 0;
	for (const auto& QueueFamily : QueueFamilyList)
	{
		// First, check if queue family has at least 1 queues in that family
		if (QueueFamily.queueCount > 0 && QueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) // Support multiple types of queue
		{
			indices.GraphicsFamily = i;	// if queue family is valid, then get index
		}

		if (indices.IsValid())
		{
			break;
		}
		i++;
	}

	return indices;
}
