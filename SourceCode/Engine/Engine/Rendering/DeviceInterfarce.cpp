// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\RenderingManager.h>
#include <Rendering\Private\Allocators.h>
#include <Rendering\Private\OpenGL\OpenGLDevice.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;
		using namespace Private::OpenGL;

		DeviceInterfarce::DeviceInterfarce(Type Type) :
			m_Type(Type),
			m_Device(nullptr)
		{
			InitializeDevice();
		}

		DeviceInterfarce::~DeviceInterfarce(void)
		{
			m_Device->~IDevice();
			DeallocateMemory(&Allocators::RenderingSystemAllocator, m_Device);
		}

		void DeviceInterfarce::InitializeDevice(void)
		{
			switch (m_Type)
			{
			case Type::OpenGL:
			{
				m_Device = reinterpret_cast<IDevice*>(AllocateMemory(&Allocators::RenderingSystemAllocator, sizeof(OpenGLDevice)));
				new (m_Device) OpenGLDevice;
			} break;
			}
		}
	}
}
