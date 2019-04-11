// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef DEFERRED_RENDERING_H
#define DEFERRED_RENDERING_H

#include <MemoryManagement\Singleton.h>
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
				class RENDERING_API DeferredRendering
				{
					SINGLETON_DECLARATION(DeferredRendering)

				private:
					DeferredRendering(void);

				public:
					void Initialize(void);

					RenderTarget *GetGBufferMRT(void)
					{
						return m_RenderTarget;
					}

					TextureHandle *GetPositionTexture(void)
					{
						return &m_PositionTexture;
					}

					TextureHandle *GetNormalTexture(void)
					{
						return &m_NormalTexture;
					}

					TextureHandle *GetAlbedoSpecularTexture(void)
					{
						return &m_AlbedoSpecularTexture;
					}

					ProgramHandle *GetAmbinetLightProgram(void)
					{
						return &m_AmbientLightProgram;
					}

					ProgramHandle *GetDirectionalLightProgram(void)
					{
						return &m_DirectionalLightProgram;
					}

					ProgramHandle *GetPointLightProgram(void)
					{
						return &m_PointLightProgram;
					}

					ProgramHandle *GetSpotLightProgram(void)
					{
						return &m_SpotLightProgram;
					}

				private:
					RenderTarget *m_RenderTarget;
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