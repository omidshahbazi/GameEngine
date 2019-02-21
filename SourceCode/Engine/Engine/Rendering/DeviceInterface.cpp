// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\DeviceInterface.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Texture.h>
#include <Rendering\Program.h>
#include <Rendering\Mesh.h>
#include <Rendering\Window.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Rendering\Private\OpenGL\OpenGLDevice.h>
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <Rendering\Private\Commands\ClearCommand.h>
#include <Rendering\Private\Commands\DrawCommand.h>
#include <Rendering\Private\Commands\SwitchRenderTargetCommand.h>
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
#define ALLOCATE_ARRAY(Type, Count) ReinterpretCast(Type*, AllocateMemory(&RenderingAllocators::RenderingSystemAllocator, Count * sizeof(Type)))
#define ALLOCATE(Type) ALLOCATE_ARRAY(Type, 1)
#define DEALLOCATE(Pointer) DeallocateMemory(&RenderingAllocators::RenderingSystemAllocator, Pointer)

#define ALLOCATE_COMMAND(Type) ReinterpretCast(Type*, AllocateMemory(&RenderingAllocators::CommandAllocator, sizeof(Type)))

		DeviceInterface::DeviceInterface(Type Type) :
			m_Type(Type),
			m_Device(nullptr),
			m_Textures(&RenderingAllocators::RenderingSystemAllocator),
			m_Programs(&RenderingAllocators::RenderingSystemAllocator),
			m_Windows(&RenderingAllocators::RenderingSystemAllocator),
			m_Commands(&RenderingAllocators::RenderingSystemAllocator)
		{
			ProgramConstantSupplier::Create(&RenderingAllocators::RenderingSystemAllocator);
		}

		DeviceInterface::~DeviceInterface(void)
		{
			for each (auto item in m_Textures)
				DestroyTextureInternal(item);

			for each (auto item in m_Programs)
				DestroyProgramInternal(item);

			for each (auto item in m_Windows)
				DestroyWindowInternal(item);

			if (m_Device != nullptr)
			{
				m_Device->~IDevice();
				DeallocateMemory(&RenderingAllocators::RenderingSystemAllocator, m_Device);
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

		Texture *DeviceInterface::CreateTexture2D(const byte *Data, uint32 Width, uint32 Height, Texture::Formats Format)
		{
			Texture *texture = CreateTexture2DInternal(Data, Width, Height, Format);

			m_Textures.Add(texture);

			return texture;
		}

		void DeviceInterface::DestroyTexture(Texture *Texture)
		{
			m_Textures.Remove(Texture);
			
			DestroyTextureInternal(Texture);
		}

		RenderTarget * DeviceInterface::CreateRenderTarget(const RenderTargetInfo *Info)
		{
			RenderTarget *texture = CreateRenderTargetInternal(Info);;

			m_RenderTargets.Add(texture);

			return texture;
		}

		void DeviceInterface::DestroyRenderTarget(RenderTarget * RenderTarget)
		{
			m_RenderTargets.Remove(RenderTarget);

			DestroyRenderTargetInternal(RenderTarget);
		}

		void DeviceInterface::SetRenderTarget(RenderTarget * RenderTarget)
		{
			SwitchRenderTargetCommand *cmd = ALLOCATE_COMMAND(SwitchRenderTargetCommand);
			new (cmd) SwitchRenderTargetCommand(RenderTarget);
			m_Commands.Add(cmd);
		}

		Program *DeviceInterface::CreateProgram(const String &Shader)
		{
			Program *program = CreateProgramInternal(Shader);

			m_Programs.Add(program);

			return program;
		}

		void DeviceInterface::DestroyProgram(Program *Program)
		{
			m_Programs.Remove(Program);

			DestroyProgramInternal(Program);
		}

		Mesh *DeviceInterface::CreateMesh(const MeshInfo *Info, IDevice::BufferUsages Usage)
		{
			Mesh *mesh = CreateMeshInternal(Info, Usage);

			return mesh;
		}

		void DeviceInterface::DestroyMesh(Mesh *Mesh)
		{
			DestroyMeshInternal(Mesh);
		}

		Window *DeviceInterface::CreateWindow(uint16 Width, uint16 Height, cstr Title)
		{
			Window *window = CreateWindowInternal(Width, Height, Title);

			m_Windows.Add(window);

			return window;
		}

		void DeviceInterface::DestroyWindow(Window *Window)
		{
			m_Windows.Remove(Window);

			DestroyWindowInternal(Window);
		}

		void DeviceInterface::Clear(IDevice::ClearFlags Flags, Color Color)
		{
			ClearCommand *cmd = ALLOCATE_COMMAND(ClearCommand);
			new (cmd) ClearCommand(Flags, Color);
			m_Commands.Add(cmd);
		}

		void DeviceInterface::DrawMesh(MeshHandle * Mesh, const Matrix4F & Transform, ProgramHandle * Program)
		{
			DrawCommand *cmd = ALLOCATE_COMMAND(DrawCommand);
			new (cmd) DrawCommand(Mesh, Transform, Program);
			m_Commands.Add(cmd);
		}

		void DeviceInterface::DrawMesh(MeshHandle * Mesh, const Matrix4F & Transform, Material * Material)
		{
			for each (auto & pass in Material->GetPasses())
			{
				DrawCommand *cmd = ALLOCATE_COMMAND(DrawCommand);
				new (cmd) DrawCommand(Mesh, Transform, ConstCast(Pass*, &pass));
				m_Commands.Add(cmd);
			}
		}

		void DeviceInterface::SubmitCommands(void)
		{
			CHECK_DEVICE();

			for each (auto command in m_Commands)
				command->Execute(m_Device);

			EraseCommands();
		}

		void DeviceInterface::BeginRender(void)
		{
		}

		void DeviceInterface::EndRender(void)
		{
			CHECK_DEVICE();

			for each (auto window in m_Windows)
				m_Device->SwapBuffers(window->GetHandle());

			m_Device->PollEvents();
		}

		Texture *DeviceInterface::CreateTexture2DInternal(const byte *Data, uint32 Width, uint32 Height, Texture::Formats Format)
		{
			CHECK_DEVICE();

			Texture::Handle handle;
			CHECK_CALL(m_Device->CreateTexture2D(Data, Width, Height, Format, handle));

			Texture *texture = ALLOCATE(Texture);
			new (texture) Texture(m_Device, handle);

			return texture;
		}

		void DeviceInterface::DestroyTextureInternal(Texture *Texture)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyTexture(Texture->GetHandle()));
			Texture->~Texture();
			DeallocateMemory(&RenderingAllocators::RenderingSystemAllocator, Texture);
		}

		RenderTarget * DeviceInterface::CreateRenderTargetInternal(const RenderTargetInfo *Info)
		{
			CHECK_DEVICE();

			RenderTarget::Handle handle;
			IDevice::TextureList texturesHandle;
			CHECK_CALL(m_Device->CreateRenderTarget(Info, handle, texturesHandle));

			RenderTarget::TexturesList textureList;

			for each(auto texHandle in texturesHandle)
				textureList.Add({ m_Device, texHandle });

			RenderTarget *texture = ALLOCATE(RenderTarget);
			new (texture) RenderTarget(m_Device, handle, textureList);

			return texture;
		}

		void DeviceInterface::DestroyRenderTargetInternal(RenderTarget * RenderTarget)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyRenderTarget(RenderTarget->GetHandle()));
			RenderTarget->~RenderTarget();
			DeallocateMemory(&RenderingAllocators::RenderingSystemAllocator, RenderTarget);
		}

		Program *DeviceInterface::CreateProgramInternal(const String &Shader)
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

			return program;
		}

		void DeviceInterface::DestroyProgramInternal(Program *Program)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyProgram(Program->GetHandle()));
			Program->~Program();
			DeallocateMemory(&RenderingAllocators::RenderingSystemAllocator, Program);
		}

		Mesh *DeviceInterface::CreateMeshInternal(const MeshInfo *Info, IDevice::BufferUsages Usage)
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

		void DeviceInterface::DestroyMeshInternal(Mesh *Mesh)
		{
			CHECK_DEVICE();

			for (uint16 i = 0; i < Mesh->GetSubMeshCount(); ++i)
				CHECK_CALL(m_Device->DestroyMesh(Mesh->GetSubMeshes()[i].GetBuffer().GetHandle()));

			DeallocateMemory(&RenderingAllocators::RenderingSystemAllocator, Mesh->GetSubMeshes());
			Mesh->~Mesh();
			DeallocateMemory(&RenderingAllocators::RenderingSystemAllocator, Mesh);
		}

		Window *DeviceInterface::CreateWindowInternal(uint16 Width, uint16 Height, cstr Title)
		{
			CHECK_DEVICE();

			Window::Handle handle;
			CHECK_CALL(m_Device->CreateWindow(Width, Height, Title, handle));

			Window *window = ALLOCATE(Window);
			new (window) Window(m_Device, handle);

			return window;
		}

		void DeviceInterface::DestroyWindowInternal(Window *Window)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyWindow(Window->GetHandle()));
			Window->~Window();
			DeallocateMemory(&RenderingAllocators::RenderingSystemAllocator, Window);
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

			RenderingAllocators::CommandAllocator.Reset();
		}
	}
}
