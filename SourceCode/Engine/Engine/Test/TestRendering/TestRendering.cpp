//#define VULKAN_HPP_NO_EXCEPTIONS


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
#include <algorithm>

using namespace vk;
using namespace Engine::Common;
using namespace Engine::Parallelizing;
using namespace Engine::Platform;

struct SwapChainSupportDetails
{
public:
	SurfaceCapabilitiesKHR Capabilities;
	std::vector<SurfaceFormatKHR> Formats;
	std::vector<PresentModeKHR> PresentModes;
};

Instance CreateInstance()
{
	ApplicationInfo info("a", 1, "n", 1, VK_API_VERSION_1_0);

	std::vector<ExtensionProperties> extensions = enumerateInstanceExtensionProperties(nullptr);
	std::vector<const char*> extensionsName;
	for (ExtensionProperties &e : extensions)
		extensionsName.emplace_back(e.extensionName);

	InstanceCreateInfo createInfo;
	createInfo.pApplicationInfo = &info;
	createInfo.enabledExtensionCount = extensionsName.size();
	createInfo.ppEnabledExtensionNames = extensionsName.data();

	Instance instance = createInstance(createInfo);

	return instance;
}

bool IsDeviceSuitable(const PhysicalDeviceProperties &Properties, const PhysicalDeviceFeatures &Features)
{
	// it's just a sample
	return (Properties.deviceType == PhysicalDeviceType::eDiscreteGpu && Features.geometryShader);
}

int32 RateDeviceSuitability(const PhysicalDeviceProperties &Properties, const PhysicalDeviceFeatures &Features)
{
	// it's just a sample

	int32 score = 0;

	// Discrete GPUs have a significant performance advantage
	if (Properties.deviceType == PhysicalDeviceType::eDiscreteGpu)
		score += 1000;

	// Maximum possible size of textures affects graphics quality
	score += Properties.limits.maxImageDimension2D;

	// Application can't function without geometry shaders
	if (!Features.geometryShader)
		return 0;

	return score;
}

PhysicalDevice PickPhysicalDevice(Instance Instance)
{
	PhysicalDevice physicalDevice = nullptr;

	std::vector<PhysicalDevice> devices = Instance.enumeratePhysicalDevices();

	if (devices.size() == 0)
		throw std::exception("Failed to find any GPU with Vulkan support");

	if (devices.size() == 1)
		return devices[0];

	std::multimap<int, PhysicalDevice> candidates;

	for (const PhysicalDevice& device : devices)
	{
		PhysicalDeviceProperties deviceProperties = device.getProperties();

		PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

		int32 score = RateDeviceSuitability(deviceProperties, deviceFeatures);
		candidates.insert(std::make_pair(score, device));
	}

	// Check if the best candidate is suitable at all
	if (candidates.rbegin()->first != 0)
		return candidates.rbegin()->second;

	throw std::runtime_error("failed to find a suitable GPU!");
}

Device CreateLogicalDevice(Instance Instance, PhysicalDevice PhysicalDevice, Queue &Queue, SurfaceKHR &SurfaceKHR, PlatformWindow::Handle Surface)
{
	std::vector<QueueFamilyProperties> queueFamilies = PhysicalDevice.getQueueFamilyProperties();

	Win32SurfaceCreateInfoKHR surfaceCreateInfo;
	surfaceCreateInfo.hinstance = (HINSTANCE)PlatformOS::GetExecutingModuleInstance();
	surfaceCreateInfo.hwnd = (HWND)Surface;

	SurfaceKHR = Instance.createWin32SurfaceKHR(surfaceCreateInfo, nullptr);

	int8 i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & QueueFlagBits::eGraphics)
			break;

		Bool32 presentSupport = PhysicalDevice.getSurfaceSupportKHR(i, SurfaceKHR);

		++i;
	}

	DeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.queueFamilyIndex = i;
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;


	DeviceCreateInfo createInfo = {};

	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;

	PhysicalDeviceFeatures deviceFeatures = {};
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;
	createInfo.enabledLayerCount = 0;

	Device device = PhysicalDevice.createDevice(createInfo, nullptr);

	Queue = device.getQueue(queueCreateInfo.queueFamilyIndex, 0);

	return device;
}

SwapchainKHR CreateSwapchain(PhysicalDevice PhysicalDevice, Device Device, SurfaceKHR SurfaceKHR, Queue &Queue)
{
	SwapChainSupportDetails details;

	details.Capabilities = PhysicalDevice.getSurfaceCapabilitiesKHR(SurfaceKHR);
	details.Formats = PhysicalDevice.getSurfaceFormatsKHR(SurfaceKHR);
	details.PresentModes = PhysicalDevice.getSurfacePresentModesKHR(SurfaceKHR);

	SurfaceFormatKHR format;

	for (SurfaceFormatKHR &availableFormat : details.Formats)
		if (availableFormat.format == Format::eR8G8B8A8Unorm && availableFormat.colorSpace == ColorSpaceKHR::eSrgbNonlinear)
		{
			format = availableFormat;

			break;
		}

	Extent2D actualExtent = { 800,600 };

	uint32_t imageCount = details.Capabilities.minImageCount + 1;
	if (details.Capabilities.maxImageCount > 0 && imageCount > details.Capabilities.maxImageCount)
		imageCount = details.Capabilities.maxImageCount;

	SwapchainCreateInfoKHR createInfo;
	createInfo.surface = SurfaceKHR;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = format.format;
	createInfo.imageColorSpace = format.colorSpace;
	createInfo.imageExtent = actualExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = ImageUsageFlagBits::eColorAttachment;
	createInfo.imageSharingMode = SharingMode::eExclusive;
	createInfo.queueFamilyIndexCount = 0; // Optional
	createInfo.pQueueFamilyIndices = nullptr; // Optional
	createInfo.preTransform = details.Capabilities.currentTransform;
	createInfo.compositeAlpha = CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo.presentMode = PresentModeKHR::eImmediate;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	return Device.createSwapchainKHR(createInfo);
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

	Queue graphicsQueue;
	SurfaceKHR surfaceKHR;
	auto device = RunJob(CreateLogicalDevice, instance.Get(), physicalDevice.Get(), graphicsQueue, surfaceKHR, Surface);
	while (!device.IsFinished());
	std::cout << "device created\n";
	
	auto swapchain = RunJob(CreateSwapchain, physicalDevice.Get(), device.Get(), surfaceKHR, graphicsQueue);
	while (!swapchain.IsFinished());
	std::cout << "swapchain created\n";
}

int32 WindowProcedure(PlatformWindow::Handle hWnd, uint32 message, uint32* wParam, uint32* lParam)
{
	return PlatformWindow::DefaultProcedure(hWnd, message, wParam, lParam);
}

PlatformWindow::Handle CreateContext()
{
	PlatformWindow::Handle handle = PlatformWindow::Create(PlatformOS::GetExecutingModuleInstance(), "TestVulkan", PlatformWindow::Style::OverlappedWindow | PlatformWindow::Style::Visible, WindowProcedure);
	PlatformWindow::SetSize(handle, 500, 500);
	return handle;
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