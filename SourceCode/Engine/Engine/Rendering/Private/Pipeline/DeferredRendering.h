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
				class DeferredRendering
				{
					SINGLETON_DECLARATION(DeferredRendering)

				private:
					DeferredRendering(void);

				public:
					void Initialize(void);

					void BindRenderTarget(void);

					void Render(void);

				private:
					RenderTarget * m_RenderTarget;
					TextureHandle m_PositionTexture;
					TextureHandle m_NormalTexture;
					TextureHandle m_AlbedoSpecularTexture;
					Mesh *m_QuadMesh;
					ProgramHandle m_Program;
					Material m_Material;
				};
			}
		}
	}
}

#endif