#include <Common\PrimitiveTypes.h>
#include <Vulkan\include\vulkan\vulkan.h>
#include <exception>
#include <iostream>
#include <vector>
#include <map>
#include <Parallelizing\JobManager.h>

using namespace Engine::Common;
using namespace Engine::Parallelizing;

#define CHECK_RESULT(Result) \
if (Result != VK_SUCCESS) \
{ \
	throw std::exception(#Result ## " Failed"); \
}

VkInstance CreateInstance()
{
	VkApplicationInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	info.pApplicationName = "a";
	info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	info.pEngineName = "n";
	info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	info.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &info;

	VkInstance instance;

	CHECK_RESULT(vkCreateInstance(&createInfo, nullptr, &instance));

	//uint32 extensionsCount = 0;
	//CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr));

	//std::vector<VkExtensionProperties> extensions(extensionsCount);
	//CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensions.data()));

	return instance;
}

bool IsDeviceSuitable(const VkPhysicalDeviceProperties &Properties, const VkPhysicalDeviceFeatures &Features)
{
	// it's just a sample
	return (Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && Features.geometryShader);
}

int32 RateDeviceSuitability(const VkPhysicalDeviceProperties &Properties, const VkPhysicalDeviceFeatures &Features)
{
	// it's just a sample

	int32 score = 0;

	// Discrete GPUs have a significant performance advantage
	if (Properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		score += 1000;

	// Maximum possible size of textures affects graphics quality
	score += Properties.limits.maxImageDimension2D;

	// Application can't function without geometry shaders
	if (!Features.geometryShader)
		return 0;

	return score;
}

VkPhysicalDevice PickPhysicalDevice(VkInstance Instance)
{
	VkPhysicalDevice physicalDevice = nullptr;

	uint32 deviceCount = 0;
	vkEnumeratePhysicalDevices(Instance, &deviceCount, nullptr);

	if (deviceCount == 0)
		throw std::exception("Failed to find any GPU with Vulkan support");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(Instance, &deviceCount, devices.data());

	if (devices.size() == 1)
		return devices[0];

	std::multimap<int, VkPhysicalDevice> candidates;

	for (const VkPhysicalDevice& device : devices)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		int32 score = RateDeviceSuitability(deviceProperties, deviceFeatures);
		candidates.insert(std::make_pair(score, device));
	}

	// Check if the best candidate is suitable at all
	if (candidates.rbegin()->first != 0)
		return candidates.rbegin()->second;

	throw std::runtime_error("failed to find a suitable GPU!");
}

VkDevice CreateLogicalDevice(VkPhysicalDevice PhysicalDevice, VkQueue &Queue)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueFamilyCount, queueFamilies.data());

	int8 i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			break;

		++i;
	}

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = i;
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;


	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;

	VkPhysicalDeviceFeatures deviceFeatures = {};
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;
	createInfo.enabledLayerCount = 0;

	VkDevice device;
	CHECK_RESULT(vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &device));

	vkGetDeviceQueue(device, queueCreateInfo.queueFamilyIndex, 0, &Queue);

	return device;
}

void main()
{
	// Using validation layers
	// Using custom allocator
	// Using queues

	Job<VkDevice> initialization = RunJob([]()
	{
		VkInstance instance = CreateInstance();

		VkPhysicalDevice physicalDevice = PickPhysicalDevice(instance);

		VkQueue graphicsQueue;
		VkDevice device = CreateLogicalDevice(physicalDevice, graphicsQueue);

		return device;
	});

	while (!initialization.IsFinished())
	{
		std::cout << "Waiting to creation";
	}

	std::cout << "Creation completed";


	//vkDestroyDevice(device, nullptr);
	//vkDestroyInstance(instance, nullptr);
}