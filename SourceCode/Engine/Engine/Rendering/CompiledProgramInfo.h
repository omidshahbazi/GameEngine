// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMPILED_PROGRAM_INFO_H
#define COMPILED_PROGRAM_INFO_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>
#include <Containers\Vector.h>
#include <Rendering\ProgramDataTypes.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Rendering
	{
		struct VariableMetaInfo
		{
		public:
			String DataType;
			String Name;
		};

		struct StructMetaInfo
		{
			struct VariableMetaInfo
			{
			public:
				ProgramDataTypes DataType;
				String Name;
			};

		public:
			String Name;
			Vector<VariableMetaInfo> Variables;
		};

		struct MetaInfo
		{
		public:
			Vector<StructMetaInfo> Structs;

			Vector<VariableMetaInfo> Variables;
		};

		struct OutputInfo
		{
		public:
			String VertexShader;
			String TessellationShader;
			String GeometryShader;
			String FragmentShader;
			String ComputeShader;

			MetaInfo MetaInfo;
		};

		struct CompiledProgramInfo
		{
		public:
			struct CompiledProgram
			{
				byte* Buffer;
				uint16 Size;
			};

		public:
			CompiledProgramInfo(void) :
				VertexShader({}),
				TessellationShader({}),
				GeometryShader({}),
				FragmentShader({}),
				ComputeShader({})
			{
			}

		public:
			CompiledProgram VertexShader;
			CompiledProgram TessellationShader;
			CompiledProgram GeometryShader;
			CompiledProgram FragmentShader;
			CompiledProgram ComputeShader;

			MetaInfo MetaInfo;
		};
	}
}

#endif