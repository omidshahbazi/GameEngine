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
#include <Containers\Strings.h>
#include <Debugging\Debug.h>

using namespace vk;
using namespace Engine::Common;
using namespace Engine::Parallelizing;
using namespace Engine::Platform;
using namespace Engine::Containers;
using namespace Engine::Debugging;


cstr vertexShader = "#version 450\n#extension GL_ARB_separate_shader_objects : enable\n\
		layout(location = 0) in vec2 inPosition;\
		layout(location = 1) in vec3 inColor;\
		layout(location = 0) out vec3 fragColor;\
		out gl_PerVertex{\
			vec4 gl_Position;\
		};\
		void main() {\
			gl_Position = vec4(inPosition, 0.0, 1.0);\
			fragColor = inColor;\
		}";

cstr fragmentShader = "#version 450\n#extension GL_ARB_separate_shader_objects : enable\n\
		layout(location = 0) in vec3 fragColor;\
		layout(location = 0) out vec4 outColor;\
		void main() {\
			outColor = vec4(fragColor, 1.0);\
		}";

template<typename R> class JobBaseInternal
{
public:
	JobBaseInternal(void) :
		m_Info(nullptr)
	{ }

	JobBaseInternal(const R &Result) :
		m_Result(Result)
	{
	}

	bool IsFinished(void) const
	{
		return true;
	}

protected:
	R m_Result;
};

template<typename R> class JobInternal : public JobBaseInternal<R>
{
public:
	JobInternal(void) :
		JobBaseInternal<R>()
	{ }

	JobInternal(R) :
		JobBaseInternal<R>(Info)
	{ }

	const R &Get(void) const
	{
		return m_Result;
	}
};

template<> class JobInternal<void> : public JobBaseInternal<void>
{
};

template<typename Function, typename ...Parameters, typename ResultType = std::result_of<Function(Parameters...)>::type, typename ReturnType = JobInternal<ResultType>> ReturnType RunJobInternal(Function &&Function, Parameters&&... Arguments)
{
	return JobInternal<R>(std::bind(Function, Arguments...)());
}

struct SwapChainSupportDetails
{
public:
	SurfaceCapabilitiesKHR Capabilities;
	std::vector<SurfaceFormatKHR> Formats;
	std::vector<PresentModeKHR> PresentModes;
};

struct Vertex
{
public:
	static VertexInputBindingDescription GetBindingDescription()
	{
		VertexInputBindingDescription desc;

		desc.binding = 0;
		desc.stride = sizeof(Vertex);
		desc.inputRate = VertexInputRate::eVertex;

		return desc;
	}

	static std::array<VertexInputAttributeDescription, 2> GetAttributeDescriptions()
	{
		std::array<VertexInputAttributeDescription, 2> attrDesc;

		attrDesc[0].binding = 0;
		attrDesc[0].location = 0;
		attrDesc[0].format = Format::eR32G32Sfloat;
		attrDesc[0].offset = offsetof(Vertex, Position);

		attrDesc[1].binding = 0;
		attrDesc[1].location = 1;
		attrDesc[1].format = Format::eR32G32B32Sfloat;
		attrDesc[1].offset = offsetof(Vertex, Color);

		return attrDesc;
	}

public:
	float Position[2];
	float Color[3];
};

Job<ShaderModule> vertexShaderModule;
Job<ShaderModule> fragmentShaderModule;
Semaphore imageAvailableSemaphore;
Semaphore renderFinishedSemaphore;
std::function<void(void)> DrawFrame;

const std::vector<Vertex> vertices =
{
	{ { 0.0f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
	{ { 0.5f, 0.5f },{ 0.0f, 1.0f, 0.0f } },
	{ { -0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } }
};

ShaderModule CompileShader(cstr Shader, cstr FileName, cstr OutputFileName, Device Device)
{
	char8 path[260];
	PlatformOS::GetExecutingDirectory(path);
	String workingPath = path;

	String filePath = workingPath + FileName;
	String outputFilePath = workingPath + OutputFileName;

	PlatformFile::Handle handle = PlatformFile::Open(filePath.GetValue(), PlatformFile::OpenModes::Output);

	Assert(handle != 0, "");

	PlatformFile::Write(handle, Shader);
	PlatformFile::Close(handle);

	String str;
	str += "C:/VulkanSDK/1.0.30.0/Bin/glslangValidator.exe -V \"";
	str += filePath;
	str += "\" -o \"";
	str += outputFilePath;
	str += "\"";

	system(str.GetValue());

	handle = PlatformFile::Open(outputFilePath.GetValue(), PlatformFile::OpenModes::Binary | PlatformFile::OpenModes::Input);

	Assert(handle != 0, "");

	uint64 size = PlatformFile::Size(handle);
	char8 data[2048];
	PlatformFile::Read(handle, data, size);
	PlatformFile::Close(handle);

	ShaderModuleCreateInfo createInfo;
	createInfo.codeSize = size;
	createInfo.pCode = reinterpret_cast<uint32*>(data);

	return Device.createShaderModule(createInfo);
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData)
{
	std::cerr << "validation layer: " << msg << std::endl;

	return VK_FALSE;
}

Instance CreateInstance(void)
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

	VkDebugReportCallbackCreateInfoEXT debugReportCreateInfo;
	debugReportCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	debugReportCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;// | VK_DEBUG_REPORT_DEBUG_BIT_EXT
	debugReportCreateInfo.pfnCallback = DebugCallback;

	VkDebugReportCallbackEXT callback;

	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	func(instance, &debugReportCreateInfo, nullptr, &callback);

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

uint32 FindMemoryType(PhysicalDevice PhysicalDevice, uint32 TypeFilter, MemoryPropertyFlags Properties)
{
	PhysicalDeviceMemoryProperties memoryProperties = PhysicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((TypeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & Properties) == Properties)
		{
			return i;
		}
	}
}

Device CreateLogicalDevice(Instance Instance, PhysicalDevice PhysicalDevice, Queue &Queue, SurfaceKHR &SurfaceKHR, PlatformWindow::Handle Surface, uint8 &QueueIndex)
{
	std::vector<QueueFamilyProperties> queueFamilies = PhysicalDevice.getQueueFamilyProperties();

	Win32SurfaceCreateInfoKHR surfaceCreateInfo;
	surfaceCreateInfo.hinstance = (HINSTANCE)PlatformOS::GetExecutingModuleInstance();
	surfaceCreateInfo.hwnd = (HWND)Surface;

	SurfaceKHR = Instance.createWin32SurfaceKHR(surfaceCreateInfo, nullptr);

	QueueIndex = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & QueueFlagBits::eGraphics)
			break;

		Bool32 presentSupport = PhysicalDevice.getSurfaceSupportKHR(QueueIndex, SurfaceKHR);

		++QueueIndex;
	}

	DeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.queueFamilyIndex = QueueIndex;
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

SwapchainKHR CreateSwapchain(PhysicalDevice PhysicalDevice, Device Device, SurfaceKHR SurfaceKHR, SurfaceFormatKHR &Fromat, std::vector<ImageView> &SwapchainImageViews)
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

	SwapchainKHR swapChain = Device.createSwapchainKHR(createInfo);

	std::vector<Image> swapChainImages = Device.getSwapchainImagesKHR(swapChain);

	for (const Image &image : swapChainImages)
	{
		ImageViewCreateInfo createInfo;
		createInfo.image = image;
		createInfo.viewType = ImageViewType::e2D;
		createInfo.format = Fromat.format;
		createInfo.components.r = ComponentSwizzle::eIdentity;
		createInfo.components.g = ComponentSwizzle::eIdentity;
		createInfo.components.b = ComponentSwizzle::eIdentity;
		createInfo.components.a = ComponentSwizzle::eIdentity;
		createInfo.subresourceRange.aspectMask = ImageAspectFlagBits::eColor;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		SwapchainImageViews.emplace_back(Device.createImageView(createInfo));
	}

	return swapChain;
}

RenderPass CreateRenderPass(Device Device, SurfaceFormatKHR Fromat)
{
	AttachmentDescription colorAttachmentDesc = {};
	colorAttachmentDesc.format = Fromat.format;
	colorAttachmentDesc.samples = SampleCountFlagBits::e1;
	colorAttachmentDesc.loadOp = AttachmentLoadOp::eClear;
	colorAttachmentDesc.storeOp = AttachmentStoreOp::eStore;
	colorAttachmentDesc.stencilLoadOp = AttachmentLoadOp::eDontCare;
	colorAttachmentDesc.stencilStoreOp = AttachmentStoreOp::eDontCare;
	colorAttachmentDesc.initialLayout = ImageLayout::eUndefined;
	colorAttachmentDesc.finalLayout = ImageLayout::ePresentSrcKHR;

	AttachmentReference colorAttachmentRef;
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = ImageLayout::eColorAttachmentOptimal;

	SubpassDescription subPassDesc;
	subPassDesc.pipelineBindPoint = PipelineBindPoint::eGraphics;
	subPassDesc.colorAttachmentCount = 1;
	subPassDesc.pColorAttachments = &colorAttachmentRef;

	SubpassDependency dependency;
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = PipelineStageFlagBits::eColorAttachmentOutput;
	dependency.dstAccessMask = AccessFlagBits::eColorAttachmentRead | AccessFlagBits::eColorAttachmentWrite;

	RenderPassCreateInfo renderPassCreateInfo;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachmentDesc;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subPassDesc;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &dependency;

	return Device.createRenderPass(renderPassCreateInfo);
}

Pipeline CreatePipeline(Device Device, RenderPass RenderPass)
{
	PipelineShaderStageCreateInfo vertexCreateInfo;
	vertexCreateInfo.stage = ShaderStageFlagBits::eVertex;
	vertexCreateInfo.module = vertexShaderModule.Get();
	vertexCreateInfo.pName = "main";

	PipelineShaderStageCreateInfo fragmentCreateInfo;
	fragmentCreateInfo.stage = ShaderStageFlagBits::eFragment;
	fragmentCreateInfo.module = fragmentShaderModule.Get();
	fragmentCreateInfo.pName = "main";

	PipelineShaderStageCreateInfo shaderStages[] = { vertexCreateInfo, fragmentCreateInfo };

	auto bindingDesc = Vertex::GetBindingDescription();
	auto attributeDesc = Vertex::GetAttributeDescriptions();

	PipelineVertexInputStateCreateInfo vertexInputCreateInfo;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
	vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDesc;
	vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32>(attributeDesc.size());
	vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDesc.data();

	PipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	inputAssemblyInfo.topology = PrimitiveTopology::eTriangleList;
	inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	Viewport viewport;
	viewport = 0.0F;
	viewport = 0.0F;
	viewport.width = 800;
	viewport.height = 600;
	viewport.minDepth = 0.0F;
	viewport.maxDepth = 1.0F;

	Rect2D scissor;
	scissor.offset = { 0, 0 };
	scissor.extent = { 800, 600 };

	PipelineViewportStateCreateInfo viewPortCreateInfo;
	viewPortCreateInfo.viewportCount = 1;
	viewPortCreateInfo.pViewports = &viewport;
	viewPortCreateInfo.scissorCount = 1;
	viewPortCreateInfo.pScissors = &scissor;

	PipelineRasterizationStateCreateInfo rasterizerCreateInfo;
	rasterizerCreateInfo.depthClampEnable = VK_TRUE;
	rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizerCreateInfo.polygonMode = PolygonMode::eFill;
	rasterizerCreateInfo.lineWidth = 1.0F;
	rasterizerCreateInfo.cullMode = CullModeFlagBits::eBack;
	rasterizerCreateInfo.frontFace = FrontFace::eClockwise;
	rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizerCreateInfo.depthBiasConstantFactor = 0.0F;
	rasterizerCreateInfo.depthBiasClamp = 0.0F;
	rasterizerCreateInfo.depthBiasSlopeFactor = 0.0F;

	PipelineMultisampleStateCreateInfo multisampleCreateInfo;
	multisampleCreateInfo.sampleShadingEnable = VK_FALSE;
	multisampleCreateInfo.rasterizationSamples = SampleCountFlagBits::e1;
	multisampleCreateInfo.minSampleShading = 1.0F;
	multisampleCreateInfo.pSampleMask = nullptr;
	multisampleCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multisampleCreateInfo.alphaToOneEnable = VK_FALSE;

	PipelineColorBlendAttachmentState colorBlendAttackmentState;
	colorBlendAttackmentState.colorWriteMask = ColorComponentFlagBits::eR | ColorComponentFlagBits::eG | ColorComponentFlagBits::eB | ColorComponentFlagBits::eA;
	colorBlendAttackmentState.blendEnable = VK_FALSE;
	colorBlendAttackmentState.srcColorBlendFactor = BlendFactor::eOne;
	colorBlendAttackmentState.dstColorBlendFactor = BlendFactor::eZero;
	colorBlendAttackmentState.colorBlendOp = BlendOp::eAdd;
	colorBlendAttackmentState.srcAlphaBlendFactor = BlendFactor::eOne;
	colorBlendAttackmentState.dstAlphaBlendFactor = BlendFactor::eZero;

	PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
	colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendStateCreateInfo.logicOp = LogicOp::eCopy;
	colorBlendStateCreateInfo.attachmentCount = 1;
	colorBlendStateCreateInfo.pAttachments = &colorBlendAttackmentState;
	colorBlendStateCreateInfo.blendConstants[0] = 0.0F;
	colorBlendStateCreateInfo.blendConstants[1] = 0.0F;
	colorBlendStateCreateInfo.blendConstants[2] = 0.0F;
	colorBlendStateCreateInfo.blendConstants[3] = 0.0F;

	DynamicState dynamicStates[] = { DynamicState::eViewport , DynamicState::eLineWidth };

	PipelineDynamicStateCreateInfo dynamicStateCreateInfo;
	dynamicStateCreateInfo.dynamicStateCount = 2;
	dynamicStateCreateInfo.pDynamicStates = dynamicStates;

	PipelineLayoutCreateInfo layoutCreateInfo;
	layoutCreateInfo.setLayoutCount = 0;
	layoutCreateInfo.pSetLayouts = nullptr;
	layoutCreateInfo.pushConstantRangeCount = 0;
	layoutCreateInfo.pPushConstantRanges = 0;

	PipelineLayout layout = Device.createPipelineLayout(layoutCreateInfo);

	GraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputCreateInfo;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineInfo.pViewportState = &viewPortCreateInfo;
	pipelineInfo.pRasterizationState = &rasterizerCreateInfo;
	pipelineInfo.pMultisampleState = &multisampleCreateInfo;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlendStateCreateInfo;
	pipelineInfo.pDynamicState = &dynamicStateCreateInfo;
	pipelineInfo.layout = layout;
	pipelineInfo.renderPass = RenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	return Device.createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo);
}

void CreateFramebuffers(Device Device, RenderPass RenderPass, std::vector<ImageView> &SwapchainImageViews, std::vector<Framebuffer> &SwapChainFramebuffers)
{
	for (int i = 0; i < SwapchainImageViews.size(); ++i)
	{
		FramebufferCreateInfo createInfo;
		createInfo.renderPass = RenderPass;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = &SwapchainImageViews[i];
		createInfo.width = 800;
		createInfo.height = 600;
		createInfo.layers = 1;

		SwapChainFramebuffers.emplace_back(Device.createFramebuffer(createInfo));
	}
}

CommandPool CreateCommandPool(Device Device, uint8 QueueIndex)
{
	CommandPoolCreateInfo createInfo;
	createInfo.queueFamilyIndex = QueueIndex;
	//createInfo.flags = CommandPoolCreateFlagBits::

	return Device.createCommandPool(createInfo);
}

void CreateCommandBuffers(Device Device, CommandPool CommandPool, uint8 Count, std::vector<CommandBuffer> &CommandBuffers)
{
	CommandBufferAllocateInfo allocInfo;
	allocInfo.commandPool = CommandPool;
	allocInfo.level = CommandBufferLevel::ePrimary;
	allocInfo.commandBufferCount = Count;

	CommandBuffers = Device.allocateCommandBuffers(allocInfo);
}

void CreateSemaphores(Device Device)
{
	SemaphoreCreateInfo createInfo;

	imageAvailableSemaphore = Device.createSemaphore(createInfo);
	renderFinishedSemaphore = Device.createSemaphore(createInfo);
}

Buffer CreateVertexBuffer(Device Device, PhysicalDevice PhysicalDevice)
{
	BufferCreateInfo createInfo;
	createInfo.size = sizeof(Vertex) * vertices.size();
	createInfo.usage = BufferUsageFlagBits::eVertexBuffer;
	createInfo.sharingMode = SharingMode::eExclusive;

	Buffer buffer = Device.createBuffer(createInfo);

	MemoryRequirements memReq = Device.getBufferMemoryRequirements(buffer);

	MemoryAllocateInfo allocInfo;
	allocInfo.allocationSize = memReq.size;
	allocInfo.memoryTypeIndex = FindMemoryType(PhysicalDevice, memReq.memoryTypeBits, MemoryPropertyFlagBits::eHostVisible | MemoryPropertyFlagBits::eHostCoherent);

	DeviceMemory memory = Device.allocateMemory(allocInfo);
	void *data = Device.mapMemory(memory, 0, createInfo.size);
	PlatformMemory::Copy(reinterpret_cast<const int8*>(&vertices[0]), (int8*)data, createInfo.size);
	Device.unmapMemory(memory);

	return buffer;
}

void Render(Device Device, SwapchainKHR Swapchain, Queue Queue, std::vector<CommandBuffer> &CommandBuffers, std::vector<Framebuffer> &SwapChainFramebuffers, RenderPass RenderPass, Pipeline GraphicPipeline, Buffer Buffer)
{
	for (int i = 0; i < CommandBuffers.size(); ++i)
	{
		CommandBuffer &commandBuffer = CommandBuffers[i];

		CommandBufferBeginInfo commandBeginInfo;
		commandBeginInfo.flags = CommandBufferUsageFlagBits::eSimultaneousUse;
		commandBeginInfo.pInheritanceInfo = nullptr;

		commandBuffer.begin(commandBeginInfo);

		RenderPassBeginInfo renderPassBeginInfo;
		renderPassBeginInfo.renderPass = RenderPass;
		renderPassBeginInfo.framebuffer = SwapChainFramebuffers[i];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = { 800, 600 };

		ClearValue clearColor;
		clearColor.color.setFloat32({ 0.0F, 0.0F, 0.0F, 1.0F });
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		DeviceSize offset[] = { 0 };
		commandBuffer.bindVertexBuffers(0, 1, &Buffer, offset);

		commandBuffer.beginRenderPass(renderPassBeginInfo, SubpassContents::eInline);

		commandBuffer.bindPipeline(PipelineBindPoint::eGraphics, GraphicPipeline);

		commandBuffer.draw(vertices.size(), 1, 0, 0);

		commandBuffer.endRenderPass();

		commandBuffer.end();
	}

	ResultValue<uint32> imageIndex = Device.acquireNextImageKHR(Swapchain, 1000, imageAvailableSemaphore, VK_NULL_HANDLE);

	PipelineStageFlags waitStage = PipelineStageFlagBits::eColorAttachmentOutput;

	SubmitInfo submitInfo;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
	submitInfo.pWaitDstStageMask = &waitStage;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &CommandBuffers[imageIndex.value];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderFinishedSemaphore;

	Queue.submit(1, &submitInfo, VK_NULL_HANDLE);

	PresentInfoKHR presentInfo;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &renderFinishedSemaphore;

	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &Swapchain;
	presentInfo.pImageIndices = &imageIndex.value;
	presentInfo.pResults = nullptr;

	Queue.presentKHR(presentInfo);
}

void InitializeVulkan(PlatformWindow::Handle Surface)
{
	// Using validation layers
	// Using custom allocator

	auto instance = RunJobInternal(CreateInstance);
	while (!instance.IsFinished());
	std::cout << "instance created\n";

	auto physicalDevice = RunJobInternal(PickPhysicalDevice, instance.Get());
	while (!physicalDevice.IsFinished());
	std::cout << "physicalDevice created\n";

	Queue graphicsQueue;
	SurfaceKHR surfaceKHR;
	uint8 QueueIndex;
	auto device = RunJobInternal(CreateLogicalDevice, instance.Get(), physicalDevice.Get(), graphicsQueue, surfaceKHR, Surface, QueueIndex);
	while (!device.IsFinished());
	std::cout << "device created\n";

	SurfaceFormatKHR format;
	std::vector<ImageView> swapchainImageViews;
	auto swapchain = RunJobInternal(CreateSwapchain, physicalDevice.Get(), device.Get(), surfaceKHR, format, swapchainImageViews);
	while (!swapchain.IsFinished());
	std::cout << "swapchain created\n";

	vertexShaderModule = RunJobInternal(CompileShader, vertexShader, "vertex.vert", "vertex.spv", device.Get());
	fragmentShaderModule = RunJobInternal(CompileShader, fragmentShader, "fragment.frag", "fragment.spv", device.Get());
	while (!vertexShaderModule.IsFinished() || !fragmentShaderModule.IsFinished());
	std::cout << "shaders compilation finished\n";

	auto renderPass = RunJobInternal(CreateRenderPass, device.Get(), format);
	while (!renderPass.IsFinished());
	std::cout << "render-pass created\n";

	auto pipeline = RunJobInternal(CreatePipeline, device.Get(), renderPass.Get());
	while (!pipeline.IsFinished());
	std::cout << "pipeline created\n";

	std::vector<Framebuffer> swapchainFramebuffers;
	auto frameBuffer = RunJobInternal(CreateFramebuffers, device.Get(), renderPass.Get(), swapchainImageViews, swapchainFramebuffers);
	while (!frameBuffer.IsFinished());
	std::cout << "frame-buffers created\n";

	auto commandPool = RunJobInternal(CreateCommandPool, device.Get(), QueueIndex);
	while (!frameBuffer.IsFinished());
	std::cout << "command-pool created\n";

	std::vector<CommandBuffer> commandBuffers;
	auto commandBuffer = RunJobInternal(CreateCommandBuffers, device.Get(), commandPool.Get(), (uint8)swapchainFramebuffers.size(), commandBuffers);
	while (!commandBuffer.IsFinished());
	std::cout << "command-buffers created\n";

	auto semaphores = RunJobInternal(CreateSemaphores, device.Get());
	while (!semaphores.IsFinished());
	std::cout << "semaphores created\n";

	auto vertexBuffer = RunJobInternal(CreateVertexBuffer, device.Get(), physicalDevice.Get());
	while (!vertexBuffer.IsFinished());
	std::cout << "vertex-buffer created\n";

	DrawFrame = [device, swapchain, graphicsQueue, commandBuffers, swapchainFramebuffers, renderPass, pipeline, vertexBuffer]()
	{
		std::vector<CommandBuffer> cb = commandBuffers;
		std::vector<Framebuffer> scfb = swapchainFramebuffers;

		Render(device.Get(), swapchain.Get(), graphicsQueue, cb, scfb, renderPass.Get(), pipeline.Get(), vertexBuffer.Get());
	};
}

int32 WindowProcedure(PlatformWindow::Handle hWnd, uint32 message, uint32* wParam, uint32* lParam)
{
	return PlatformWindow::DefaultProcedure(hWnd, message, wParam, lParam);
}

PlatformWindow::Handle CreateContext()
{
	PlatformWindow::Handle handle = PlatformWindow::Create(PlatformOS::GetExecutingModuleInstance(), "TestVulkan", PlatformWindow::Style::OverlappedWindow | PlatformWindow::Style::Visible, WindowProcedure);
	PlatformWindow::SetSize(handle, 800, 600);
	return handle;
}

void main()
{
	PlatformWindow::Handle surface = CreateContext();

	auto initializeVulkan = RunJobInternal(InitializeVulkan, surface);

	MSG msg;
	while (true)
	{
		while (PeekMessage(&msg, NULL, 0, 0, TRUE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (initializeVulkan.IsFinished())
			DrawFrame();
	}

	//vkDestroyDevice(device.Get(), nullptr);
	//vkDestroyInstance(instance.Get(), nullptr);
}