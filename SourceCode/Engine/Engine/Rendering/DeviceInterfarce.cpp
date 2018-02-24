// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\RenderingManager.h>
#include <Rendering\Texture.h>
#include <Rendering\Program.h>
#include <Rendering\Window.h>
#include <Rendering\Private\Allocators.h>
#include <Rendering\Private\OpenGL\OpenGLDevice.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;
		using namespace Private::OpenGL;

#define CHECK_DEVICE() Assert(m_Device != nullptr, "m_Device cannot be null")
#define CHECK_CALL(Experssion) Assert((Experssion), m_Device->GetLastError())

		DeviceInterfarce::DeviceInterfarce(Type Type) :
			m_Type(Type),
			m_Device(nullptr)
		{
		}

		DeviceInterfarce::~DeviceInterfarce(void)
		{
			m_Device->~IDevice();
			DeallocateMemory(&Allocators::RenderingSystemAllocator, m_Device);
		}

		void DeviceInterfarce::Initialize(void)
		{
			InitializeDevice();
		}

		void DeviceInterfarce::SetSampleCount(uint8 Count)
		{
			CHECK_DEVICE();

			m_Device->SetSampleCount(Count);
		}

		void DeviceInterfarce::SetForwardCompatible(bool Value)
		{
			CHECK_DEVICE();

			m_Device->SetForwardCompatible(Value);
		}

		void DeviceInterfarce::SetProfilingEnabled(bool Value)
		{
			CHECK_DEVICE();

			m_Device->SetProfilingEnabled(Value);
		}

		Texture *DeviceInterfarce::CreateTexture2D(const byte * Data, uint32 Width, uint32 Height)
		{
			CHECK_DEVICE();

			Texture::Handle handle;
			CHECK_CALL(m_Device->CreateTexture2D(Data, Width, Height, handle));

			Texture *texture = reinterpret_cast<Texture*>(AllocateMemory(&Allocators::RenderingSystemAllocator, sizeof(Texture)));
			new (texture) Texture(m_Device, handle);
			return texture;
		}

		Program *DeviceInterfarce::CreateProgram(cstr VertexShader, cstr FragmentShader)
		{
			CHECK_DEVICE();

			Program::Handle handle;
			CHECK_CALL(m_Device->CreateProgram(VertexShader, FragmentShader, handle));

			Program *program = reinterpret_cast<Program*>(AllocateMemory(&Allocators::RenderingSystemAllocator, sizeof(Program)));
			new (program) Program(m_Device, handle);
			return program;
		}

		Window *DeviceInterfarce::CreateWindow(uint16 Width, uint16 Height, cstr Title)
		{
			CHECK_DEVICE();

			Window::Handle handle;
			CHECK_CALL(m_Device->CreateWindow(Width, Height, Title, handle));

			Window *window = reinterpret_cast<Window*>(AllocateMemory(&Allocators::RenderingSystemAllocator, sizeof(Window)));
			new (window) Window(m_Device, handle);
			return window;
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

			m_Device->Initialize();
		}
	}
}
