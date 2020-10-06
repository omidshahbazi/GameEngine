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
						TextureResource PositionTexture;
						TextureResource NormalTexture;
						TextureResource AlbedoSpecularTexture;
					};

					typedef Map<Window*, WindowRenderTargetInfo> RenderTargetWindowMap;

				public:
					DeferredRendering(DeviceInterface* DeviceInterface);
					virtual ~DeferredRendering(void);

				private:
					void BeginRender(void) override;
					void EndRender(void) override;

					ShaderResource* GetAmbinetLightShader(void) override
					{
						return &m_AmbientLightShader;
					}

					ShaderResource* GetDirectionalLightShader(void) override
					{
						return &m_DirectionalLightShader;
					}

					ShaderResource* GetPointLightShader(void) override
					{
						return &m_PointLightShader;
					}

					ShaderResource* GetSpotLightShader(void) override
					{
						return &m_SpotLightShader;
					}

					void OnWindowChanged(Window* Window) override;
					void OnWindowResized(Window* Window) override;

					void SetPassConstants(Pass* Pass) override;

					void RefreshRenderTarget(Window* Window);

				private:
					DeviceInterface* m_DeviceInterface;

					ShaderResource m_AmbientLightShader;
					ShaderResource m_DirectionalLightShader;
					ShaderResource m_PointLightShader;
					ShaderResource m_SpotLightShader;

					RenderTargetWindowMap m_RenderTargets;

					WindowRenderTargetInfo* m_ActiveInfo;
				};
			}
		}
	}
}

#endif