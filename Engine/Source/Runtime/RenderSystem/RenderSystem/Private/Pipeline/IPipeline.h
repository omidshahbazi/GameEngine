// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef IPIPELINE_H
#define IPIPELINE_H

#include <ResourceCommon\Resource.h>

namespace Engine
{
	using namespace ResourceCommon;

	namespace RenderSystem
	{
		class Material;
		class RenderContext;

		namespace Private
		{
			namespace Pipeline
			{
				class IPipeline
				{
				public:
					virtual ~IPipeline(void)
					{
					}

					virtual void BeginRender(void) = 0;

					virtual void EndRender(void) = 0;

					virtual ProgramResource* GetAmbinetLightProgram(void) = 0;
					virtual ProgramResource* GetDirectionalLightProgram(void) = 0;
					virtual ProgramResource* GetPointLightProgram(void) = 0;
					virtual ProgramResource* GetSpotLightProgram(void) = 0;

					virtual void SetPassConstants(Material* Material) = 0;

					virtual void OnContextChanged(RenderContext* Context) = 0;
					virtual void OnContextResized(RenderContext* Context) = 0;
				};
			}
		}
	}
}

#endif