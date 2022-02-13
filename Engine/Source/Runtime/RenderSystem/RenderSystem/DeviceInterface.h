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
		class CommandBuffer;
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
			class FrameDataChain;
			class FrameConstantBuffers;
		}

		using namespace Private;

		class RENDERSYSTEM_API DeviceInterface
		{
			friend class RenderManager;
			friend class ConstantBuffer;
			friend class BuiltiInProgramConstants;
			friend class ProgramConstantHolder;
			friend class FrameConstantBuffers;
			friend class CommandBuffer;

		public:
			typedef Delegate<const RenderContext*> ContextChangedEventHandler;
			typedef Delegate<const Vector2I&> ContextResizedEventHandler;

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

			Texture* CreateTexture(const TextureInfo* Info);
			Sprite* CreateSprite(const TextureInfo* Info);
			void DestroyTexture(Texture* Texture);

			RenderTarget* CreateRenderTarget(const RenderTargetInfo* Info);
			void DestroyRenderTarget(RenderTarget* RenderTarget);

			bool CompileProgram(const ProgramInfo* Info, CompiledProgramInfo* CompiledInfo);
			Program* CreateProgram(const ProgramInfo* Info);
			Program* CreateProgram(const CompiledProgramInfo* Info);
			void DestroyProgram(Program* Program);

			Mesh* CreateMesh(const MeshInfo* Info);
			void DestroyMesh(Mesh* Mesh);

			void SubmitCommandBuffer(const CommandBuffer* Buffer);
			void SubmitCommandBufferAsync(const CommandBuffer* Buffer);

			void BeginFrame(const RenderContext* Context);
			void EndFrame(void);

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
			FrameDataChain* GetFrameDataChain(void)
			{
				return m_FrameDataChain;
			}

			void DestroyContextInternal(RenderContext* Context);

		public:
			ContextChangedEventHandler OnContextChangedEvent;
			ContextResizedEventHandler OnContextResizedEvent;

		private:
			bool m_Initialized;

			DeviceTypes m_DeviceType;
			IDevice* m_Device;
			ThreadedDevice* m_ThreadedDevice;
			const RenderContext* m_LastContext;
			FrameDataChain* m_FrameDataChain;

			//UNDONE:RENDERING -> IPipeline
			//Should be per camera
			IPipeline* m_Pipeline;
		};
	}
}

#endif