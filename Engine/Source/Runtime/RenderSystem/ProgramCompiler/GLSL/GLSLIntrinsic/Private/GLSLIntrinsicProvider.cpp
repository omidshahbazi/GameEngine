// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <GLSLIntrinsic\Private\GLSLIntrinsicProvider.h>
#include <ASTCompiler\ASTCompilerBase.h>

namespace Engine
{
	namespace GLSLIntrinsic
	{
		namespace Private
		{
			void GLSLIntrinsicProvider::Initialize(IInitializeHelper* Helper)
			{
				BEGIN_FUNCTION("int2", 6);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Integer2);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("ivec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer2);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("ivec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer2);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						SET_NATIVE_DESCRIPTION("ivec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer2);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("ivec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer2);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("ivec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer2);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						SET_NATIVE_DESCRIPTION("ivec2");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("uint2", 6);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger2);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("uvec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger2);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("uvec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger2);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						SET_NATIVE_DESCRIPTION("uvec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger2);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("uvec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger2);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("uvec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger2);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						SET_NATIVE_DESCRIPTION("uvec2");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("float2", 11);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("vec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("vec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						SET_NATIVE_DESCRIPTION("vec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("vec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("vec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						SET_NATIVE_DESCRIPTION("vec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("vec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("vec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("vec2");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("double2", 12);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Double2);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("dvec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double2);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("dvec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double2);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						SET_NATIVE_DESCRIPTION("dvec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double2);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("dvec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double2);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("dvec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double2);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						SET_NATIVE_DESCRIPTION("dvec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("dvec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("dvec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("dvec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double2);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("dvec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double2);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("dvec2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double2);
					{
						ADD_PARAMETER(ProgramDataTypes::Double2);
						SET_NATIVE_DESCRIPTION("dvec2");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("int3", 10);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Integer3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("ivec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("ivec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("ivec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						SET_NATIVE_DESCRIPTION("ivec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger3);
						SET_NATIVE_DESCRIPTION("ivec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("ivec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("ivec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("ivec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						SET_NATIVE_DESCRIPTION("ivec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer3);
						SET_NATIVE_DESCRIPTION("ivec3");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("uint3", 10);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("uvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("uvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("uvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						SET_NATIVE_DESCRIPTION("uvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer3);
						SET_NATIVE_DESCRIPTION("uvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("uvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("uvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("uvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						SET_NATIVE_DESCRIPTION("uvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger3);
						SET_NATIVE_DESCRIPTION("uvec3");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("float3", 15);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("vec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("vec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("vec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						SET_NATIVE_DESCRIPTION("vec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer3);
						SET_NATIVE_DESCRIPTION("vec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("vec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("vec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("vec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						SET_NATIVE_DESCRIPTION("vec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger3);
						SET_NATIVE_DESCRIPTION("vec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("vec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("vec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("vec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("vec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float3);
						SET_NATIVE_DESCRIPTION("vec3");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("double3", 22);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer3);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger3);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float3);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Double2);
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double2);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Double3);
						SET_NATIVE_DESCRIPTION("dvec3");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("int4", 16);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Integer4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("ivec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("ivec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("ivec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("ivec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						SET_NATIVE_DESCRIPTION("ivec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger3);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("ivec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger3);
						SET_NATIVE_DESCRIPTION("ivec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger4);
						SET_NATIVE_DESCRIPTION("ivec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("ivec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("ivec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("ivec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("ivec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						SET_NATIVE_DESCRIPTION("ivec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer3);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("ivec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer3);
						SET_NATIVE_DESCRIPTION("ivec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Integer4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer4);
						SET_NATIVE_DESCRIPTION("ivec4");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("uint4", 16);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("uvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("uvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("uvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("uvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						SET_NATIVE_DESCRIPTION("uvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer3);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("uvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer3);
						SET_NATIVE_DESCRIPTION("uvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer4);
						SET_NATIVE_DESCRIPTION("uvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("uvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("uvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("uvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("uvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						SET_NATIVE_DESCRIPTION("uvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger3);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("uvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger3);
						SET_NATIVE_DESCRIPTION("uvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::UnsignedInteger4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger4);
						SET_NATIVE_DESCRIPTION("uvec4");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("float4", 24);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer3);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer3);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer4);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger3);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger3);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger4);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float3);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float3);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float4);
						SET_NATIVE_DESCRIPTION("vec4");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("double4", 34);
				{

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer2);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer3);
						ADD_PARAMETER(ProgramDataTypes::Integer);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer);
						ADD_PARAMETER(ProgramDataTypes::Integer3);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Integer4);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger2);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger3);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger);
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger3);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::UnsignedInteger4);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float3);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float3);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float4);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Double2);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double2);
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double2);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Double3);
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double3);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Double4);
						SET_NATIVE_DESCRIPTION("dvec4");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("matrix4f", 2);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Matrix4F);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("mat4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Matrix4F);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("mat4");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("matrix4D", 2);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Matrix4D);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("mat4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Matrix4D);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("mat4");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Sample", 1);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Texture1D);
						ADD_PARAMETER(ProgramDataTypes::Float);						
						SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
							{
								String textureVarialeName;
								Compiler->BuildStatement(Arguments[0], Type, Stage, textureVarialeName);

								Shader += "texture(";
								Shader += ASTCompiler::ASTCompilerBase::GetSamplerVariableName(textureVarialeName);
								Shader += ", ";
								Compiler->BuildStatement(Arguments[1], Type, Stage, Shader);
								Shader += ')';
							});
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Normalize", 3);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("normalize");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float3);
						SET_NATIVE_DESCRIPTION("normalize");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float4);
						SET_NATIVE_DESCRIPTION("normalize");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Min", 4);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("min");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("min");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float3);
						ADD_PARAMETER(ProgramDataTypes::Float3);
						SET_NATIVE_DESCRIPTION("min");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float4);
						ADD_PARAMETER(ProgramDataTypes::Float4);
						SET_NATIVE_DESCRIPTION("min");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Max", 4);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("max");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("max");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float3);
						ADD_PARAMETER(ProgramDataTypes::Float3);
						SET_NATIVE_DESCRIPTION("max");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float4);
						ADD_PARAMETER(ProgramDataTypes::Float4);
						SET_NATIVE_DESCRIPTION("max");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Multiply", 8);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Matrix4F);
						ADD_PARAMETER(ProgramDataTypes::Float4);
						SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
							{
								Shader += '(';
								Compiler->BuildStatement(Arguments[0], Type, Stage, Shader);
								Shader += '*';
								Compiler->BuildStatement(Arguments[1], Type, Stage, Shader);
								Shader += ')';
							});
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Matrix4D);
						ADD_PARAMETER(ProgramDataTypes::Float4);
						SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
							{
								Shader += '(';
								Compiler->BuildStatement(Arguments[0], Type, Stage, Shader);
								Shader += '*';
								Compiler->BuildStatement(Arguments[1], Type, Stage, Shader);
								Shader += ')';
							});
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Matrix4F);
						ADD_PARAMETER(ProgramDataTypes::Double4);
						SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
							{
								Shader += '(';
								Compiler->BuildStatement(Arguments[0], Type, Stage, Shader);
								Shader += '*';
								Compiler->BuildStatement(Arguments[1], Type, Stage, Shader);
								Shader += ')';
							});
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Matrix4D);
						ADD_PARAMETER(ProgramDataTypes::Double4);
						SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
							{
								Shader += '(';
								Compiler->BuildStatement(Arguments[0], Type, Stage, Shader);
								Shader += '*';
								Compiler->BuildStatement(Arguments[1], Type, Stage, Shader);
								Shader += ')';
							});
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Matrix4F);
					{
						ADD_PARAMETER(ProgramDataTypes::Matrix4F);
						ADD_PARAMETER(ProgramDataTypes::Matrix4F);
						SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
							{
								Shader += '(';
								Compiler->BuildStatement(Arguments[0], Type, Stage, Shader);
								Shader += '*';
								Compiler->BuildStatement(Arguments[1], Type, Stage, Shader);
								Shader += ')';
							});
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Matrix4D);
					{
						ADD_PARAMETER(ProgramDataTypes::Matrix4D);
						ADD_PARAMETER(ProgramDataTypes::Matrix4D);
						SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
							{
								Shader += '(';
								Compiler->BuildStatement(Arguments[0], Type, Stage, Shader);
								Shader += '*';
								Compiler->BuildStatement(Arguments[1], Type, Stage, Shader);
								Shader += ')';
							});
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Matrix4D);
					{
						ADD_PARAMETER(ProgramDataTypes::Matrix4D);
						ADD_PARAMETER(ProgramDataTypes::Matrix4F);
						SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
							{
								Shader += '(';
								Compiler->BuildStatement(Arguments[0], Type, Stage, Shader);
								Shader += '*';
								Compiler->BuildStatement(Arguments[1], Type, Stage, Shader);
								Shader += ')';
							});
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Matrix4D);
					{
						ADD_PARAMETER(ProgramDataTypes::Matrix4F);
						ADD_PARAMETER(ProgramDataTypes::Matrix4D);
						SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
							{
								Shader += '(';
								Compiler->BuildStatement(Arguments[0], Type, Stage, Shader);
								Shader += '*';
								Compiler->BuildStatement(Arguments[1], Type, Stage, Shader);
								Shader += ')';
							});
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Reminder", 1);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("mod");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Dot", 3);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("dot");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float3);
						ADD_PARAMETER(ProgramDataTypes::Float3);
						SET_NATIVE_DESCRIPTION("dot");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float4);
						ADD_PARAMETER(ProgramDataTypes::Float4);
						SET_NATIVE_DESCRIPTION("dot");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Reflect", 3);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("reflect");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float3);
						ADD_PARAMETER(ProgramDataTypes::Float3);
						SET_NATIVE_DESCRIPTION("reflect");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float4);
						ADD_PARAMETER(ProgramDataTypes::Float4);
						SET_NATIVE_DESCRIPTION("reflect");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Refract", 3);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("refract");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float3);
						ADD_PARAMETER(ProgramDataTypes::Float3);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("refract");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float4);
						ADD_PARAMETER(ProgramDataTypes::Float4);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("refract");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Length", 3);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("length");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float3);
						SET_NATIVE_DESCRIPTION("length");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float4);
						SET_NATIVE_DESCRIPTION("length");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Distance", 3);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("distance");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float3);
						ADD_PARAMETER(ProgramDataTypes::Float3);
						SET_NATIVE_DESCRIPTION("distance");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float4);
						ADD_PARAMETER(ProgramDataTypes::Float4);
						SET_NATIVE_DESCRIPTION("distance");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Transpose", 2);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Matrix4F);
					{
						ADD_PARAMETER(ProgramDataTypes::Matrix4F);
						SET_NATIVE_DESCRIPTION("transpose");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Matrix4D);
					{
						ADD_PARAMETER(ProgramDataTypes::Matrix4D);
						SET_NATIVE_DESCRIPTION("transpose");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Sin", 1);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("sin");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Cos", 1);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("cos");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Tan", 1);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("tan");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("ASos", 1);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("asin");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("ACos", 1);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("acos");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Pow", 4);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("pow");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("pow");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float3);
						ADD_PARAMETER(ProgramDataTypes::Float3);
						SET_NATIVE_DESCRIPTION("pow");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float4);
						ADD_PARAMETER(ProgramDataTypes::Float4);
						SET_NATIVE_DESCRIPTION("pow");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Clamp", 4);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("clamp");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("clamp");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float3);
						ADD_PARAMETER(ProgramDataTypes::Float3);
						ADD_PARAMETER(ProgramDataTypes::Float3);
						SET_NATIVE_DESCRIPTION("clamp");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float4);
						ADD_PARAMETER(ProgramDataTypes::Float4);
						ADD_PARAMETER(ProgramDataTypes::Float4);
						SET_NATIVE_DESCRIPTION("clamp");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("AppendToStream", 1);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Void);
					{
						MARK_AS_TEMPLATE();
						SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
							{
								Shader += "EmitVertex();\n";
							});
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("RestartStrip", 1);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Void);
					{
						MARK_AS_TEMPLATE();
						SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
							{
								Shader += "EndPrimitive();\n";
							});
					}
					END_OVERRIDE();
				}
				END_FUNCTION();
			}
		}
	}
}