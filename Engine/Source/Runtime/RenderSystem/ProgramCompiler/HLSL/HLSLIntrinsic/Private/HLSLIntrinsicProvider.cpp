// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <HLSLIntrinsic\Private\HLSLIntrinsicProvider.h>
#include <ASTToHLSLCompiler\Private\ASTToHLSLCompiler.h>

namespace Engine
{
	namespace HLSLIntrinsic
	{
		namespace Private
		{
			void HLSLIntrinsicProvider::Initialize(IInitializeHelper* Helper)
			{
				ADD_CONSTANT("_FragPosition", ProgramDataTypes::Float2, [this](auto Compiler, auto Type, auto Stage, auto& Shader)
					{
						if (Type != FunctionType::Types::FragmentMain)
							return;

						Compiler->InjectParameterIntoTopFunction(ProgramDataTypes::Float4, "dx_FragCoord", "SV_Position");

						Shader += "dx_FragCoord.xy";
					});

				BEGIN_FUNCTION("float2", 2);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("float2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("float2");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("double2", 4);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Double2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("double2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double2);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("double2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double2);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("double2");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double2);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("double2");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("float3", 5);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("float3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("float3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("float3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("float3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float3);
						SET_NATIVE_DESCRIPTION("float3");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("double3", 7);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("double3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("double3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("double3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("double3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Double2);
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("double3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double2);
						SET_NATIVE_DESCRIPTION("double3");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double3);
					{
						ADD_PARAMETER(ProgramDataTypes::Double3);
						SET_NATIVE_DESCRIPTION("double3");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("float4", 8);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("float4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("float4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("float4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("float4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_NATIVE_DESCRIPTION("float4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float3);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("float4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float3);
						SET_NATIVE_DESCRIPTION("float4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float4);
						SET_NATIVE_DESCRIPTION("float4");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("double4", 10);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("double4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("double4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("double4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("double4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Double2);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("double4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double2);
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("double4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double2);
						SET_NATIVE_DESCRIPTION("double4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Double3);
						ADD_PARAMETER(ProgramDataTypes::Double);
						SET_NATIVE_DESCRIPTION("double4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Double);
						ADD_PARAMETER(ProgramDataTypes::Double3);
						SET_NATIVE_DESCRIPTION("double4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Double4);
					{
						ADD_PARAMETER(ProgramDataTypes::Double4);
						SET_NATIVE_DESCRIPTION("double4");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("matrix4", 2);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Matrix4);
					{
						ADD_PARAMETER(ProgramDataTypes::Float);
						SET_NATIVE_DESCRIPTION("float4x4");
					}
					END_OVERRIDE();

					BEGIN_OVERRIDE(ProgramDataTypes::Matrix4);
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
						SET_NATIVE_DESCRIPTION("float4x4");
					}
					END_OVERRIDE();
				}
				END_FUNCTION();

				BEGIN_FUNCTION("Sample", 1);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Texture2D);
						ADD_PARAMETER(ProgramDataTypes::Float2);
						SET_CUSTOM_NATIVE_DESCRIPTION([this](auto Compiler, auto Arguments, auto Type, auto Stage, auto& Shader)
							{
								String textureVarialeName;
								Compiler->BuildStatement(Arguments[0], Type, Stage, textureVarialeName);

								Shader += textureVarialeName;
								Shader += ".Sample(";
								Shader += ASTToHLSLCompiler::Private::ASTToHLSLCompiler::GetSamplerVariableName(textureVarialeName);
								Shader += ',';
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

				BEGIN_FUNCTION("Multiply", 2);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Float4);
					{
						ADD_PARAMETER(ProgramDataTypes::Matrix4);
						ADD_PARAMETER(ProgramDataTypes::Float4);
						SET_NATIVE_DESCRIPTION("mul");
					}
					END_OVERRIDE();
					BEGIN_OVERRIDE(ProgramDataTypes::Matrix4);
					{
						ADD_PARAMETER(ProgramDataTypes::Matrix4);
						ADD_PARAMETER(ProgramDataTypes::Matrix4);
						SET_NATIVE_DESCRIPTION("mul");
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
						SET_NATIVE_DESCRIPTION("fmod");
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

				BEGIN_FUNCTION("Transpose", 1);
				{
					BEGIN_OVERRIDE(ProgramDataTypes::Matrix4);
					{
						ADD_PARAMETER(ProgramDataTypes::Matrix4);
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
			}
		}
	}
}