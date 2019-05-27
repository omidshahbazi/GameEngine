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
#include <Rendering\Private\Pipeline\DeferredRendering.h>
#include <Rendering\ProgramConstantSupplier.h>
#include <Rendering\Material.h>

namespace Engine
{
	using namespace Utility;

	namespace Rendering
	{
		using namespace Private;
		using namespace Private::OpenGL;
		using namespace Private::ShaderCompiler;
		using namespace Private::Commands;
		using namespace Private::Pipeline;

#define CHECK_DEVICE() Assert(m_Device != nullptr, "m_Device cannot be null")
#define CHECK_CALL(Expression) if (!(Expression)) Assert(false, #Expression);

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
		BaseType *AllocateCommand(RenderQueues Queue)
		{
			return ReinterpretCast(BaseType*, AllocateMemory(RenderingAllocators::CommandAllocators[(int8)Queue], sizeof(BaseType)));
		}

		INLINE void AddCommand(DeviceInterface::CommandList *Commands, RenderQueues Queue, CommandBase *Command)
		{
			//Commands[(int8)Queue].Add(Command);
			Commands[0].Add(Command);
		}

		DeviceInterface::DeviceInterface(Type Type) :
			m_Type(Type),
			m_Device(nullptr),
			m_Textures(&RenderingAllocators::RenderingSystemAllocator),
			m_Programs(&RenderingAllocators::RenderingSystemAllocator)
		{
			ProgramConstantSupplier::Create(&RenderingAllocators::RenderingSystemAllocator);
			DeferredRendering::Create(&RenderingAllocators::RenderingSystemAllocator);

			switch (m_Type)
			{
			case Type::OpenGL:
			{
				m_Device = Allocate<OpenGLDevice>();
				new (m_Device) OpenGLDevice;
			} break;
			}

			for (int8 i = 0; i < (int8)RenderQueues::COUNT; ++i)
				m_CommandQueues[i] = CommandList(&RenderingAllocators::RenderingSystemAllocator, 10000000);
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

			ProgramConstantSupplier::GetInstance()->Initialize();
			DeferredRendering::GetInstance()->Initialize();
		}

		void DeviceInterface::SetWindow(Window * Window)
		{
			CHECK_DEVICE();

			Window->AddListener(this);

			CHECK_CALL(m_Device->SetWindow(Window->GetHandle()));

			m_Window = Window;
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

		void DeviceInterface::SetRenderTarget(RenderTarget * RenderTarget, RenderQueues Queue)
		{
			SwitchRenderTargetCommand *cmd = AllocateCommand<SwitchRenderTargetCommand>(Queue);
			new (cmd) SwitchRenderTargetCommand(RenderTarget);
			AddCommand(m_CommandQueues, Queue, cmd);
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

		void DeviceInterface::Clear(IDevice::ClearFlags Flags, Color Color, RenderQueues Queue)
		{
			ClearCommand *cmd = AllocateCommand<ClearCommand>(Queue);
			new (cmd) ClearCommand(Flags, Color);
			AddCommand(m_CommandQueues, Queue, cmd);
		}

		void DeviceInterface::DrawMesh(Mesh * Mesh, const Matrix4F & Transform, Program * Program, RenderQueues Queue)
		{
			static Matrix4F id;
			id.MakeIdentity();

			DrawMesh(Mesh, id, id, id, Transform, Program, Queue);
		}

		void DeviceInterface::DrawMesh(Mesh * Mesh, const Matrix4F & Model, const Matrix4F & View, const Matrix4F & Projection, const Matrix4F & MVP, Program * Program, RenderQueues Queue)
		{
			DrawCommand *cmd = AllocateCommand<DrawCommand>(Queue);
			new (cmd) DrawCommand(Mesh, Model, View, Projection, MVP, Program);
			AddCommand(m_CommandQueues, Queue, cmd);
		}

		void DeviceInterface::DrawMesh(Mesh * Mesh, const Matrix4F & Transform, Material * Material)
		{
			static Matrix4F id;
			id.MakeIdentity();

			DrawMesh(Mesh, id, id, id, Transform, Material);
		}

		void DeviceInterface::DrawMesh(Mesh * Mesh, const Matrix4F & Model, const Matrix4F & View, const Matrix4F & Projection, const Matrix4F & MVP, Material * Material)
		{
			if (Mesh == nullptr)
				return;

			if (Material == nullptr)
				return;

			RenderQueues queue = Material->GetQueue();

			for each (auto & pass in Material->GetPasses())
			{
				DrawCommand *cmd = AllocateCommand<DrawCommand>(queue);
				new (cmd) DrawCommand(Mesh, Model, View, Projection, MVP, ConstCast(Pass*, &pass));
				AddCommand(m_CommandQueues, queue, cmd);
			}
		}

		void DeviceInterface::BeginRender(void)
		{
			//SetRenderTarget(DeferredRendering::GetInstance()->GetGBufferMRT(), RenderQueues::Geometry);
			//Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer, Color(0, 0, 0, 255), RenderQueues::Geometry);

			//SetRenderTarget(nullptr, RenderQueues::Lighting);
			Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer, Color(0, 0, 0, 255), RenderQueues::Lighting);
		}

		void DeviceInterface::EndRender(void)
		{
			CHECK_DEVICE();

			RenderQueue(RenderQueues::Default, RenderQueues::HUD);

			EraseQueue(RenderQueues::Default, RenderQueues::HUD);

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

				CHECK_CALL(m_Device->CreateMesh(subMeshInfo, Usage, handle));

				new (&subMeshes[i]) Mesh::SubMesh(GPUBuffer(m_Device, handle, subMeshInfo->Vertices.GetSize()), subMeshInfo->Indices.GetSize(), subMeshInfo->Type, subMeshInfo->Layout);
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

		void DeviceInterface::RenderQueue(RenderQueues From, RenderQueues To)
		{
			for (int8 i = (int8)From; i <= (int8)To; ++i)
			{
				auto &commands = m_CommandQueues[i];

				//for each (auto command in commands)
					for (int8 j = 0; j < commands.GetSize(); ++j)
						commands[j]->Execute(m_Device);
			}
		}

		void DeviceInterface::EraseQueue(RenderQueues From, RenderQueues To)
		{
			for (int8 i = (int8)From; i <= (int8)To; ++i)
			{
				m_CommandQueues[i].Clear();

				RenderingAllocators::CommandAllocators[i]->Reset();
			}
		}

		void DeviceInterface::OnWindowResized(Window * Window)
		{
			m_Device->ResizeViewport(Window->GetSize());
		}
	}
}
