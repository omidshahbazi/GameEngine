// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\DeviceInterface.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Texture.h>
#include <Rendering\Program.h>
#include <Rendering\Mesh.h>
#include <Rendering\Window.h>
#include <Rendering\Private\Allocators.h>
#include <Rendering\Private\OpenGL\OpenGLDevice.h>
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <Rendering\Private\Commands\DrawCommand.h>
#include <Rendering\ProgramConstantSupplier.h>
#include <Rendering\Material.h>

namespace Engine
{
	namespace Rendering
	{
		using namespace Private;
		using namespace Private::OpenGL;
		using namespace Private::ShaderCompiler;
		using namespace Private::Commands;

#define CHECK_DEVICE() Assert(m_Device != nullptr, "m_Device cannot be null")
#define CHECK_CALL(Experssion) if (!(Experssion)) Assert(false, m_Device->GetLastError());
#define ALLOCATE_ARRAY(Type, Count) ReinterpretCast(Type*, AllocateMemory(&Allocators::RenderingSystemAllocator, Count * sizeof(Type)))
#define ALLOCATE(Type) ALLOCATE_ARRAY(Type, 1)
#define DEALLOCATE(Pointer) DeallocateMemory(&Allocators::RenderingSystemAllocator, Pointer)

#define ALLOCATE_COMMAND(Type) ReinterpretCast(Type*, AllocateMemory(&Allocators::CommandAllocator, sizeof(Type)))

		DeviceInterface::DeviceInterface(Type Type) :
			m_Type(Type),
			m_Device(nullptr),
			m_Textures(&Allocators::RenderingSystemAllocator),
			m_Programs(&Allocators::RenderingSystemAllocator),
			m_Windows(&Allocators::RenderingSystemAllocator),
			m_Commands(&Allocators::RenderingSystemAllocator)
		{
			ProgramConstantSupplier::Create(&Allocators::RenderingSystemAllocator);
		}

		DeviceInterface::~DeviceInterface(void)
		{
			for each (auto item in m_Textures)
				DestroyTexture(item);

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

		void DeviceInterface::Initialize(void)
		{
			InitializeDevice();
		}

		void DeviceInterface::SetSampleCount(uint8 Count)
		{
			CHECK_DEVICE();

			m_Device->SetSampleCount(Count);
		}

		void DeviceInterface::SetForwardCompatible(bool Value)
		{
			CHECK_DEVICE();

			m_Device->SetForwardCompatible(Value);
		}

		void DeviceInterface::SetClearColor(Color Color)
		{
			CHECK_DEVICE();

			m_Device->SetClearColor(Color);
		}

		void DeviceInterface::SetClearFlags(IDevice::ClearFlags Flags)
		{
			CHECK_DEVICE();

			m_Device->SetClearFlags(Flags);
		}

		void DeviceInterface::SetFaceOrder(IDevice::FaceOrders Order)
		{
			CHECK_DEVICE();

			m_Device->SetFaceOrder(Order);
		}

		void DeviceInterface::SetCullMode(IDevice::CullModes Modes)
		{
			CHECK_DEVICE();

			m_Device->SetCullMode(Modes);
		}

		void DeviceInterface::SetDepthTestFunction(IDevice::TestFunctions Function)
		{
			CHECK_DEVICE();

			m_Device->SetDepthTestFunction(Function);
		}

		void DeviceInterface::SetStencilTestFunction(IDevice::CullModes CullMode, IDevice::TestFunctions Function, int32 Reference, uint32 Mask)
		{
			CHECK_DEVICE();

			m_Device->SetStencilTestFunction(CullMode, Function, Reference, Mask);
		}

		void DeviceInterface::SetStencilMask(IDevice::CullModes CullMode, uint32 Mask)
		{
			CHECK_DEVICE();

			m_Device->SetStencilMask(CullMode, Mask);
		}

		void DeviceInterface::SetStencilOperation(IDevice::CullModes CullMode, IDevice::StencilOperations StencilFail, IDevice::StencilOperations DepthFailed, IDevice::StencilOperations DepthPassed)
		{
			CHECK_DEVICE();

			m_Device->SetStencilOperation(CullMode, StencilFail, DepthFailed, DepthPassed);
		}

		void DeviceInterface::SetBlendFunction(IDevice::BlendFunctions SourceFactor, IDevice::BlendFunctions DestinationFactor)
		{
			CHECK_DEVICE();

			m_Device->SetBlendFunction(SourceFactor, DestinationFactor);
		}

		void DeviceInterface::SetPolygonMode(IDevice::CullModes CullMode, IDevice::PolygonModes PolygonMode)
		{
			CHECK_DEVICE();

			m_Device->SetPolygonMode(CullMode, PolygonMode);
		}

		Texture *DeviceInterface::CreateTexture2D(const byte *Data, uint32 Width, uint32 Height, uint8 ComponentCount, IDevice::TextureFormats Format)
		{
			CHECK_DEVICE();

			Texture::Handle handle;
			CHECK_CALL(m_Device->CreateTexture2D(Data, Width, Height, ComponentCount, Format, handle));

			Texture *texture = ALLOCATE(Texture);
			new (texture) Texture(m_Device, handle);

			m_Textures.Add(texture);

			return texture;
		}

		void DeviceInterface::DestroyTexture(Texture *Texture)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyTexture(Texture->GetHandle()));
			Texture->~Texture();
			DeallocateMemory(&Allocators::RenderingSystemAllocator, Texture);
		}

		void DeviceInterface::SetTexture2DVerticalWrapping(Texture * Texture, IDevice::TextureWrapModes Mode)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->SetTexture2DVerticalWrapping(Texture->GetHandle(), Mode));
		}

		void DeviceInterface::SetTexture2DHorizontalWrapping(Texture * Texture, IDevice::TextureWrapModes Mode)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->SetTexture2DHorizontalWrapping(Texture->GetHandle(), Mode));
		}

		void DeviceInterface::SetTexture2DMinifyFilter(Texture * Texture, IDevice::MinifyFilters Filter)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->SetTexture2DMinifyFilter(Texture->GetHandle(), Filter));
		}

		void DeviceInterface::SetTexture2DMagnifyFilter(Texture * Texture, IDevice::MagnfyFilters Filter)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->SetTexture2DMagnifyFilter(Texture->GetHandle(), Filter));
		}

		Program *DeviceInterface::CreateProgram(const String &Shader)
		{
			static Compiler compiler;

			CHECK_DEVICE();

			String vertProgram;
			String fragProgram;
			compiler.Compile(m_Type, Shader, vertProgram, fragProgram);

			Program::Handle handle;
			CHECK_CALL(m_Device->CreateProgram(vertProgram.GetValue(), fragProgram.GetValue(), handle));

			Program *program = ALLOCATE(Program);
			new (program) Program(m_Device, handle);

			m_Programs.Add(program);

			return program;
		}

		void DeviceInterface::DestroyProgram(Program *Program)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyProgram(Program->GetHandle()));
			Program->~Program();
			DeallocateMemory(&Allocators::RenderingSystemAllocator, Program);
		}

		Mesh *DeviceInterface::CreateMesh(MeshInfo *Info, IDevice::BufferUsages Usage)
		{
			CHECK_DEVICE();

			Mesh::SubMesh *subMeshes = ALLOCATE_ARRAY(Mesh::SubMesh, Info->SubMeshes.GetSize());

			for (uint16 i = 0; i < Info->SubMeshes.GetSize(); ++i)
			{
				GPUBuffer::Handle handle;

				auto &subMeshInfo = Info->SubMeshes[i];

				CHECK_CALL(m_Device->CreateMesh(&subMeshInfo, Usage, handle));

				new (&subMeshes[i]) Mesh::SubMesh(GPUBuffer(m_Device, handle, subMeshInfo.Vertices.GetSize()), subMeshInfo.Indices.GetSize());
			}

			Mesh *mesh = ALLOCATE(Mesh);
			new (mesh) Mesh(subMeshes, Info->SubMeshes.GetSize());
			return mesh;
		}

		void DeviceInterface::DestroyMesh(Mesh *Mesh)
		{
			//CHECK_DEVICE();

			//CHECK_CALL(m_Device->DestroyBuffer(Mesh->GetVertices().GetHandle()));
			//CHECK_CALL(m_Device->DestroyBuffer(Mesh->GetUVs().GetHandle()));
			//Mesh->~Mesh();
			//DeallocateMemory(&Allocators::RenderingSystemAllocator, Mesh);
		}

		Window *DeviceInterface::CreateWindow(uint16 Width, uint16 Height, cstr Title)
		{
			CHECK_DEVICE();

			Window::Handle handle;
			CHECK_CALL(m_Device->CreateWindow(Width, Height, Title, handle));

			Window *window = ALLOCATE(Window);
			new (window) Window(m_Device, handle);

			m_Windows.Add(window);

			return window;
		}

		void DeviceInterface::DestroyWindow(Window *Window)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyWindow(Window->GetHandle()));
			Window->~Window();
			DeallocateMemory(&Allocators::RenderingSystemAllocator, Window);
		}

		void DeviceInterface::DrawMesh(Mesh * Mesh, const Matrix4F & Transform, Program * Program)
		{
			DrawCommand *cmd = ALLOCATE_COMMAND(DrawCommand);
			new (cmd) DrawCommand(Mesh, Transform, Program);
			m_Commands.Add(cmd);
		}

		void DeviceInterface::DrawMesh(Mesh * Mesh, const Matrix4F & Transform, Material * Material)
		{
			for each (auto & pass in Material->GetPasses())
			{
				DrawCommand *cmd = ALLOCATE_COMMAND(DrawCommand);
				new (cmd) DrawCommand(Mesh, Transform, ConstCast(Pass*, &pass));
				m_Commands.Add(cmd);
			}
		}

		void DeviceInterface::BeginRender(void)
		{
			CHECK_DEVICE();

			m_Device->Clear();

			for each (auto command in m_Commands)
				command->Execute(m_Device);

			EraseCommands();
		}

		void DeviceInterface::EndRender(void)
		{
			CHECK_DEVICE();

			for each (auto window in m_Windows)
				m_Device->SwapBuffers(window->GetHandle());

			m_Device->PollEvents();
		}

		void DeviceInterface::InitializeDevice(void)
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

		void DeviceInterface::EraseCommands(void)
		{
			m_Commands.Clear();

			Allocators::CommandAllocator.Reset();
		}
	}
}
