// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef VULKAN_WRAPPER_H
#define VULKAN_WRAPPER_H

#include <Common\PrimitiveTypes.h>
#include <VulkanSDK\include\vulkan\vulkan.hpp>
#include <functional>

using namespace vk;

namespace Engine
{
	namespace VulkanRenderDevice
	{
		namespace Private
		{
#define SUCCESS(Expression) ((Expression) == Result::eSuccess)

			class VulkanWrapper
			{
			public:
				class Initialization
				{
				public:
					typedef std::function<void(DebugUtilsMessageTypeFlagBitsEXT, const DebugUtilsMessengerCallbackDataEXT*, DebugUtilsMessageSeverityFlagBitsEXT)> DebugCallbackType;

				private:
					static uint32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT Severity, VkDebugUtilsMessageTypeFlagsEXT Type, const VkDebugUtilsMessengerCallbackDataEXT* Data, void* UserData)
					{
						DebugCallbackType& callback = *(ReinterpretCast(DebugCallbackType*, UserData));

						callback((DebugUtilsMessageTypeFlagBitsEXT)Type, ReinterpretCast(const DebugUtilsMessengerCallbackDataEXT*, Data), (DebugUtilsMessageSeverityFlagBitsEXT)Severity);

						return 0;
					}

				public:
					static bool CreateInstance(Instance* Instance, bool DebugMode, DebugCallbackType Callback)
					{
						static auto DebugCallbackUserData = Callback;

						InstanceCreateInfo info = {};

						VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerInfo = {};
						if (DebugMode)
						{
							static const cstr ValidationLayers[] = { "VK_LAYER_KHRONOS_validation" };

							info.ppEnabledLayerNames = ValidationLayers;
							info.enabledLayerCount = _countof(ValidationLayers);

							static const cstr Extensions[] = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
							info.ppEnabledExtensionNames = Extensions;
							info.enabledExtensionCount = _countof(Extensions);

							debugUtilsMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
							debugUtilsMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
							debugUtilsMessengerInfo.pfnUserCallback = DebugCallback;
							debugUtilsMessengerInfo.pUserData = &DebugCallbackUserData;
						}

						*Instance = createInstance(info);

						return Instance;
					}

					static bool FindBestAdapter(Instance Instance, PhysicalDevice* Device)
					{
						static const uint8 OrderedTypePriorities[] = { 1, 4, 5, 2, 3 };

						PhysicalDevice devices[8];
						uint32 count = _countof(devices);

						if (!SUCCESS(Instance.enumeratePhysicalDevices(&count, devices)))
							return false;

						if (count == 0)
							return false;

						uint8 maxPriority = 0;

						for (uint8 i = 0; i < count; ++i)
						{
							const PhysicalDevice& device = devices[i];

							PhysicalDeviceProperties properties = {};
							device.getProperties(&properties);

							uint8 priority = OrderedTypePriorities[(uint8)properties.deviceType];

							if (priority < maxPriority)
								continue;

							maxPriority = priority;
							*Device = device;
						}

						return Device;
					}

					static bool CreateDevice(PhysicalDevice PhysicalDevice, Device* Device)
					{
						DeviceQueueCreateInfo queueInfo = {};
						float queuePriority = 1.0f;
						queueInfo.pQueuePriorities = &queuePriority;

						PhysicalDeviceFeatures features = {};

						DeviceCreateInfo info = {};
						info.pQueueCreateInfos = &queueInfo;
						info.queueCreateInfoCount = 1;
						info.pEnabledFeatures = &features;

						*Device = PhysicalDevice.createDevice(info);

						return Device;
					}

					//static bool CreateDeviceQueue(Device Device, DeviceQUeue* DeviceQueue)
					//{
						//Device.getQueue
					//}
				};

				template<typename T>
				static bool DestroyInstance(T Instance)
				{
					Instance.destroy();

					return true;
				}
			};
		}
	}
}

#endif