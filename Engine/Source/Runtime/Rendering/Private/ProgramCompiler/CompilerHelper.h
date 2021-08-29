// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMPILER_HELPER_H
#define COMPILER_HELPER_H

#include <Common\PrimitiveTypes.h>
#include <Rendering\ProgramInfo.h>
#include <Rendering\CompiledProgramInfo.h>
#include <Rendering\RenderingCommon.h>

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				class RENDERING_API CompilerHelper
				{
				public:
					static bool Compile(const ProgramInfo& Info, const DeviceTypes* DeviceTypes, uint8 DeviceTypeCount, CompiledProgramInfo* CompiledInfos);
				};
			}
		}
	}
}

#endif