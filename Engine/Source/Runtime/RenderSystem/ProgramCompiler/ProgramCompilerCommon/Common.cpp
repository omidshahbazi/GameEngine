// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ProgramCompilerCommon\Common.h>

namespace Engine
{
	using namespace Common;

	namespace ProgramCompilerCommon
	{
		cstr Constants::ENTRY_POINT_NAME = "main";

		cstr Constants::VERTEX_ENTRY_POINT_NAME = "vertexmain";
		cstr Constants::TESSELLATION_ENTRY_POINT_NAME = "tessellationmain";
		cstr Constants::GEOMETRY_ENTRY_POINT_NAME = "geometrymain";
		cstr Constants::FRAGMENT_ENTRY_POINT_NAME = "fragmentmain";
		cstr Constants::COMPUTE_ENTRY_POINT_NAME = "computemain";
	}
}