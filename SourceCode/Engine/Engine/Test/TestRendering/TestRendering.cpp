#pragma once
//#define VULKAN_HPP_NO_EXCEPTIONS


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
#include <Platform\PlatformFile.h>

using namespace vk;
using namespace Engine::Common;
using namespace Engine::Parallelizing;
using namespace Engine::Platform;


cstr vertexShader = "#version 450\n#extension GL_ARB_separate_shader_objects : enable\n\
		out gl_PerVertex {\
		vec4 gl_Position;\
		};\
		layout(location = 0) out vec3 fragColor;\
		vec2 positions[3] = vec2[](\
			vec2(0.0, -0.5),\
			vec2(0.5, 0.5),\
			vec2(-0.5, 0.5));\
		vec3 colors[3] = vec3[](\
			vec3(1.0, 0.0, 0.0),\
			vec3(0.0, 1.0, 0.0),\
			vec3(0.0, 0.0, 1.0));\
		void main() {\
			gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);\
			fragColor = colors[gl_VertexIndex];\
		}";

cstr fragmentShader = "#version 450\n#extension GL_ARB_separate_shader_objects : enable\n\
		layout(location = 0) in vec3 fragColor;\
		layout(location = 0) out vec4 outColor;\
		void main() {\
			outColor = vec4(fragColor, 1.0);\
		}";

struct SwapChainSupportDetails
{
public:
	SurfaceCapabilitiesKHR Capabilities;
	std::vector<SurfaceFormatKHR> Formats;
	std::vector<PresentModeKHR> PresentModes;
};

void CompileShader(cstr Shader, cstr FileName, cstr OutputFileName, char **Data)
{
	PlatformFile::Handle handle = PlatformFile::Open(FileName, PlatformFile::OpenModes::Output);
	PlatformFile::Write(handle, Shader);
	PlatformFile::Close(handle);

	std::string stream;
	stream += "C:/VulkanSDK/1.0.30.0/Bin/glslangValidator.exe -V \"";
	stream += FileName;
	stream += "\" -o \"";
	stream += OutputFileName;
	stream += "\"";

	system(stream.c_str());

	handle = PlatformFile::Open(OutputFileName, PlatformFile::OpenModes::Binary | PlatformFile::OpenModes::Input);
	uint64 size = PlatformFile::Size(handle);
	*Data = new char[size];
	PlatformFile::Read(handle, *Data, size);
	PlatformFile::Close(handle);
}

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

SwapchainKHR CreateSwapchain(PhysicalDevice PhysicalDevice, Device Device, SurfaceKHR SurfaceKHR, SurfaceFormatKHR &Fromat)
{
	SwapChainSupportDetails details;

	details.Capabilities = PhysicalDevice.getSurfaceCapabilitiesKHR(SurfaceKHR);
	details.Formats = PhysicalDevice.getSurfaceFormatsKHR(SurfaceKHR);
	details.PresentModes = PhysicalDevice.getSurfacePresentModesKHR(SurfaceKHR);

	for (SurfaceFormatKHR &availableFormat : details.Formats)
		if (availableFormat.format == Format::eB8G8R8A8Unorm && availableFormat.colorSpace == ColorSpaceKHR::eSrgbNonlinear)
		{
			Fromat = availableFormat;

			break;
		}

	Extent2D actualExtent = { 800, 600 };

	uint32_t imageCount = details.Capabilities.minImageCount + 1;
	//if (details.Capabilities.maxImageCount > 0 && imageCount > details.Capabilities.maxImageCount)
	//	imageCount = details.Capabilities.maxImageCount;

	SwapchainCreateInfoKHR createInfo;
	createInfo.surface = SurfaceKHR;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = Fromat.format;
	createInfo.imageColorSpace = Fromat.colorSpace;
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

	SurfaceFormatKHR format;
	auto swapchain = RunJob(CreateSwapchain, physicalDevice.Get(), device.Get(), surfaceKHR, format);
	while (!swapchain.IsFinished());
	std::cout << "swapchain created\n";

	std::vector<Image> swapChainImages = device.Get().getSwapchainImagesKHR(swapchain.Get());
	std::vector<ImageView> imageViews;

	for (const Image &image : swapChainImages)
	{
		ImageViewCreateInfo createInfo;
		createInfo.image = image;
		createInfo.viewType = ImageViewType::e2D;
		createInfo.format = format.format;
		createInfo.components.r = ComponentSwizzle::eIdentity;
		createInfo.components.g = ComponentSwizzle::eIdentity;
		createInfo.components.b = ComponentSwizzle::eIdentity;
		createInfo.components.a = ComponentSwizzle::eIdentity;
		createInfo.subresourceRange.aspectMask = ImageAspectFlagBits::eColor;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		imageViews.emplace_back(device.Get().createImageView(createInfo));
	}
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

	char *vertexShaderBytes;
	RunJob(CompileShader, vertexShader, "E:/Projects/GameEngine/SourceCode/Engine/Binaries/vertex.vert", "E:/Projects/GameEngine/SourceCode/Engine/Binaries/vertex.spv", &vertexShaderBytes);

	char *fragmentShaderBytes;
	RunJob(CompileShader, fragmentShader, "E:/Projects/GameEngine/SourceCode/Engine/Binaries/fragment.frag", "E:/Projects/GameEngine/SourceCode/Engine/Binaries/fragment.spv", &fragmentShaderBytes);

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