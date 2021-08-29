// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMPILE_OUTPUT_INFO_H
#define COMPILE_OUTPUT_INFO_H


#include <Rendering\CompiledProgramInfo.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				struct CompileOutputInfo
				{
				public:
					bool Result;

					String VertexShader;
					String TessellationShader;
					String GeometryShader;
					String FragmentShader;
					String ComputeShader;

					MetaInfo MetaInfo;
				};
			}
		}
	}
}

#endif