// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OUTPUT_INFO_H
#define OUTPUT_INFO_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>
#include <Containers\Vector.h>
#include <ProgramCompilerCommon\Common.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace ProgramCompilerCommon
	{
		struct VariableMetaInfo
		{
		public:
			uint8 Handle;
			ProgramDataTypes DataType;
			String UserDefinedType;
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
			uint16 Size;
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
			String HullShader;
			String DomainShader;
			String GeometryShader;
			String FragmentShader;
			String ComputeShader;

			MetaInfo MetaInfo;
		};
	}
}

#endif