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

#define CHECK_CALL(PromiseExpr) \
		m_ThreadedDevice->Lock(); \
		auto promise = PromiseExpr; \
		m_ThreadedDevice->Release(); \
		if (!(promise).Wait()) \
			Assert(false, #PromiseExpr);

		template<typename BaseType>
		BaseType* AllocateCommand(RenderQueues Queue)
		{
			return ReinterpretCast(BaseType*, AllocateMemory(RenderingAllocators::CommandAllocators[(int8)Queue], sizeof(BaseType)));
		}

		//TODO: Implement DirectX
		//TODO: Implement Vulkan
		DeviceInterface::DeviceInterface(DeviceTypes DeviceType) :
			m_DeviceType(DeviceType),
			m_Device(nullptr),
			m_ThreadedDevice(nullptr),
			m_CurentContext(nullptr),
			m_Window(nullptr)
		{
			Compiler::Create(RenderingAllocators::RenderingSystemAllocator);
			ShaderConstantSupplier::Create(RenderingAllocators::RenderingSystemAllocator);
			PipelineManager::Create(RenderingAllocators::RenderingSystemAllocator);

			switch (m_DeviceType)
			{
			case DeviceTypes::OpenGL:
			{
				m_Device = RenderingAllocators::RenderingSystemAllocator_Allocate<OpenGLDevice>();
				Construct(ReinterpretCast(OpenGLDevice*, m_Device));
			} break;
			}

			for (int8 i = 0; i < (int8)RenderQueues::COUNT; ++i)
				m_CommandQueues[i] = CommandList(RenderingAllocators::ContainersAllocator, ThreadedDevice::CommandPerQueueCount);

			m_ThreadedDevice = RenderingAllocators::RenderingSystemAllocator_Allocate<ThreadedDevice>();
			Construct(m_ThreadedDevice, m_Device, m_DeviceType);
		}

		DeviceInterface::~DeviceInterface(void)
		{
			PipelineManager::Destroy();
			ShaderConstantSupplier::Destroy();
			Compiler::Destroy();

			if (m_Device != nullptr)
				RenderingAllocators::RenderingSystemAllocator_Deallocate(m_Device);
		}

		//TODO: secure all Initialize/Deinitialize functions
		void DeviceInterface::Initialize(void)
		{
			CHECK_CALL(m_ThreadedDevice->Initialize());

			ShaderConstantSupplier::GetInstance()->Initialize();

			PipelineManager::GetInstance()->Initialize(this);

			//TODO: why this?
			CALL_CALLBACK(IListener, OnWindowChanged, m_Window)
		}

		cstr DeviceInterface::GetVersion(void)
		{
			CHECK_CALL(m_ThreadedDevice->GetVersion());

			return promise.GetValue();
		}

		cstr DeviceInterface::GetVendorName(void)
		{
			CHECK_CALL(m_ThreadedDevice->GetVendorName());

			return promise.GetValue();
		}

		cstr DeviceInterface::GetRendererName(void)
		{
			CHECK_CALL(m_ThreadedDevice->GetRendererName());

			return promise.GetValue();
		}

		cstr DeviceInterface::GetShadingLanguageVersion(void)
		{
			CHECK_CALL(m_ThreadedDevice->GetShadingLanguageVersion());

			return promise.GetValue();
		}

		RenderContext* DeviceInterface::CreateContext(Window* Window)
		{
			CHECK_CALL(m_ThreadedDevice->CreateContext(Window->GetHandle()));

			RenderContext* context = promise.GetValue();

			m_ContextWindows[context] = Window;

			return context;
		}

		RenderContext* DeviceInterface::CreateDummyContext(void)
		{
			Window* window = RenderingAllocators::RenderingSystemAllocator_Allocate<Window>();
			Construct(window, "DummyContextWindow");
			window->Initialize();
			window->SetIsVisible(false);

			RenderContext* context = CreateContext(window);

			m_DummyContextWindows[context] = window;

			return context;
		}

		void DeviceInterface::DestroyContext(RenderContext* Context)
		{
			m_ContextWindows.Remove(Context);

			DestroyContextInternal(Context);
		}

		void DeviceInterface::SetContext(RenderContext* Context)
		{
			Assert(Context == nullptr || m_ContextWindows.Contains(Context), "Window that pair to Context doesn't exists");

			if (m_Window != nullptr)
				m_Window->RemoveListener(this);

			Window* window = nullptr;
			if (Context != nullptr)
				window = m_ContextWindows.Get(Context);

			CHECK_CALL(m_ThreadedDevice->SetContext(Context));

			m_CurentContext = Context;
			m_Window = window;

			if (m_Window != nullptr)
			{
				m_Window->AddListener(this);

				m_ThreadedDevice->SetViewport(Vector2I::Zero, m_Window->GetClientSize());
			}

			CALL_CALLBACK(IListener, OnWindowChanged, m_Window)
		}

		RenderContext* DeviceInterface::GetContext(void)
		{
			CHECK_CALL(m_ThreadedDevice->GetContext());

			return promise.GetValue();
		}

		Texture* DeviceInterface::CreateTexture(const TextureInfo* Info)
		{
			Texture* texture = CreateTextureInternal(Info);

			return texture;
		}

		Sprite* DeviceInterface::CreateSprite(const TextureInfo* Info)
		{
			Sprite::Handle handle;
			CHECK_CALL(m_ThreadedDevice->CreateTexture(Info, handle));

			Sprite* sprite = RenderingAllocators::RenderingSystemAllocator_Allocate<Sprite>();
			ConstructMacro(Sprite, sprite, m_ThreadedDevice, handle, Texture::Types::TwoD, Info->Format, Info->Dimension, Info->Borders);

			if (Info->Data != nullptr)
				sprite->GenerateMipMaps();

			return sprite;
		}

		void DeviceInterface::DestroyTexture(Texture* Texture)
		{
			DestroyTextureInternal(Texture);
		}

		RenderTarget* DeviceInterface::CreateRenderTarget(const RenderTargetInfo* Info)
		{
			RenderTarget* texture = CreateRenderTargetInternal(Info);;

			return texture;
		}

		void DeviceInterface::DestroyRenderTarget(RenderTarget* RenderTarget)
		{
			DestroyRenderTargetInternal(RenderTarget);
		}

		void DeviceInterface::SetRenderTarget(RenderTarget* RenderTarget, RenderQueues Queue)
		{
			SwitchRenderTargetCommand* cmd = AllocateCommand<SwitchRenderTargetCommand>(Queue);
			ConstructMacro(SwitchRenderTargetCommand, cmd, RenderTarget);
			AddCommand(m_CommandQueues, Queue, cmd);
		}

		Shader* DeviceInterface::CreateShader(const ShaderInfo* Info, String* Message)
		{
			Shader* shader = CreateShaderInternal(Info, Message);

			return shader;
		}

		void DeviceInterface::DestroyShader(Shader* Shader)
		{
			DestroyShaderInternal(Shader);
		}

		Mesh* DeviceInterface::CreateMesh(const MeshInfo* Info, GPUBuffer::Usages Usage)
		{
			return CreateMeshInternal(Info, Usage);
		}

		void DeviceInterface::DestroyMesh(Mesh* Mesh)
		{
			DestroyMeshInternal(Mesh);
		}

		void DeviceInterface::Clear(IDevice::ClearFlags Flags, const ColorUI8& Color, RenderQueues Queue)
		{
			ClearCommand* cmd = AllocateCommand<ClearCommand>(Queue);
			ConstructMacro(ClearCommand, cmd, Flags, Color);
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
			ConstructMacro(DrawCommand, cmd, Mesh, Model, View, Projection, MVP, Shader);
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
				ConstructMacro(DrawCommand, cmd, Mesh, Model, View, Projection, MVP, ConstCast(Pass*, &pass));
				AddCommand(m_CommandQueues, queue, cmd);
			}
		}

		void DeviceInterface::BeginRender(void)
		{
			PipelineManager::GetInstance()->BeginRender();
		}

		void DeviceInterface::EndRender(void)
		{
			m_ThreadedDevice->Lock();

			RenderQueue(RenderQueues::Default, RenderQueues::HUD);

			EraseQueue(RenderQueues::Default, RenderQueues::HUD);

			m_ThreadedDevice->SwapBuffers();

			m_ThreadedDevice->Release();

			PipelineManager::GetInstance()->EndRender();
		}

		void DeviceInterface::SetDebugCallback(IDevice::DebugProcedureType Callback)
		{
			m_ThreadedDevice->SetDebugCallback(Callback).Wait();
		}

		void DeviceInterface::DestroyContextInternal(RenderContext* Context)
		{
			if (m_CurentContext == Context && m_CurentContext != nullptr && m_ContextWindows.Contains(Context))
				m_ContextWindows.Get(Context)->RemoveListener(this);

			m_ThreadedDevice->DestroyContext(Context);
		}

		Texture* DeviceInterface::CreateTextureInternal(const TextureInfo* Info)
		{
			Texture::Handle handle;
			CHECK_CALL(m_ThreadedDevice->CreateTexture(Info, handle));

			Texture* texture = RenderingAllocators::RenderingSystemAllocator_Allocate<Texture>();
			ConstructMacro(Texture, texture, m_ThreadedDevice, handle, Info->Type, Info->Format, Info->Dimension);

			if (Info->Data != nullptr)
				texture->GenerateMipMaps();

			return texture;
		}

		void DeviceInterface::DestroyTextureInternal(Texture* Texture)
		{
			CHECK_CALL(m_ThreadedDevice->DestroyTexture(Texture->GetHandle()));

			RenderingAllocators::RenderingSystemAllocator_Deallocate(Texture);
		}

		RenderTarget* DeviceInterface::CreateRenderTargetInternal(const RenderTargetInfo* Info)
		{
			RenderTarget::Handle handle;
			IDevice::TextureList texturesHandle;
			CHECK_CALL(m_ThreadedDevice->CreateRenderTarget(Info, handle, texturesHandle));

			RenderTarget::TexturesList textureList;

			for (uint8 i = 0; i < Info->Textures.GetSize(); ++i)
			{
				const auto& info = Info->Textures[i];

				Texture* tex = RenderingAllocators::RenderingSystemAllocator_Allocate<Texture>();
				ConstructMacro(Texture, tex, m_ThreadedDevice, texturesHandle[i], Texture::Types::TwoD, info.Format, info.Dimension);

				tex->GenerateMipMaps();

				textureList.Add(tex);
			}

			RenderTarget* texture = RenderingAllocators::RenderingSystemAllocator_Allocate<RenderTarget>();
			ConstructMacro(RenderTarget, texture, m_ThreadedDevice, handle, textureList);

			return texture;
		}

		void DeviceInterface::DestroyRenderTargetInternal(RenderTarget* RenderTarget)
		{
			CHECK_CALL(m_ThreadedDevice->DestroyRenderTarget(RenderTarget->GetHandle()));

			auto textures = RenderTarget->GetTextures();
			for each (auto texture in textures)
				RenderingAllocators::RenderingSystemAllocator_Deallocate(texture);

			RenderingAllocators::RenderingSystemAllocator_Deallocate(RenderTarget);
		}

		Shader* DeviceInterface::CreateShaderInternal(const ShaderInfo* Info, String* Message)
		{
			if (Info->Source.GetLength() == 0)
				return nullptr;

			cstr shadingLanguageVersion;
			{
				CHECK_CALL(m_ThreadedDevice->GetShadingLanguageVersion());
				shadingLanguageVersion = promise.Wait();
			}

			String vertShader;
			String fragShader;
			if (!Compiler::GetInstance()->Compile(m_DeviceType, shadingLanguageVersion, Info, vertShader, fragShader))
			{
				if (Message != nullptr)
					*Message = "Compile failed in Compiler";

				return nullptr;
			}

			IDevice::Shaders shaders;
			shaders.VertexShader = vertShader.GetValue();
			shaders.FragmentShader = fragShader.GetValue();

			Shader::Handle handle = 0;
			cstr message;
			CHECK_CALL(m_ThreadedDevice->CreateShader(&shaders, handle, &message));

			if (handle == 0)
			{
				if (Message != nullptr)
					*Message = message;

				DebugLogError(message);

				return nullptr;
			}

			Shader* shader = RenderingAllocators::RenderingSystemAllocator_Allocate<Shader>();
			ConstructMacro(Shader, shader, m_ThreadedDevice, handle);

			return shader;
		}

		void DeviceInterface::DestroyShaderInternal(Shader* Shader)
		{
			CHECK_CALL(m_ThreadedDevice->DestroyShader(Shader->GetHandle()));

			RenderingAllocators::RenderingSystemAllocator_Deallocate(Shader);
		}

		Mesh* DeviceInterface::CreateMeshInternal(const MeshInfo* Info, GPUBuffer::Usages Usage)
		{
			SubMesh* subMeshes = RenderingAllocators::RenderingSystemAllocator_AllocateArray<SubMesh>(Info->SubMeshes.GetSize());
			uint16 subMeshIndex = 0;

			for (uint16 i = 0; i < Info->SubMeshes.GetSize(); ++i)
			{
				SubMesh::Handle handle;

				auto& subMeshInfo = Info->SubMeshes[i];

				if (subMeshInfo->Vertices.GetSize() == 0)
					continue;

				CHECK_CALL(m_ThreadedDevice->CreateMesh(subMeshInfo, Usage, handle));

				ConstructMacro(SubMesh, &subMeshes[subMeshIndex++], m_ThreadedDevice, handle, subMeshInfo->Vertices.GetSize(), subMeshInfo->Indices.GetSize(), subMeshInfo->Type, subMeshInfo->Layout);
			}

			Mesh* mesh = RenderingAllocators::RenderingSystemAllocator_Allocate<Mesh>();
			Construct(mesh, subMeshes, subMeshIndex);
			return mesh;
		}

		void DeviceInterface::DestroyMeshInternal(Mesh* Mesh)
		{
			for (uint16 i = 0; i < Mesh->GetSubMeshCount(); ++i)
			{
				CHECK_CALL(m_ThreadedDevice->DestroyMesh(Mesh->GetSubMeshes()[i].GetHandle()));
			}

			RenderingAllocators::RenderingSystemAllocator_Deallocate(Mesh->GetSubMeshes());

			RenderingAllocators::RenderingSystemAllocator_Deallocate(Mesh);
		}

		void DeviceInterface::RenderQueue(RenderQueues From, RenderQueues To)
		{
			for (int8 i = (int8)From; i <= (int8)To; ++i)
			{
				auto& commands = m_CommandQueues[i];

				for each (auto command in commands)
					command->Execute(m_ThreadedDevice);
			}
		}

		void DeviceInterface::EraseQueue(RenderQueues From, RenderQueues To)
		{
			for (int8 i = (int8)From; i <= (int8)To; ++i)
			{
				auto& commands = m_CommandQueues[i];

				for each (auto command in commands)
					DestructMacro(CommandBase, command);

				commands.Clear();

				RenderingAllocators::CommandAllocators[i]->Reset();
			}
		}

		void DeviceInterface::OnSizeChanged(Window* Window)
		{
			CHECK_CALL(m_ThreadedDevice->SetViewport(Vector2I::Zero, Window->GetClientSize()));

			CALL_CALLBACK(IListener, OnWindowResized, m_Window)
		}
	}
}
