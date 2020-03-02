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
		class Program;
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
			typedef Vector<Program*> ProgramVector;
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

			Texture* CreateTexture2D(const byte* Data, uint32 Width, uint32 Height, Texture::Formats Format);
			void DestroyTexture(Texture* Texture);

			RenderTarget* CreateRenderTarget(const RenderTargetInfo* Info);
			void DestroyRenderTarget(RenderTarget* RenderTarget);
			void SetRenderTarget(RenderTarget* RenderTarget, RenderQueues Queue = RenderQueues::Default);

			Program* CreateProgram(const String& Shader);
			void DestroyProgram(Program* Program);

			Mesh* CreateMesh(const MeshInfo* Info, IDevice::BufferUsages Usage);
			void DestroyMesh(Mesh* Mesh);

			void Clear(IDevice::ClearFlags Flags, Color Color, RenderQueues Queue = RenderQueues::Default);

			void DrawMesh(Mesh* Mesh, const Matrix4F& Transform, Program* Program, RenderQueues Queue = RenderQueues::Default);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, Program* Program, RenderQueues Queue = RenderQueues::Default);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Program* Program, RenderQueues Queue = RenderQueues::Default);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Transform, Material* Material);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, Material* Material);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Material* Material);

			void BeginRender(void);
			void EndRender(void);

			IDevice* GetDevice(void) const
			{
				return m_Device;
			}

		private:
			Texture* CreateTexture2DInternal(const byte* Data, uint32 Width, uint32 Height, Texture::Formats Format);
			void DestroyTextureInternal(Texture* Texture);

			RenderTarget* CreateRenderTargetInternal(const RenderTargetInfo* Info);
			void DestroyRenderTargetInternal(RenderTarget* RenderTarget);

			Program* CreateProgramInternal(const String& Shader);
			void DestroyProgramInternal(Program* Program);

			Mesh* CreateMeshInternal(const MeshInfo* Info, IDevice::BufferUsages Usage);
			void DestroyMeshInternal(Mesh* Mesh);

			void RenderQueue(RenderQueues From, RenderQueues To);
			void EraseQueue(RenderQueues From, RenderQueues To);

			void OnWindowMoved(Window* Window) override
			{
			}
			void OnWindowResized(Window* Window) override;
			void OnKeyDown(PlatformWindow::VirtualKeys Key) override
			{
			}
			void OnKeyUp(PlatformWindow::VirtualKeys Key) override
			{
			}
			void OnKeyPressed(PlatformWindow::VirtualKeys Key) override
			{
			}
			void OnMouseDown(PlatformWindow::VirtualKeys Key, Vector2I Position) override
			{
			}
			void OnMouseUp(PlatformWindow::VirtualKeys Key, Vector2I Position) override
			{
			}
			void OnMouseClick(PlatformWindow::VirtualKeys Key, Vector2I Position) override
			{
			}
			virtual void OnMouseWheel(Vector2I Position, uint16 Delta) override
			{
			}
			void OnMouseMove(Vector2I Position) override
			{
			}
			void OnMouseLeave(void) override
			{
			}
			void OnClosing(void) override
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
			ProgramVector m_Programs;
			CommandList m_CommandQueues[(int8)RenderQueues::COUNT];

			// Inherited via IListener
};
	}
}

#endif