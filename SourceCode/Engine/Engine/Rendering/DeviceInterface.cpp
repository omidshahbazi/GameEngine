// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\DeviceInterface.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Texture.h>
#include <Rendering\Program.h>
#include <Rendering\Mesh.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Rendering\Private\OpenGL\OpenGLDevice.h>
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <Rendering\Private\Commands\ClearCommand.h>
#include <Rendering\Private\Commands\DrawCommand.h>
#include <Rendering\Private\Commands\SwitchRenderTargetCommand.h>
#include <Rendering\ProgramConstantSupplier.h>
#include <Rendering\Material.h>
#include <Utility\Window.h>

namespace Engine
{
	using namespace Utility;

	namespace Rendering
	{
		using namespace Private;
		using namespace Private::OpenGL;
		using namespace Private::ShaderCompiler;
		using namespace Private::Commands;

#define CHECK_DEVICE() Assert(m_Device != nullptr, "m_Device cannot be null")
#define CHECK_CALL(Experssion) if (!(Experssion)) Assert(false, m_Device->GetLastError());

		template<typename BaseType>
		BaseType *AllocateArray(uint32 Count)
		{
			return ReinterpretCast(BaseType*, AllocateMemory(&RenderingAllocators::RenderingSystemAllocator, Count * sizeof(BaseType)));
		}

		template<typename BaseType>
		BaseType *Allocate(void)
		{
			return AllocateArray<BaseType>(1);
		}

		template<typename BaseType>
		void Deallocate(BaseType *Ptr)
		{
			DeallocateMemory(&RenderingAllocators::RenderingSystemAllocator, Ptr);
		}

		template<typename BaseType>
		BaseType *AllocateCommand(void)
		{
			return ReinterpretCast(BaseType*, AllocateMemory(&RenderingAllocators::CommandAllocator, sizeof(BaseType)));
		}

		DeviceInterface::DeviceInterface(Type Type) :
			m_Type(Type),
			m_Device(nullptr),
			m_Textures(&RenderingAllocators::RenderingSystemAllocator),
			m_Programs(&RenderingAllocators::RenderingSystemAllocator),
			m_Commands(&RenderingAllocators::RenderingSystemAllocator, 10000000)
		{
			ProgramConstantSupplier::Create(&RenderingAllocators::RenderingSystemAllocator);

			switch (m_Type)
			{
			case Type::OpenGL:
			{
				m_Device = Allocate<OpenGLDevice>();
				new (m_Device) OpenGLDevice;
			} break;
			}
		}

		DeviceInterface::~DeviceInterface(void)
		{
			for each (auto item in m_Textures)
				DestroyTextureInternal(item);

			for each (auto item in m_Programs)
				DestroyProgramInternal(item);

			if (m_Device != nullptr)
			{
				m_Device->~IDevice();
				Deallocate(m_Device);
			}
		}

		void DeviceInterface::Initialize(void)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->Initialize());
		}

		void DeviceInterface::SetWindow(Window * Window)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->SetWindow(Window->GetHandle()));
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
			SwitchRenderTargetCommand *cmd = AllocateCommand<SwitchRenderTargetCommand>();
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

		void DeviceInterface::Clear(IDevice::ClearFlags Flags, Color Color)
		{
			ClearCommand *cmd = AllocateCommand<ClearCommand>();
			new (cmd) ClearCommand(Flags, Color);
			m_Commands.Add(cmd);
		}

		void DeviceInterface::DrawMesh(MeshHandle * Mesh, const Matrix4F & Transform, ProgramHandle * Program)
		{
			DrawCommand *cmd = AllocateCommand<DrawCommand>();
			new (cmd) DrawCommand(Mesh, Transform, Program);
			m_Commands.Add(cmd);
		}

		void DeviceInterface::DrawMesh(MeshHandle * Mesh, const Matrix4F & Transform, Material * Material)
		{
			for each (auto & pass in Material->GetPasses())
			{
				DrawCommand *cmd = AllocateCommand<DrawCommand>();
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

			m_Device->SwapBuffers();
		}

		Texture *DeviceInterface::CreateTexture2DInternal(const byte *Data, uint32 Width, uint32 Height, Texture::Formats Format)
		{
			CHECK_DEVICE();

			Texture::Handle handle;
			CHECK_CALL(m_Device->CreateTexture2D(Data, Width, Height, Format, handle));

			Texture *texture = Allocate<Texture>();
			new (texture) Texture(m_Device, handle);

			return texture;
		}

		void DeviceInterface::DestroyTextureInternal(Texture *Texture)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyTexture(Texture->GetHandle()));
			Texture->~Texture();
			Deallocate(Texture);
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

			RenderTarget *texture = Allocate<RenderTarget>();
			new (texture) RenderTarget(m_Device, handle, textureList);

			return texture;
		}

		void DeviceInterface::DestroyRenderTargetInternal(RenderTarget * RenderTarget)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyRenderTarget(RenderTarget->GetHandle()));
			RenderTarget->~RenderTarget();
			Deallocate(RenderTarget);
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

			Program *program = Allocate<Program>();
			new (program) Program(m_Device, handle);

			return program;
		}

		void DeviceInterface::DestroyProgramInternal(Program *Program)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyProgram(Program->GetHandle()));
			Program->~Program();
			Deallocate(Program);
		}

		Mesh *DeviceInterface::CreateMeshInternal(const MeshInfo *Info, IDevice::BufferUsages Usage)
		{
			CHECK_DEVICE();

			Mesh::SubMesh *subMeshes = AllocateArray<Mesh::SubMesh>(Info->SubMeshes.GetSize());

			for (uint16 i = 0; i < Info->SubMeshes.GetSize(); ++i)
			{
				GPUBuffer::Handle handle;

				auto &subMeshInfo = Info->SubMeshes[i];

				CHECK_CALL(m_Device->CreateMesh(&subMeshInfo, Usage, handle));

				new (&subMeshes[i]) Mesh::SubMesh(GPUBuffer(m_Device, handle, subMeshInfo.Vertices.GetSize()), subMeshInfo.Indices.GetSize());
			}

			Mesh *mesh = Allocate<Mesh>();
			new (mesh) Mesh(subMeshes, Info->SubMeshes.GetSize());
			return mesh;
		}

		void DeviceInterface::DestroyMeshInternal(Mesh *Mesh)
		{
			CHECK_DEVICE();

			for (uint16 i = 0; i < Mesh->GetSubMeshCount(); ++i)
				CHECK_CALL(m_Device->DestroyMesh(Mesh->GetSubMeshes()[i].GetBuffer().GetHandle()));

			Deallocate(Mesh->GetSubMeshes());
			Mesh->~Mesh();
			Deallocate(Mesh);
		}

		void DeviceInterface::EraseCommands(void)
		{
			m_Commands.Clear();

			RenderingAllocators::CommandAllocator.Reset();
		}
	}
}
