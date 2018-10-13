// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\RenderingManager.h>
#include <Rendering\Texture.h>
#include <Rendering\Program.h>
#include <Rendering\Mesh.h>
#include <Rendering\Window.h>
#include <Rendering\Private\Allocators.h>
#include <Rendering\Private\OpenGL\OpenGLDevice.h>
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <Rendering\Private\Commands\Command.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;
		using namespace Private::OpenGL;
		using namespace Private::ShaderCompiler;

#define CHECK_DEVICE() Assert(m_Device != nullptr, "m_Device cannot be null")
#define CHECK_CALL(Experssion) Assert((Experssion), m_Device->GetLastError())
#define ALLOCATE(Type) ReinterpretCast(Type*, AllocateMemory(&Allocators::RenderingSystemAllocator, sizeof(Type)));

		DeviceInterfarce::DeviceInterfarce(Type Type) :
			m_Type(Type),
			m_Device(nullptr),
			m_Textures(&Allocators::RenderingSystemAllocator),
			m_Programs(&Allocators::RenderingSystemAllocator),
			m_Windows(&Allocators::RenderingSystemAllocator),
			m_Commands(&Allocators::RenderingSystemAllocator)
		{
		}

		DeviceInterfarce::~DeviceInterfarce(void)
		{
			for each (auto item in m_Textures)
				DestroyTexture2D(item);

			for each (auto item in m_Programs)
				DestroyProgram(item);

			for each (auto item in m_Windows)
				DestroyWindow(item);

			if (m_Device != nullptr)
			{
				m_Device->~IDevice();
				DeallocateMemory(&Allocators::RenderingSystemAllocator, m_Device);
			}
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

		void DeviceInterfarce::SetClearColor(Color Color)
		{
			CHECK_DEVICE();

			m_Device->SetClearColor(Color);
		}

		void DeviceInterfarce::SetClearFlags(IDevice::ClearFlags Flags)
		{
			CHECK_DEVICE();

			m_Device->SetClearFlags(Flags);
		}

		Texture *DeviceInterfarce::CreateTexture2D(const byte * Data, uint32 Width, uint32 Height)
		{
			CHECK_DEVICE();

			Texture::Handle handle;
			CHECK_CALL(m_Device->CreateTexture2D(Data, Width, Height, handle));

			Texture *texture = ALLOCATE(Texture);
			new (texture) Texture(m_Device, handle);

			m_Textures.Add(texture);

			return texture;
		}

		void DeviceInterfarce::DestroyTexture2D(Texture *Texture)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyTexture2D(Texture->GetHandle()));
			Texture->~Texture();
			DeallocateMemory(&Allocators::RenderingSystemAllocator, Texture);
		}

		Program *DeviceInterfarce::CreateProgram(const String &Shader)
		{
			static Compiler compiler;

			CHECK_DEVICE();

			String vertProgram;
			String fragProgram;
			compiler.Compile(m_Type, Shader, vertProgram, fragProgram);

			vertProgram = "#version 330 core\nlayout(location = 0) in vec3 vertexPosition_modelspace;\nvoid main(){gl_Position.xyz = vertexPosition_modelspace;gl_Position.w = 1.0;}";
			fragProgram = "#version 330 core\nout vec3 color;\nvoid main() {color = vec3(0, 1, 0);}";

			Program::Handle handle;
			CHECK_CALL(m_Device->CreateProgram(vertProgram.GetValue(), fragProgram.GetValue(), handle));

			Program *program = ALLOCATE(Program);
			new (program) Program(m_Device, handle);

			m_Programs.Add(program);

			return program;
		}

		void DeviceInterfarce::DestroyProgram(Program *Program)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyProgram(Program->GetHandle()));
			Program->~Program();
			DeallocateMemory(&Allocators::RenderingSystemAllocator, Program);
		}

		Mesh *DeviceInterfarce::CreateMesh(const float32 *VerticesData, uint32 VertexCount, const float32 *UVsData, uint32 UVsCount, IDevice::BufferUsages Usage)
		{
			CHECK_DEVICE();

			GPUBuffer::Handle verticesHandle;
			CHECK_CALL(m_Device->CreateBuffer(VerticesData, VertexCount, Usage, verticesHandle));

			GPUBuffer::Handle uvsHandle;
			CHECK_CALL(m_Device->CreateBuffer(UVsData, UVsCount, Usage, uvsHandle));

			Mesh *mesh = ALLOCATE(Mesh);
			new (mesh) Mesh(GPUBuffer(m_Device, verticesHandle, VertexCount), GPUBuffer(m_Device, uvsHandle, UVsCount));

			return mesh;
		}

		void DeviceInterfarce::DestroyMesh(Mesh *Mesh)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyBuffer(Mesh->GetVertices().GetHandle()));
			CHECK_CALL(m_Device->DestroyBuffer(Mesh->GetUVs().GetHandle()));
			Mesh->~Mesh();
			DeallocateMemory(&Allocators::RenderingSystemAllocator, Mesh);
		}

		Window *DeviceInterfarce::CreateWindow(uint16 Width, uint16 Height, cstr Title)
		{
			CHECK_DEVICE();

			Window::Handle handle;
			CHECK_CALL(m_Device->CreateWindow(Width, Height, Title, handle));

			Window *window = ALLOCATE(Window);
			new (window) Window(m_Device, handle);

			m_Windows.Add(window);

			return window;
		}

		void DeviceInterfarce::DestroyWindow(Window *Window)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyWindow(Window->GetHandle()));
			Window->~Window();
			DeallocateMemory(&Allocators::RenderingSystemAllocator, Window);
		}

		void DeviceInterfarce::DrawMesh(Mesh *Mesh, Program *Program)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->BindProgram(Program->GetHandle()));

			auto &vertices = Mesh->GetVertices();
			CHECK_CALL(m_Device->BindBuffer(vertices.GetHandle(), 3, 0, false, 0));

			//auto &uvs = Mesh->GetUVs();

			m_Device->Draw(IDevice::DrawModes::Triangles, 0, 12 * 3);
		}

		void DeviceInterfarce::BeginRender(void)
		{
			CHECK_DEVICE();

			m_Device->Clear();

			for each (auto command in m_Commands)
				command->Execute(m_Device);
		}

		void DeviceInterfarce::EndRender(void)
		{
			CHECK_DEVICE();

			for each (auto window in m_Windows)
				m_Device->SwapBuffers(window->GetHandle());

			m_Device->PollEvents();
		}

		void DeviceInterfarce::InitializeDevice(void)
		{
			switch (m_Type)
			{
			case Type::OpenGL:
			{
				m_Device = ALLOCATE(OpenGLDevice);
				new (m_Device) OpenGLDevice;
			} break;
			}

			CHECK_DEVICE();

			CHECK_CALL(m_Device->Initialize());
		}
	}
}
