// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMMON_H
#define COMMON_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace ProgramCompilerCommon
	{
		enum class Stages
		{
			Vertex = 0,
			Hull,
			Domain,
			Geometry,
			Fragment,
			Compute
		};

		enum class ProgramDataTypes
		{
			Void = 0,
			Bool,
			Integer,
			UnsignedInteger,
			Float,
			Double,
			Float2,
			Double2,
			Float3,
			Double3,
			Float4,
			Double4,
			Matrix4,
			Texture2D,
			Unknown
		};

		const ProgramDataTypes ALLOWED_CONTEXT_FREE_DATA_TYPES[]{ ProgramDataTypes::Texture2D };

		class PROGRAMCOMPILERCOMMON_API Constants
		{
		public:
			static cstr ENTRY_POINT_NAME;

			static cstr VERTEX_ENTRY_POINT_NAME;
			static cstr HULL_ENTRY_POINT_NAME;
			static cstr DOMAIN_ENTRY_POINT_NAME;
			static cstr GEOMETRY_ENTRY_POINT_NAME;
			static cstr FRAGMENT_ENTRY_POINT_NAME;
			static cstr COMPUTE_ENTRY_POINT_NAME;
		};
	}
}

#endif