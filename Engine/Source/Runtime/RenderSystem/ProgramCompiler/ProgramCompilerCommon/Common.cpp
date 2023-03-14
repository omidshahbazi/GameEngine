// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ProgramCompilerCommon\Common.h>

namespace Engine
{
	using namespace Common;

	namespace ProgramCompilerCommon
	{
		cstr Constants::ENTRY_POINT_NAME = "main";

		cstr Constants::VERTEX_ENTRY_POINT_NAME = "vertexmain";
		cstr Constants::HULL_ENTRY_POINT_NAME = "hullmain";
		cstr Constants::DOMAIN_ENTRY_POINT_NAME = "domainmain";
		cstr Constants::GEOMETRY_ENTRY_POINT_NAME = "geometrymain";
		cstr Constants::FRAGMENT_ENTRY_POINT_NAME = "fragmentmain";
		cstr Constants::COMPUTE_ENTRY_POINT_NAME = "computemain";

		const ProgramDataTypes Constants::UNSIGNED_DATA_TYPES[] = { ProgramDataTypes::UnsignedInteger, ProgramDataTypes::UnsignedInteger2, ProgramDataTypes::UnsignedInteger3 };
		const ProgramDataTypes Constants::FLOATING_POINT_TYPES[] = { ProgramDataTypes::Float, ProgramDataTypes::Float2, ProgramDataTypes::Float3, ProgramDataTypes::Float4, ProgramDataTypes::Double, ProgramDataTypes::Double2, ProgramDataTypes::Double3, ProgramDataTypes::Double4 };
		const ProgramDataTypes Constants::FLOATING_POINT_VECTOR_TYPES[] = { ProgramDataTypes::Float2, ProgramDataTypes::Float3, ProgramDataTypes::Float4, ProgramDataTypes::Double2, ProgramDataTypes::Double3, ProgramDataTypes::Double4 };
		const ProgramDataTypes Constants::NUMERIC_DATA_TYPES[] = { ProgramDataTypes::Integer, ProgramDataTypes::UnsignedInteger, ProgramDataTypes::Float, ProgramDataTypes::Double };
		const ProgramDataTypes Constants::VECTOR_DATA_TYPES[] = { ProgramDataTypes::Integer2, ProgramDataTypes::UnsignedInteger2, ProgramDataTypes::Float2, ProgramDataTypes::Double2, ProgramDataTypes::Integer3, ProgramDataTypes::UnsignedInteger3, ProgramDataTypes::Float3, ProgramDataTypes::Double3, ProgramDataTypes::Integer4, ProgramDataTypes::UnsignedInteger4, ProgramDataTypes::Float4, ProgramDataTypes::Double4 };
		const ProgramDataTypes Constants::NUMERIC_AND_VECTOR_DATA_TYPES[] = { ProgramDataTypes::Integer, ProgramDataTypes::UnsignedInteger, ProgramDataTypes::Float, ProgramDataTypes::Double, ProgramDataTypes::Integer2, ProgramDataTypes::UnsignedInteger2, ProgramDataTypes::Float2, ProgramDataTypes::Double2, ProgramDataTypes::Integer3, ProgramDataTypes::UnsignedInteger3, ProgramDataTypes::Float3, ProgramDataTypes::Double3, ProgramDataTypes::Integer4, ProgramDataTypes::UnsignedInteger4, ProgramDataTypes::Float4, ProgramDataTypes::Double4 };
		const ProgramDataTypes Constants::MATRICES_DATA_TYPES[] = { ProgramDataTypes::Matrix4F, ProgramDataTypes::Matrix4D };
		const ProgramDataTypes Constants::NUMERIC_VECTOR_AND_MATRICES_DATA_TYPES[] = { ProgramDataTypes::Integer, ProgramDataTypes::UnsignedInteger, ProgramDataTypes::Float, ProgramDataTypes::Double, ProgramDataTypes::Integer2, ProgramDataTypes::UnsignedInteger2, ProgramDataTypes::Float2, ProgramDataTypes::Double2, ProgramDataTypes::Integer3, ProgramDataTypes::UnsignedInteger3, ProgramDataTypes::Float3, ProgramDataTypes::Double3, ProgramDataTypes::Integer4, ProgramDataTypes::UnsignedInteger4, ProgramDataTypes::Float4, ProgramDataTypes::Double4, ProgramDataTypes::Matrix4F, ProgramDataTypes::Matrix4D };
		const ProgramDataTypes Constants::TEXTURE_DATA_TYPES[] = { ProgramDataTypes::Integer, ProgramDataTypes::Integer2, ProgramDataTypes::Integer4, ProgramDataTypes::UnsignedInteger, ProgramDataTypes::UnsignedInteger2, ProgramDataTypes::UnsignedInteger4, ProgramDataTypes::Float, ProgramDataTypes::Float2, ProgramDataTypes::Float4, ProgramDataTypes::Double, ProgramDataTypes::Double2 };

		const ProgramDataTypes Constants::SAMPLABLE_TEXTURE_TYPES[] = { ProgramDataTypes::Texture1D, ProgramDataTypes::Texture2D, ProgramDataTypes::Texture3D, ProgramDataTypes::TextureCube };
		const ProgramDataTypes Constants::SAMPLABLE_TEXTURE_UV_TYPES[] = { ProgramDataTypes::Float, ProgramDataTypes::Float2, ProgramDataTypes::Float3, ProgramDataTypes::Float4 };

		const ProgramDataTypes Constants::READABLE_TEXTURE_TYPES[] = { ProgramDataTypes::Texture1D, ProgramDataTypes::Texture2D, ProgramDataTypes::Texture3D, ProgramDataTypes::TextureCube, ProgramDataTypes::Texture1DRW, ProgramDataTypes::Texture2DRW, ProgramDataTypes::Texture3DRW };
		const ProgramDataTypes Constants::READABLE_TEXTURE_UV_TYPES[] = { ProgramDataTypes::Integer, ProgramDataTypes::Integer2, ProgramDataTypes::Integer3, ProgramDataTypes::Integer4, ProgramDataTypes::Integer, ProgramDataTypes::Integer2, ProgramDataTypes::Integer3 };

		const ProgramDataTypes Constants::WRITEABLE_TEXTURE_TYPES[] = { ProgramDataTypes::Texture1DRW, ProgramDataTypes::Texture2DRW, ProgramDataTypes::Texture3DRW };
		const ProgramDataTypes Constants::WRITEABLE_TEXTURE_UV_TYPES[] = { ProgramDataTypes::Integer, ProgramDataTypes::Integer2, ProgramDataTypes::Integer3 };
	}
}