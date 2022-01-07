// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DEFERRED_RENDERING_H
#define DEFERRED_RENDERING_H

#include <RenderSystem\IPipeline.h>
#include <RenderSystem\Mesh.h>
#include <RenderSystem\RenderContext.h>
#include <Containers\Map.h>
#include <Containers\Delegate.h>
#include <RenderSystem\CommandBuffer.h>

namespace Engine
{
	namespace RenderSystem
	{
		class DeviceInterface;
		class RenderTarget;
	}

	using namespace Containers;
	using namespace ResourceCommon;
	using namespace RenderSystem;

	namespace DeferredPipeline
	{
		namespace Private
		{
			class DEFERREDPIPELINE_API DeferredRendering : public IPipeline
			{
			private:
				struct ContextRenderTargetInfo
				{
				public:
					Vector2I Size;
					RenderTarget* RenderTarget;
					TextureResource PositionTexture;
					TextureResource NormalTexture;
					TextureResource AlbedoSpecularTexture;
				};

				typedef Map<RenderContext*, ContextRenderTargetInfo> RenderTargetContextMap;

			public:
				DeferredRendering(void);
				virtual ~DeferredRendering(void);

				void Initialize(DeviceInterface* DeviceInterface) override;
				void Uninitialize(void) override;

				void BeginRender(void) override;
				void EndRender(void) override;

				ProgramResource* GetAmbinetLightProgram(void) override
				{
					return &m_AmbientLightProgram;
				}

				ProgramResource* GetDirectionalLightProgram(void) override
				{
					return &m_DirectionalLightProgram;
				}

				ProgramResource* GetPointLightProgram(void) override
				{
					return &m_PointLightProgram;
				}

				ProgramResource* GetSpotLightProgram(void) override
				{
					return &m_SpotLightProgram;
				}

				void OnContextChanged(RenderContext* Context);
				DECLARE_MEMBER_EVENT_LISTENER(DeferredRendering, OnContextChanged);

				void OnContextResized(RenderContext* Context);
				DECLARE_MEMBER_EVENT_LISTENER(DeferredRendering, OnContextResized);

				void SetPassConstants(Material* Material) override;

				void RefreshRenderTarget(RenderContext* Context);

			private:
				DeviceInterface* m_DeviceInterface;

				ProgramResource m_AmbientLightProgram;
				ProgramResource m_DirectionalLightProgram;
				ProgramResource m_PointLightProgram;
				ProgramResource m_SpotLightProgram;

				RenderTargetContextMap m_RenderTargets;

				ContextRenderTargetInfo* m_ActiveInfo;

				CommandBuffer* m_CommandBufferGBuffer;
				CommandBuffer* m_CommandBufferLighting;
				CommandBuffer* m_CommandBufferFinal;
			};
		}
	}
}

#endif