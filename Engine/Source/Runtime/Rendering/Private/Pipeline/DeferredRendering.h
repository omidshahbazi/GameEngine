// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DEFERRED_RENDERING_H
#define DEFERRED_RENDERING_H

#include <Rendering\DeviceInterface.h>
#include <Rendering\Private\Pipeline\IPipeline.h>
#include <Rendering\RenderTarget.h>
#include <Rendering\Material.h>
#include <Rendering\Program.h>
#include <Rendering\Mesh.h>
#include <Containers\Map.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace Pipeline
			{
				class RENDERING_API DeferredRendering : public IPipeline
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
					DeferredRendering(DeviceInterface* DeviceInterface);
					virtual ~DeferredRendering(void);

				private:
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

					void OnContextChanged(RenderContext* Context) override;
					void OnContextResized(RenderContext* Context) override;

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
				};
			}
		}
	}
}

#endif