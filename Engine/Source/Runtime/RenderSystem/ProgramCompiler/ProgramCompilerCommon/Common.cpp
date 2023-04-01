// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ProgramCompilerCommon\Common.h>
#include <Containers\Map.h>

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

		ProgramDataTypes Constants::GetPrimitiveDataType(const String& Name)
		{
			static bool initialized = false;
			static Map<String, ProgramDataTypes> dataTypesName;

			if (!initialized)
			{
				initialized = true;

				dataTypesName["void"] = ProgramDataTypes::Void;
				dataTypesName["bool"] = ProgramDataTypes::Bool;
				dataTypesName["int"] = ProgramDataTypes::Integer;
				dataTypesName["uint"] = ProgramDataTypes::UnsignedInteger;
				dataTypesName["float"] = ProgramDataTypes::Float;
				dataTypesName["double"] = ProgramDataTypes::Double;
				dataTypesName["int2"] = ProgramDataTypes::Integer2;
				dataTypesName["uint2"] = ProgramDataTypes::UnsignedInteger2;
				dataTypesName["float2"] = ProgramDataTypes::Float2;
				dataTypesName["double2"] = ProgramDataTypes::Double2;
				dataTypesName["int3"] = ProgramDataTypes::Integer3;
				dataTypesName["uint3"] = ProgramDataTypes::UnsignedInteger3;
				dataTypesName["float3"] = ProgramDataTypes::Float3;
				dataTypesName["double3"] = ProgramDataTypes::Double3;
				dataTypesName["int4"] = ProgramDataTypes::Integer4;
				dataTypesName["uint4"] = ProgramDataTypes::UnsignedInteger4;
				dataTypesName["float4"] = ProgramDataTypes::Float4;
				dataTypesName["double4"] = ProgramDataTypes::Double4;
				dataTypesName["matrix4f"] = ProgramDataTypes::Matrix4F;
				dataTypesName["matrix4d"] = ProgramDataTypes::Matrix4D;
				dataTypesName["texture1D"] = ProgramDataTypes::Texture1D;
				dataTypesName["texture2D"] = ProgramDataTypes::Texture2D;
				dataTypesName["texture3D"] = ProgramDataTypes::Texture3D;
				dataTypesName["textureCube"] = ProgramDataTypes::TextureCube;
				dataTypesName["texture1DRW"] = ProgramDataTypes::Texture1DRW;
				dataTypesName["texture2DRW"] = ProgramDataTypes::Texture2DRW;
				dataTypesName["texture3DRW"] = ProgramDataTypes::Texture3DRW;
				dataTypesName["buffer"] = ProgramDataTypes::Buffer;
				dataTypesName["bufferRW"] = ProgramDataTypes::BufferRW;
			}

			if (dataTypesName.Contains(Name))
				return dataTypesName[Name];

			return ProgramDataTypes::Unknown;
		}
	}
}