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
		class DeviceInterface;

		class IPipeline
		{
		public:
			virtual ~IPipeline(void)
			{
			}

			virtual void Initialize(DeviceInterface* DeviceInterface) = 0;
			virtual void Uninitialize(void) = 0;

			virtual void BeginRender(void) = 0;
			virtual void EndRender(void) = 0;

			virtual ProgramResource* GetAmbinetLightProgram(void) = 0;
			virtual ProgramResource* GetDirectionalLightProgram(void) = 0;
			virtual ProgramResource* GetPointLightProgram(void) = 0;
			virtual ProgramResource* GetSpotLightProgram(void) = 0;

			virtual void SetPassConstants(Material* Material) = 0;
		};
	}
}

#endif