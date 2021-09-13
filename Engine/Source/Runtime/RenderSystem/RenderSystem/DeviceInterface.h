// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef DEVICE_INTERFACE_H
#define DEVICE_INTERFACE_H

#include <RenderCommon\RenderCommon.h>
#include <RenderDevice\IDevice.h>
#include <RenderDevice\ProgramInfo.h>
#include <RenderDevice\CompiledProgramInfo.h>
#include <Containers\Delegate.h>
#include <Containers\Map.h>
#include <WindowUtility\Window.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace RenderCommon;
	using namespace RenderDevice;
	using namespace WindowUtility;

	namespace RenderSystem
	{
		class RenderManager;
		class IPipeline;
		class RenderContext;
		class RenderTarget;
		class Texture;
		class Sprite;
		class Program;
		class Material;
		class Mesh;
		struct Color;

		namespace Private
		{
			class ThreadedDevice;
			class BuiltiInProgramConstants;
			class ProgramConstantHolder;

			namespace Commands
			{
				class CommandBase;
				class CommandsHolder;
				class IntermediateConstantBuffers;
			}
		}

		using namespace Private;
		using namespace Private::Commands;

		class RENDERSYSTEM_API DeviceInterface
		{
			friend class RenderManager;
			friend class ConstantBuffer;
			friend class BuiltiInProgramConstants;
			friend class ProgramConstantHolder;
			friend class IntermediateConstantBuffers;

		public:
			typedef Delegate<RenderContext*> ContextChangedEventHandler;
			typedef Delegate<RenderContext*> ContextResizedEventHandler;

			static const uint16 DEFAULT_COMPILED_SHADER_BUFFER_SIZE = 32768;

		private:
			DeviceInterface(DeviceTypes DeviceType);

		public:
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

			void SetViewport(const Vector2I& Position, const Vector2I& Size, RenderQueues Queue = RenderQueues::Default);

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

			void BeginEvent(const String& Label, RenderQueues Queue = RenderQueues::Default);
			void BeginEvent(const WString& Label, RenderQueues Queue = RenderQueues::Default);
			void EndEvent(RenderQueues Queue = RenderQueues::Default);

			void SetMarker(const String& Label, RenderQueues Queue = RenderQueues::Default);
			void SetMarker(const WString& Label, RenderQueues Queue = RenderQueues::Default);

			DeviceTypes GetType(void) const
			{
				return m_DeviceType;
			}

			void SetPipeline(IPipeline* Pipeline);
			IPipeline* GetPipeline(void) const
			{
				return m_Pipeline;
			}

		private:
			void DestroyContextInternal(RenderContext* Context);

			void AddCommandToQueue(RenderQueues Queue, CommandBase* Command);

			void OnWindowSizeChanged(Window* Window);
			DECLARE_MEMBER_EVENT_LISTENER(DeviceInterface, OnWindowSizeChanged);

			ThreadedDevice* GetThreadedDevice(void) const
			{
				return m_ThreadedDevice;
			}

		public:
			ContextChangedEventHandler OnContextChangedEvent;
			ContextResizedEventHandler OnContextResizedEvent;

		private:
			bool m_Initialized;

			DeviceTypes m_DeviceType;
			IDevice* m_Device;
			IPipeline* m_Pipeline;
			ThreadedDevice* m_ThreadedDevice;
			CommandsHolder* m_CommandsHolder;
			RenderContext* m_CurentContext;
		};
	}
}

#endif