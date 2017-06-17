#include <Common\PrimitiveTypes.h>
#include <Vulkan\include\vulkan\vulkan.h>
#include <exception>
#include <iostream>
#include <vector>
#include <map>
#include <Windows.h>
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR greeting[] = "Hello, World!";

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);

		// Here your application is laid out.  
		// For this introduction, we just print out "Hello, World!"  
		// in the top left corner.  
		//TextOut(hdc,
		//	5, 5,
		//	greeting, _tcslen(greeting));
		// End application specific layout section.  

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

HWND CreateContext()
{
	WNDCLASSEX wcex;

	HINSTANCE instance = GetModuleHandle(nullptr);

	const char *className = "TestRenderingClass";
	const char *title = "Test Rendering";

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = LoadIcon(instance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = className;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

	if (!RegisterClassEx(&wcex))
		throw std::exception("Window class registration failed");

	//if (!RegisterClassEx(&wcex))
	//{
	//	MessageBox(NULL,
	//		_T("Call to RegisterClassEx failed!"),
	//		_T("Win32 Guided Tour"),
	//		NULL);

	//	return 1;
	//}

	const DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

	HWND hWnd = CreateWindow(
		className,
		title,
		style,
		CW_USEDEFAULT, CW_USEDEFAULT,
		500, 100,
		NULL,
		NULL,
		instance,
		NULL
	);

	if (!hWnd)
		throw std::exception("Window creation failed");

	//ShowWindow(hWnd, 0);
	UpdateWindow(hWnd);

	return hWnd;
}

void InitializeVulkan()
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
	auto device = RunJob(CreateLogicalDevice, physicalDevice.Get(), graphicsQueue);
	while (!device.IsFinished());
	std::cout << "device created\n";
}

void main()
{
	auto initializeVulkan = RunJob(InitializeVulkan);

	CreateContext();
	//auto windowHandle = RunJob(CreateContext);
	//while (!windowHandle.IsFinished());
	//std::cout << "windowHandle created\n";

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	//vkDestroyDevice(device.Get(), nullptr);
	//vkDestroyInstance(instance.Get(), nullptr);
}