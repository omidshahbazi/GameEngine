// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DEFERRED_RENDERING_H
#define DEFERRED_RENDERING_H

#include <Rendering\DeviceInterface.h>
#include <Rendering\Private\Pipeline\IPipeline.h>
#include <Rendering\RenderTarget.h>
#include <Rendering\Material.h>
#include <Rendering\Shader.h>
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
					struct WindowRenderTargetInfo
					{
					public:
						Vector2I Size;
						RenderTarget* RenderTarget;
						TextureHandle PositionTexture;
						TextureHandle NormalTexture;
						TextureHandle AlbedoSpecularTexture;
					};

					typedef Map<Window*, WindowRenderTargetInfo> RenderTargetWindowMap;

				public:
					DeferredRendering(DeviceInterface* DeviceInterface);
					virtual ~DeferredRendering(void);

				private:
					void BeginRender(void) override;
					void EndRender(void) override;

					ShaderHandle* GetAmbinetLightShader(void) override
					{
						return &m_AmbientLightShader;
					}

					ShaderHandle* GetDirectionalLightShader(void) override
					{
						return &m_DirectionalLightShader;
					}

					ShaderHandle* GetPointLightShader(void) override
					{
						return &m_PointLightShader;
					}

					ShaderHandle* GetSpotLightShader(void) override
					{
						return &m_SpotLightShader;
					}

					void OnWindowChanged(Window* Window) override;
					void OnWindowResized(Window* Window) override;

					void SetPassConstants(Pass* Pass) override;

					void RefreshRenderTarget(Window* Window);

				private:
					DeviceInterface* m_DeviceInterface;

					ShaderHandle m_AmbientLightShader;
					ShaderHandle m_DirectionalLightShader;
					ShaderHandle m_PointLightShader;
					ShaderHandle m_SpotLightShader;

					RenderTargetWindowMap m_RenderTargets;

					WindowRenderTargetInfo* m_ActiveInfo;
				};
			}
		}
	}
}

#endif