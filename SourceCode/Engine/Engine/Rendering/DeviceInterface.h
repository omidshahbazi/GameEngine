// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef DEVICE_INTERFACE_H
#define DEVICE_INTERFACE_H

#include <Rendering\Private\ThreadedDevice.h>
#include <Rendering\ShaderInfo.h>
#include <Containers\ListenerContainer.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Utility;

	namespace Rendering
	{
		class Sprite;
		class Shader;
		class Material;
		class Mesh;
		struct Color;

		namespace Private
		{
			namespace Commands
			{
				class CommandBase;
			}
		}

		using namespace Private::Commands;

		class RENDERING_API DeviceInterface : private Window::IListener
		{
		private:
			typedef Map<RenderContext*, Window*> ContextWindowMap;

		public:
			class RENDERING_API IListener
			{
			public:
				virtual void OnWindowChanged(Window* Window)
				{
				}

				virtual void OnWindowResized(Window* Window)
				{
				}
			};

			LISTENER_DECLARATION(IListener)

		public:
			DeviceInterface(DeviceTypes DeviceType);
			~DeviceInterface(void);

			void Initialize(void);

			cstr GetVersion(void);
			cstr GetVendorName(void);
			cstr GetRendererName(void);
			cstr GetShadingLanguageVersion(void);

			RenderContext* CreateContext(Window* Window);
			RenderContext* CreateDummyContext(void);
			void DestroyContext(RenderContext* Context);
			void SetContext(RenderContext* Context);
			RenderContext* GetContext(void);
			Window* GetWindow(void)
			{
				return m_Window;
			}

			Texture* CreateTexture(const TextureInfo* Info);
			Sprite* CreateSprite(const TextureInfo* Info);
			void DestroyTexture(Texture* Texture);

			RenderTarget* CreateRenderTarget(const RenderTargetInfo* Info);
			void DestroyRenderTarget(RenderTarget* RenderTarget);
			void SetRenderTarget(RenderTarget* RenderTarget, RenderQueues Queue = RenderQueues::Default);

			Shader* CreateShader(const ShaderInfo* Info, String* Message = nullptr);
			void DestroyShader(Shader* Shader);

			Mesh* CreateMesh(const MeshInfo* Info, GPUBuffer::Usages Usage);
			void DestroyMesh(Mesh* Mesh);

			void Clear(IDevice::ClearFlags Flags, const ColorUI8& Color, RenderQueues Queue = RenderQueues::Default);

			void DrawMesh(Mesh* Mesh, const Matrix4F& Transform, Shader* Shader, RenderQueues Queue = RenderQueues::Default);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, Shader* Shader, RenderQueues Queue = RenderQueues::Default);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Shader* Shader, RenderQueues Queue = RenderQueues::Default);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Transform, Material* Material);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Material* Material);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material);

			void BeginRender(void);
			void EndRender(void);

			void SetDebugCallback(IDevice::DebugProcedureType Callback);

		private:
			void DestroyContextInternal(RenderContext* Context);

			Texture* CreateTextureInternal(const TextureInfo* Info);
			void DestroyTextureInternal(Texture* Texture);

			RenderTarget* CreateRenderTargetInternal(const RenderTargetInfo* Info);
			void DestroyRenderTargetInternal(RenderTarget* RenderTarget);

			Shader* CreateShaderInternal(const ShaderInfo* Info, String* Message = nullptr);
			void DestroyShaderInternal(Shader* Shader);

			Mesh* CreateMeshInternal(const MeshInfo* Info, GPUBuffer::Usages Usage);
			void DestroyMeshInternal(Mesh* Mesh);

			void RenderQueue(RenderQueues From, RenderQueues To);
			void EraseQueue(RenderQueues From, RenderQueues To);

			void OnPositionChanged(Window* Window) override
			{
			}
			void OnSizeChanged(Window* Window) override;
			void OnKeyDown(Window* Window, PlatformWindow::VirtualKeys Key) override
			{
			}
			void OnKeyUp(Window* Window, PlatformWindow::VirtualKeys Key) override
			{
			}
			void OnKeyPressed(Window* Window, PlatformWindow::VirtualKeys Key) override
			{
			}
			void OnMouseDown(Window* Window, PlatformWindow::VirtualKeys Key, const Vector2I& Position) override
			{
			}
			void OnMouseUp(Window* Window, PlatformWindow::VirtualKeys Key, const Vector2I& Position) override
			{
			}
			void OnMouseClick(Window* Window, PlatformWindow::VirtualKeys Key, const Vector2I& Position) override
			{
			}
			virtual void OnMouseWheel(Window* Window, const Vector2I& Position, uint16 Delta) override
			{
			}
			void OnMouseMove(Window* Window, const Vector2I& Position) override
			{
			}
			void OnMouseLeave(Window* Window) override
			{
			}
			void OnClosing(Window* Window) override
			{
			}

			INLINE void AddCommand(CommandList* Commands, RenderQueues Queue, CommandBase* Command)
			{
				Commands[(int8)Queue].Add(Command);
			}

		private:
			DeviceTypes m_DeviceType;
			IDevice* m_Device;
			ThreadedDevice* m_ThreadedDevice;
			ContextWindowMap m_ContextWindows;
			ContextWindowMap m_DummyContextWindows;
			RenderContext* m_CurentContext;
			Window* m_Window;
			CommandList m_CommandQueues[(int8)RenderQueues::COUNT];
		};
	}
}

#endif