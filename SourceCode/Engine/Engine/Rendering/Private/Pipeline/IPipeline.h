// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef IPIPELINE_H
#define IPIPELINE_H

#include <Rendering\Pass.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Pipeline
			{
				class IPipeline
				{
				public:
					virtual void BeginRender(void) = 0;

					virtual void EndRender(void) = 0;

					virtual ProgramHandle* GetAmbinetLightProgram(void) = 0;
					virtual ProgramHandle* GetDirectionalLightProgram(void) = 0;
					virtual ProgramHandle* GetPointLightProgram(void) = 0;
					virtual ProgramHandle* GetSpotLightProgram(void) = 0;

					virtual void SetPassConstants(Pass* Pass) = 0;

					virtual void OnDeviceInterfaceResized(void) = 0;
				};
			}
		}
	}
}

#endif