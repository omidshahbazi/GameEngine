#include <Common\PrimitiveTypes.h>
#include <Common\BitwiseUtils.h>
#include <vulkan\vulkan.hpp>
#include <exception>
#include <iostream>
#include <vector>
#include <map>
#include <Windows.h>
#include <Parallelizing\JobManager.h>
#include <Platform\PlatformWindow.h>

using namespace vk;
using namespace Engine::Common;
using namespace Engine::Parallelizing;
using namespace Engine::Platform;

#define CHECK_RESULT(Result) \
if (Result != VK_SUCCESS) \
{ \
	throw std::exception(#Result ## " Failed"); \
}

Instance CreateInstance()
{
	ApplicationInfo info("a", 1, "n", 1, VK_API_VERSION_1_0);

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &info;

	VkInstance instance;

	CHECK_RESULT(vkCreateInstance(&createInfo, nullptr, &instance));

	uint32 extensionsCount = 0;
	CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr));

	std::vector<VkExtensionProperties> extensions(extensionsCount);
	CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensions.data()));

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

VkDevice CreateLogicalDevice(VkInstance Instance, VkPhysicalDevice PhysicalDevice, VkQueue &Queue, PlatformWindow::Handle Surface)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueFamilyCount, queueFamilies.data());

	VkSurfaceKHR surface;
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hinstance = (HINSTANCE)PlatformOS::GetExecutingModuleInstance();
	surfaceCreateInfo.hwnd = (HWND)Surface;
	VkResult result = vkCreateWin32SurfaceKHR(Instance, &surfaceCreateInfo, NULL, &surface);

	int8 i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			break;

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, surface, &presentSupport);

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

int32 WindowProcedure(PlatformWindow::Handle hWnd, uint32 message, uint32* wParam, uint32* lParam)
{
	return PlatformWindow::DefaultProcedure(hWnd, message, wParam, lParam);
}

PlatformWindow::Handle CreateContext()
{
	return PlatformWindow::Create(PlatformOS::GetExecutingModuleInstance(), "TestVulkan", PlatformWindow::Style::OverlappedWindow | PlatformWindow::Style::Visible, WindowProcedure);
}

void InitializeVulkan(PlatformWindow::Handle Surface)
{
	// Using validation layers
	// Using custom allocator
	// Using queues

	auto instance = RunJob(CreateInstance);
	while (!instance.IsFinished());
	std::cout << "instance created\n";

	auto physicalDevice = RunJob(PickPhysicalDevice, instance.Get());
	while (!physicalDevice.IsFinished());
	std::cout << "physicalDevice created\n";

	VkQueue graphicsQueue;
	auto device = RunJob(CreateLogicalDevice, instance.Get(), physicalDevice.Get(), graphicsQueue, Surface);
	while (!device.IsFinished());
	std::cout << "device created\n";
}

void main()
{
	PlatformWindow::Handle surface = CreateContext();


	InitializeVulkan(surface);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//vkDestroyDevice(device.Get(), nullptr);
	//vkDestroyInstance(instance.Get(), nullptr);
}