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

				private:
					void BeginRender(void) override;
					void EndRender(void) override;

					ProgramHandle* GetAmbinetLightProgram(void) override
					{
						return &m_AmbientLightProgram;
					}

					ProgramHandle* GetDirectionalLightProgram(void) override
					{
						return &m_DirectionalLightProgram;
					}

					ProgramHandle* GetPointLightProgram(void) override
					{
						return &m_PointLightProgram;
					}

					ProgramHandle* GetSpotLightProgram(void) override
					{
						return &m_SpotLightProgram;
					}

					void OnWindowChanged(Window* Window) override;
					void OnWindowResized(Window* Window) override;

					void SetPassConstants(Pass* Pass) override;

					void RefreshRenderTarget(Window* Window);

				private:
					DeviceInterface* m_DeviceInterface;

					ProgramHandle m_AmbientLightProgram;
					ProgramHandle m_DirectionalLightProgram;
					ProgramHandle m_PointLightProgram;
					ProgramHandle m_SpotLightProgram;

					RenderTargetWindowMap m_RenderTargets;

					WindowRenderTargetInfo* m_ActiveInfo;
				};
			}
		}
	}
}

#endif