// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef DEVICE_INTERFACE_H
#define DEVICE_INTERFACE_H

#include <Rendering\RenderingCommon.h>
#include <Rendering\IDevice.h>
#include <Rendering\ProgramInfo.h>
#include <Rendering\CompiledProgramInfo.h>
#include <Containers\ListenerContainer.h>
#include <Containers\Map.h>
#include <Utility\Window.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Utility;

	namespace Rendering
	{
		class RenderContext;
		class Sprite;
		class Program;
		class Material;
		class Mesh;
		struct Color;

		namespace Private
		{
			class ThreadedDevice;
			class CommandsHolder;

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

				virtual void OnError(const String& Message)
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

			bool CompileProgram(const ProgramInfo* Info, CompiledProgramInfo* CompiledInfo);
			Program* CreateProgram(const CompiledProgramInfo* Info);
			Program* CreateProgram(const ProgramInfo* Info);
			void DestroyProgram(Program* Program);

			Mesh* CreateMesh(const MeshInfo* Info);
			void DestroyMesh(Mesh* Mesh);

			void Clear(IDevice::ClearFlags Flags, const ColorUI8& Color, RenderQueues Queue = RenderQueues::Default);

			void DrawMesh(Mesh* Mesh, const Matrix4F& Transform, Program* Program, RenderQueues Queue = RenderQueues::Default);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, Program* Program, RenderQueues Queue = RenderQueues::Default);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, Program* Program, RenderQueues Queue = RenderQueues::Default);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Transform, Material* Material);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Material* Material);
			void DrawMesh(Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material);

			void BeginRender(void);
			void EndRender(void);

		private:
			RenderContext* CreateDummyContext(void);

			void DestroyContextInternal(RenderContext* Context);

			void AddCommandToQueue(RenderQueues Queue, CommandBase* Command);

			void OnSizeChanged(Window* Window) override;

		private:
			bool m_Initialized;

			DeviceTypes m_DeviceType;
			IDevice* m_Device;
			ThreadedDevice* m_ThreadedDevice;
			CommandsHolder* m_CommandsHolder;
			ContextWindowMap m_ContextWindows;
			ContextWindowMap m_DummyContextWindows;
			RenderContext* m_CurentContext;
			Window* m_Window;
		};
	}
}

#endif