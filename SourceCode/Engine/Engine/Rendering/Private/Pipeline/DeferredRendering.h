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

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Pipeline
			{
				class RENDERING_API DeferredRendering : public IPipeline
				{
				public:
					DeferredRendering(DeviceInterface* DeviceInterface);

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

					void OnDeviceInterfaceResized(void) override;

					void SetPassConstants(Pass* Pass) override;

				private:
					DeviceInterface* m_DeviceInterface;
					RenderTarget* m_RenderTarget;
					TextureHandle m_PositionTexture;
					TextureHandle m_NormalTexture;
					TextureHandle m_AlbedoSpecularTexture;
					ProgramHandle m_AmbientLightProgram;
					ProgramHandle m_DirectionalLightProgram;
					ProgramHandle m_PointLightProgram;
					ProgramHandle m_SpotLightProgram;
				};
			}
		}
	}
}

#endif