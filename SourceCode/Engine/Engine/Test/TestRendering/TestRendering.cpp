#include <Common\PrimitiveTypes.h>
#include <Vulkan\include\vulkan\vulkan.h>
#include <exception>
#include <iostream>
#include <vector>

using namespace Engine::Common;

#define CHECK_RESULT(Result) \
if (Result != VK_SUCCESS) \
{ \
	throw std::exception(#Result ## " Failed"); \
}

void main()
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

	uint32 extensionsCount = 0;
	CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr));

	std::vector<VkExtensionProperties> extensions(extensionsCount);
	CHECK_RESULT(vkEnumerateInstanceExtensionProperties(nullptr, &extensionsCount, extensions.data()));



	vkDestroyInstance(instance, nullptr);
}