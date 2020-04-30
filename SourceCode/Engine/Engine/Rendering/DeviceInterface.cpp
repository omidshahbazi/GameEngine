// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\DeviceInterface.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Sprite.h>
#include <Rendering\Shader.h>
#include <Rendering\Mesh.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Rendering\Private\OpenGL\OpenGLDevice.h>
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <Rendering\Private\Commands\ClearCommand.h>
#include <Rendering\Private\Commands\DrawCommand.h>
#include <Rendering\Private\Commands\SwitchRenderTargetCommand.h>
#include <Rendering\Private\Pipeline\PipelineManager.h>
#include <Rendering\ShaderConstantSupplier.h>
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
		BaseType* AllocateArray(uint32 Count)
		{
			return ReinterpretCast(BaseType*, AllocateMemory(&RenderingAllocators::RenderingSystemAllocator, Count * sizeof(BaseType)));
		}

		template<typename BaseType>
		BaseType* Allocate(void)
		{
			return AllocateArray<BaseType>(1);
		}

		template<typename BaseType>
		void Deallocate(BaseType* Ptr)
		{
			DeallocateMemory(&RenderingAllocators::RenderingSystemAllocator, Ptr);
		}

		template<typename BaseType>
		BaseType* AllocateCommand(RenderQueues Queue)
		{
			return ReinterpretCast(BaseType*, AllocateMemory(RenderingAllocators::CommandAllocators[(int8)Queue], sizeof(BaseType)));
		}

		DeviceInterface::DeviceInterface(Type Type) :
			m_Type(Type),
			m_Device(nullptr),
			m_Context(nullptr),
			m_Window(nullptr),
			m_Textures(&RenderingAllocators::RenderingSystemAllocator),
			m_RenderTargets(&RenderingAllocators::RenderingSystemAllocator),
			m_Shaders(&RenderingAllocators::RenderingSystemAllocator)
		{
			ShaderConstantSupplier::Create(&RenderingAllocators::RenderingSystemAllocator);
			PipelineManager::Create(&RenderingAllocators::RenderingSystemAllocator);

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

			for each (auto item in m_Shaders)
				DestroyShaderInternal(item);

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

			ShaderConstantSupplier::GetInstance()->Initialize();

			PipelineManager::GetInstance()->Initialize(this);

			CALL_CALLBACK(IListener, OnWindowChanged, m_Window)
		}

		RenderContext* DeviceInterface::CreateContext(Window* Window)
		{
			if (Window == nullptr)
				return nullptr;

			CHECK_DEVICE();

			RenderContext* context = m_Device->CreateContext(Window->GetHandle());

			m_ContextWindows[context] = Window;

			return context;
		}

		void DeviceInterface::SetContext(RenderContext* Context)
		{
			if (Context == nullptr)
				return;

			CHECK_DEVICE();

			Assert(m_ContextWindows.Contains(Context), "Window that pair to Context doesn't exists");

			Window* window = m_ContextWindows[Context];

			if (m_Window != nullptr)
				m_Window->RemoveListener(this);

			CHECK_CALL(m_Device->SetContext(Context));

			m_Context = Context;
			m_Window = window;

			if (m_Window != nullptr)
			{
				m_Window->AddListener(this);

				m_Device->SetViewport(Vector2I::Zero, m_Window->GetClientSize());
			}

			CALL_CALLBACK(IListener, OnWindowChanged, m_Window)
		}

		Texture* DeviceInterface::CreateTexture2D(const Vector2I& Dimension, Texture::Formats Format, const byte* Data)
		{
			Texture* texture = CreateTexture2DInternal(Data, Dimension, Format);

			m_Textures.Add(texture);

			return texture;
		}

		Sprite* DeviceInterface::CreateSprite(const Vector2I& Dimension, const Vector4I& Borders, Texture::Formats Format, const byte* Data)
		{
			CHECK_DEVICE();

			Sprite::Handle handle;
			CHECK_CALL(m_Device->CreateTexture(Texture::Types::TwoD, Data, Dimension.X, Dimension.Y, Format, handle));

			Sprite* sprite = Allocate<Sprite>();
			new (sprite) Sprite(m_Device, handle, Texture::Types::TwoD, Format, Dimension, Borders);

			if (Data != nullptr)
				sprite->GenerateMipMaps();

			return sprite;
		}

		void DeviceInterface::DestroyTexture(Texture* Texture)
		{
			m_Textures.Remove(Texture);

			DestroyTextureInternal(Texture);
		}

		RenderTarget* DeviceInterface::CreateRenderTarget(const IDevice::RenderTargetInfo* Info)
		{
			RenderTarget* texture = CreateRenderTargetInternal(Info);;

			m_RenderTargets.Add(texture);

			return texture;
		}

		void DeviceInterface::DestroyRenderTarget(RenderTarget* RenderTarget)
		{
			m_RenderTargets.Remove(RenderTarget);

			DestroyRenderTargetInternal(RenderTarget);
		}

		void DeviceInterface::SetRenderTarget(RenderTarget* RenderTarget, RenderQueues Queue)
		{
			SwitchRenderTargetCommand* cmd = AllocateCommand<SwitchRenderTargetCommand>(Queue);
			new (cmd) SwitchRenderTargetCommand(RenderTarget);
			AddCommand(m_CommandQueues, Queue, cmd);
		}

		Shader* DeviceInterface::CreateShader(const String& Source, String* Message)
		{
			Shader* shader = CreateShaderInternal(Source, Message);

			if (shader == nullptr)
				return nullptr;

			m_Shaders.Add(shader);

			return shader;
		}

		void DeviceInterface::DestroyShader(Shader* Shader)
		{
			m_Shaders.Remove(Shader);

			DestroyShaderInternal(Shader);
		}

		Mesh* DeviceInterface::CreateMesh(const MeshInfo* Info, GPUBuffer::Usages Usage)
		{
			Mesh* mesh = CreateMeshInternal(Info, Usage);

			return mesh;
		}

		void DeviceInterface::DestroyMesh(Mesh* Mesh)
		{
			DestroyMeshInternal(Mesh);
		}

		void DeviceInterface::Clear(IDevice::ClearFlags Flags, const ColorUI8& Color, RenderQueues Queue)
		{
			ClearCommand* cmd = AllocateCommand<ClearCommand>(Queue);
			new (cmd) ClearCommand(Flags, Color);
			AddCommand(m_CommandQueues, Queue, cmd);
		}

		void DeviceInterface::DrawMesh(Mesh* Mesh, const Matrix4F& Transform, Shader* Shader, RenderQueues Queue)
		{
			if (Mesh == nullptr)
				return;

			static Matrix4F id;
			id = Matrix4F::Identity;

			DrawMesh(Mesh, id, id, id, Transform, Shader, Queue);
		}

		void DeviceInterface::DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, Shader* Shader, RenderQueues Queue)
		{
			if (Mesh == nullptr)
				return;

			Matrix4F mvp = Projection;
			mvp *= View;
			mvp *= Model;

			DrawMesh(Mesh, Model, View, Projection, mvp, Shader, Queue);
		}

		void DeviceInterface::DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Shader* Shader, RenderQueues Queue)
		{
			if (Mesh == nullptr)
				return;

			DrawCommand* cmd = AllocateCommand<DrawCommand>(Queue);
			new (cmd) DrawCommand(Mesh, Model, View, Projection, MVP, Shader);
			AddCommand(m_CommandQueues, Queue, cmd);
		}

		void DeviceInterface::DrawMesh(Mesh* Mesh, const Matrix4F& Transform, Material* Material)
		{
			if (Mesh == nullptr)
				return;

			if (Material == nullptr)
				return;

			static Matrix4F id = Matrix4F::Identity;

			DrawMesh(Mesh, id, id, id, Transform, Material);
		}

		void DeviceInterface::DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Material* Material)
		{
			if (Mesh == nullptr)
				return;

			if (Material == nullptr)
				return;

			Matrix4F mvp = Projection;
			mvp *= View;
			mvp *= Model;

			DrawMesh(Mesh, Model, View, Projection, mvp, Material);
		}

		void DeviceInterface::DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material)
		{
			if (Mesh == nullptr)
				return;

			if (Material == nullptr)
				return;

			for each (auto & pass in Material->GetPasses())
			{
				auto queue = pass.GetQueue();

				DrawCommand* cmd = AllocateCommand<DrawCommand>(queue);
				new (cmd) DrawCommand(Mesh, Model, View, Projection, MVP, ConstCast(Pass*, &pass));
				AddCommand(m_CommandQueues, queue, cmd);
			}
		}

		void DeviceInterface::BeginRender(void)
		{
			PipelineManager::GetInstance()->BeginRender();
		}

		void DeviceInterface::EndRender(void)
		{
			CHECK_DEVICE();

			RenderQueue(RenderQueues::Default, RenderQueues::HUD);

			EraseQueue(RenderQueues::Default, RenderQueues::HUD);

			CHECK_CALL(m_Device->SwapBuffers());

			PipelineManager::GetInstance()->EndRender();
		}

		Texture* DeviceInterface::CreateTexture2DInternal(const byte* Data, const Vector2I& Dimension, Texture::Formats Format)
		{
			CHECK_DEVICE();

			Texture::Handle handle;
			CHECK_CALL(m_Device->CreateTexture(Texture::Types::TwoD, Data, Dimension.X, Dimension.Y, Format, handle));

			Texture* texture = Allocate<Texture>();
			new (texture) Texture(m_Device, handle, Texture::Types::TwoD, Format, Dimension);

			if (Data != nullptr)
				texture->GenerateMipMaps();

			return texture;
		}

		void DeviceInterface::DestroyTextureInternal(Texture* Texture)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyTexture(Texture->GetHandle()));
			Texture->~Texture();
			Deallocate(Texture);
		}

		RenderTarget* DeviceInterface::CreateRenderTargetInternal(const IDevice::RenderTargetInfo* Info)
		{
			CHECK_DEVICE();

			RenderTarget::Handle handle;
			IDevice::TextureList texturesHandle;
			CHECK_CALL(m_Device->CreateRenderTarget(Info, handle, texturesHandle));

			RenderTarget::TexturesList textureList;

			for (uint8 i = 0; i < Info->Textures.GetSize(); ++i)
			{
				const auto& info = Info->Textures[i];

				Texture tex(m_Device, texturesHandle[i], Texture::Types::TwoD, info.Format, { (int32)info.Width, (int32)info.Height });

				tex.GenerateMipMaps();

				textureList.Add(tex);
			}

			RenderTarget* texture = Allocate<RenderTarget>();
			new (texture) RenderTarget(m_Device, handle, textureList);

			return texture;
		}

		void DeviceInterface::DestroyRenderTargetInternal(RenderTarget* RenderTarget)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyRenderTarget(RenderTarget->GetHandle()));
			RenderTarget->~RenderTarget();
			Deallocate(RenderTarget);
		}

		Shader* DeviceInterface::CreateShaderInternal(const String& Source, String* Message)
		{
			static Compiler compiler;

			CHECK_DEVICE();

			String vertProgram;
			String fragProgram;
			compiler.Compile(m_Type, Source, vertProgram, fragProgram);

			Shader::Handle handle = 0;
			cstr message;
			CHECK_CALL(m_Device->CreateShader(vertProgram.GetValue(), fragProgram.GetValue(), handle, &message));

			if (handle == 0)
			{
				if (Message != nullptr)
					*Message = message;

				DebugLogError(message);

				return nullptr;
			}

			Shader* shader = Allocate<Shader>();
			new (shader) Shader(m_Device, handle);

			return shader;
		}

		void DeviceInterface::DestroyShaderInternal(Shader* Shader)
		{
			CHECK_DEVICE();

			CHECK_CALL(m_Device->DestroyShader(Shader->GetHandle()));
			Shader->~Shader();
			Deallocate(Shader);
		}

		Mesh* DeviceInterface::CreateMeshInternal(const MeshInfo* Info, GPUBuffer::Usages Usage)
		{
			CHECK_DEVICE();

			SubMesh* subMeshes = AllocateArray<SubMesh>(Info->SubMeshes.GetSize());

			for (uint16 i = 0; i < Info->SubMeshes.GetSize(); ++i)
			{
				SubMesh::Handle handle;

				auto& subMeshInfo = Info->SubMeshes[i];

				CHECK_CALL(m_Device->CreateMesh(subMeshInfo, Usage, handle));

				Construct(&subMeshes[i], m_Device, handle, subMeshInfo->Vertices.GetSize(), subMeshInfo->Indices.GetSize(), subMeshInfo->Type, subMeshInfo->Layout);
			}

			Mesh* mesh = Allocate<Mesh>();
			Construct(mesh, subMeshes, Info->SubMeshes.GetSize());
			return mesh;
		}

		void DeviceInterface::DestroyMeshInternal(Mesh* Mesh)
		{
			CHECK_DEVICE();

			for (uint16 i = 0; i < Mesh->GetSubMeshCount(); ++i)
				CHECK_CALL(m_Device->DestroyMesh(Mesh->GetSubMeshes()[i].GetHandle()));

			Deallocate(Mesh->GetSubMeshes());
			Mesh->~Mesh();
			Deallocate(Mesh);
		}

		void DeviceInterface::RenderQueue(RenderQueues From, RenderQueues To)
		{
			for (int8 i = (int8)From; i <= (int8)To; ++i)
			{
				auto& commands = m_CommandQueues[i];

				for each (auto command in commands)
					command->Execute(m_Device);
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

		void DeviceInterface::OnSizeChanged(Window* Window)
		{
			CHECK_CALL(m_Device->SetViewport(Vector2I::Zero, Window->GetClientSize()));

			CALL_CALLBACK(IListener, OnWindowResized, m_Window)
		}
	}
}
