// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMPILED_PROGRAM_INFO_H
#define COMPILED_PROGRAM_INFO_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Rendering
	{
		struct CompiledProgramInfo
		{
		public:
			struct CompiledShader
			{
				byte* Buffer;
				uint16 Size;
			};

		public:
			CompiledProgramInfo(void) :
				VertexShader({}),
				GeometryShader({}),
				DomainShader({}),
				FragmentShader({}),
				ComputeShader({})
			{
			}

		public:
			CompiledShader VertexShader;
			CompiledShader GeometryShader;
			CompiledShader DomainShader;
			CompiledShader FragmentShader;
			CompiledShader ComputeShader;
		};
	}
}

#endif