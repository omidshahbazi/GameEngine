// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef DEVICE_INTERFACE_H
#define DEVICE_INTERFACE_H

#include <Containers\Strings.h>
#include <Containers\Map.h>
#include <Rendering\IDevice.h>
#include <Containers\ListenerContainer.h>
#include <Rendering\RenderingCommon.h>
#include <Utility\Window.h>

namespace Engine
{
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
			typedef Vector<Texture*> TextureList;
			typedef Vector<RenderTarget*> RenderTargetList;
			typedef Vector<Shader*> ShaderVector;
			typedef Vector<CommandBase*> CommandList;

		public:
			class RENDERING_API IListener
			{
			public:
				virtual void OnWindowChanged(Window* Window) = 0;
				virtual void OnWindowResized(Window* Window) = 0;
			};

			enum class Type
			{
				OpenGL
			};

			LISTENER_DECLARATION(IListener)

		public:
			DeviceInterface(Type Type);
			~DeviceInterface(void);

			void Initialize(void);

			RenderContext* CreateContext(Window* Window);
			void SetContext(RenderContext* Context);
			Window* GetWindow(void)
			{
				return m_Window;
			}

			Texture* CreateTexture2D(const Vector2I& Dimension, Texture::Formats Format, const byte* Data = nullptr);
			Sprite* CreateSprite(const Vector2I& Dimension, const Vector4I& Borders, Texture::Formats Format, const byte* Data = nullptr);
			void DestroyTexture(Texture* Texture);

			RenderTarget* CreateRenderTarget(const IDevice::RenderTargetInfo* Info);
			void DestroyRenderTarget(RenderTarget* RenderTarget);
			void SetRenderTarget(RenderTarget* RenderTarget, RenderQueues Queue = RenderQueues::Default);

			Shader* CreateShader(const String& Source, String* Message = nullptr);
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

			IDevice* GetDevice(void) const
			{
				return m_Device;
			}

		private:
			Texture* CreateTexture2DInternal(const byte* Data, const Vector2I& Dimension, Texture::Formats Format);
			void DestroyTextureInternal(Texture* Texture);

			RenderTarget* CreateRenderTargetInternal(const IDevice::RenderTargetInfo* Info);
			void DestroyRenderTargetInternal(RenderTarget* RenderTarget);

			Shader* CreateShaderInternal(const String& Source, String* Message = nullptr);
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
			Type m_Type;
			IDevice* m_Device;
			ContextWindowMap m_ContextWindows;
			RenderContext* m_Context;
			Window* m_Window;
			TextureList m_Textures;
			RenderTargetList m_RenderTargets;
			ShaderVector m_Shaders;
			CommandList m_CommandQueues[(int8)RenderQueues::COUNT];

			// Inherited via IListener
		};
	}
}

#endif