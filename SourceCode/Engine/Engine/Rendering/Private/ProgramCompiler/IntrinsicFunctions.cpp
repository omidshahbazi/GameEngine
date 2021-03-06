// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ProgramCompiler\IntrinsicFunctions.h>
#include <Rendering\Private\ProgramCompiler\DirectXCompiler.h>
#include <Rendering\Private\ProgramCompiler\Syntax\FunctionCallStatement.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Utility\Hash.h>

namespace Engine
{
	using namespace Utility;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				IntrinsicFunctions::IntrinsicFunctions(DeviceTypes Device) :
					m_Functions(RenderingAllocators::RenderingSystemAllocator)
				{
#define BEGIN_FUNCTION(Name, OverrideCount) \
					{ \
						const String name = Name; \
						Assert(!m_Functions.Contains(name), "Function already added"); \
						FunctionOverrideInfoList& list = m_Functions[name]; \
						list.Extend(OverrideCount); \
						uint8 overrideIndex = 0;

#define BEGIN_OVERRIDE(ReturnDataType) \
						{ \
							FunctionInfo& function = list[overrideIndex++]; \
							function.Name = name; \
							function.ReturnType = ReturnDataType; \
							function.ParameterTypeCount = 0;

#define ADD_PARAMETER(DataType) function.ParameterTypes[function.ParameterTypeCount++] = DataType;

#define SET_NATIVE_DESCRIPTION(DeviceType, NativeName) \
							if (Device == DeviceType) \
							{ \
								function.NativeFunctionName = NativeName; \
								function.BuildCustom = nullptr; \
							}

#define SET_CUSTOM_NATIVE_DESCRIPTION(DeviceType, CustomBuildCallback) \
							if (Device == DeviceType) \
							{ \
								function.NativeFunctionName = ""; \
								function.BuildCustom = std::make_shared<FunctionInfo::CustomBuildStatement>(CustomBuildCallback); \
							}

#define END_OVERRIDE() \
							function.Hash = CalculateFunctionSignatureHash(name, function.ParameterTypes, function.ParameterTypeCount); \
						}

#define END_FUNCTION() \
					}

					BEGIN_FUNCTION("float2", 2);
					{
						BEGIN_OVERRIDE(ProgramDataTypes::Float2);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec2");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float2");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Float2);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec2");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float2");
						}
						END_OVERRIDE();
					}
					END_FUNCTION();

					BEGIN_FUNCTION("double2", 4);
					{
						BEGIN_OVERRIDE(ProgramDataTypes::Double2);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec2");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double2");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double2);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec2");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double2");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double2);
						{
							ADD_PARAMETER(ProgramDataTypes::Double);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec2");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double2");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double2);
						{
							ADD_PARAMETER(ProgramDataTypes::Double);
							ADD_PARAMETER(ProgramDataTypes::Double);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec2");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double2");
						}
						END_OVERRIDE();
					}
					END_FUNCTION();

					BEGIN_FUNCTION("float3", 5);
					{
						BEGIN_OVERRIDE(ProgramDataTypes::Float3);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "vec3");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float3");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Float3);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "vec3");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float3");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Float3);
						{
							ADD_PARAMETER(ProgramDataTypes::Float2);
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "vec3");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float3");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Float3);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float2);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "vec3");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float3");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Float3);
						{
							ADD_PARAMETER(ProgramDataTypes::Float3);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "vec3");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float3");
						}
						END_OVERRIDE();
					}
					END_FUNCTION();

					BEGIN_FUNCTION("double3", 7);
					{
						BEGIN_OVERRIDE(ProgramDataTypes::Double3);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec3");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double3");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double3);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec3");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double3");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double3);
						{
							ADD_PARAMETER(ProgramDataTypes::Double);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec3");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double3");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double3);
						{
							ADD_PARAMETER(ProgramDataTypes::Double);
							ADD_PARAMETER(ProgramDataTypes::Double);
							ADD_PARAMETER(ProgramDataTypes::Double);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec3");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double3");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double3);
						{
							ADD_PARAMETER(ProgramDataTypes::Double2);
							ADD_PARAMETER(ProgramDataTypes::Double);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec3");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double3");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double3);
						{
							ADD_PARAMETER(ProgramDataTypes::Double);
							ADD_PARAMETER(ProgramDataTypes::Double2);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec3");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double3");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double3);
						{
							ADD_PARAMETER(ProgramDataTypes::Double3);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec3");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double3");
						}
						END_OVERRIDE();
					}
					END_FUNCTION();

					BEGIN_FUNCTION("float4", 8);
					{
						BEGIN_OVERRIDE(ProgramDataTypes::Float4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "vec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float4");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Float4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "vec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float4");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Float4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float2);
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "vec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float4");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Float4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float2);
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "vec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float4");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Float4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float2);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "vec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float4");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Float4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float3);
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "vec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float4");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Float4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float3);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "vec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float4");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Float4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float4);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "vec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float4");
						}
						END_OVERRIDE();
					}
					END_FUNCTION();

					BEGIN_FUNCTION("double4", 10);
					{
						BEGIN_OVERRIDE(ProgramDataTypes::Double4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double4");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double4");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double4);
						{
							ADD_PARAMETER(ProgramDataTypes::Double);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double4");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double4);
						{
							ADD_PARAMETER(ProgramDataTypes::Double);
							ADD_PARAMETER(ProgramDataTypes::Double);
							ADD_PARAMETER(ProgramDataTypes::Double);
							ADD_PARAMETER(ProgramDataTypes::Double);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double4");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double4);
						{
							ADD_PARAMETER(ProgramDataTypes::Double2);
							ADD_PARAMETER(ProgramDataTypes::Double);
							ADD_PARAMETER(ProgramDataTypes::Double);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double4");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double4);
						{
							ADD_PARAMETER(ProgramDataTypes::Double);
							ADD_PARAMETER(ProgramDataTypes::Double2);
							ADD_PARAMETER(ProgramDataTypes::Double);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double4");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double4);
						{
							ADD_PARAMETER(ProgramDataTypes::Double);
							ADD_PARAMETER(ProgramDataTypes::Double);
							ADD_PARAMETER(ProgramDataTypes::Double2);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double4");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double4);
						{
							ADD_PARAMETER(ProgramDataTypes::Double3);
							ADD_PARAMETER(ProgramDataTypes::Double);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double4");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double4);
						{
							ADD_PARAMETER(ProgramDataTypes::Double);
							ADD_PARAMETER(ProgramDataTypes::Double3);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double4");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Double4);
						{
							ADD_PARAMETER(ProgramDataTypes::Double4);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dvec4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "double4");
						}
						END_OVERRIDE();
					}
					END_FUNCTION();

					BEGIN_FUNCTION("matrix4", 2);
					{
						BEGIN_OVERRIDE(ProgramDataTypes::Matrix4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "mat4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float4x4");
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
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "mat4");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float4x4");
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
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "texture");
							SET_CUSTOM_NATIVE_DESCRIPTION(DeviceTypes::DirectX12,
								[this](auto Statement, auto Type, auto Stage, auto& Shader)
								{
									const auto& items = Statement->GetArguments().GetItems();

									String textureVarialeName;
									BuildStatement(items[0], Type, Stage, textureVarialeName);

									Shader += textureVarialeName;
									Shader += ".Sample(";
									Shader += DirectXCompiler::GetSamplerVariableName(textureVarialeName);
									Shader += ",";
									BuildStatement(items[1], Type, Stage, Shader);
									Shader += ")";
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
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "normalize");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "normalize");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float3);
						{
							ADD_PARAMETER(ProgramDataTypes::Float3);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "normalize");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "normalize");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float4);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "normalize");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "normalize");
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
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "min");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "min");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float2);
						{
							ADD_PARAMETER(ProgramDataTypes::Float2);
							ADD_PARAMETER(ProgramDataTypes::Float2);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "min");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "min");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float3);
						{
							ADD_PARAMETER(ProgramDataTypes::Float3);
							ADD_PARAMETER(ProgramDataTypes::Float3);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "min");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "min");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float4);
							ADD_PARAMETER(ProgramDataTypes::Float4);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "min");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "min");
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
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "max");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "max");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float2);
						{
							ADD_PARAMETER(ProgramDataTypes::Float2);
							ADD_PARAMETER(ProgramDataTypes::Float2);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "max");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "max");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float3);
						{
							ADD_PARAMETER(ProgramDataTypes::Float3);
							ADD_PARAMETER(ProgramDataTypes::Float3);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "max");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "max");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float4);
							ADD_PARAMETER(ProgramDataTypes::Float4);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "max");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "max");
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
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dot");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "dot");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float);
						{
							ADD_PARAMETER(ProgramDataTypes::Float3);
							ADD_PARAMETER(ProgramDataTypes::Float3);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dot");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "dot");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float);
						{
							ADD_PARAMETER(ProgramDataTypes::Float4);
							ADD_PARAMETER(ProgramDataTypes::Float4);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "dot");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "dot");
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
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "reflect");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "reflect");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float3);
						{
							ADD_PARAMETER(ProgramDataTypes::Float3);
							ADD_PARAMETER(ProgramDataTypes::Float3);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "reflect");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "reflect");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float4);
							ADD_PARAMETER(ProgramDataTypes::Float4);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "reflect");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "reflect");
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
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "refract");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "refract");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float3);
						{
							ADD_PARAMETER(ProgramDataTypes::Float3);
							ADD_PARAMETER(ProgramDataTypes::Float3);
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "refract");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "refract");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float4);
							ADD_PARAMETER(ProgramDataTypes::Float4);
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "refract");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "refract");
						}
						END_OVERRIDE();
					}
					END_FUNCTION();

					BEGIN_FUNCTION("Length", 3);
					{
						BEGIN_OVERRIDE(ProgramDataTypes::Float);
						{
							ADD_PARAMETER(ProgramDataTypes::Float2);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "length");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "length");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float);
						{
							ADD_PARAMETER(ProgramDataTypes::Float3);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "length");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "length");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float);
						{
							ADD_PARAMETER(ProgramDataTypes::Float4);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "length");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "length");
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
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "distance");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "distance");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float);
						{
							ADD_PARAMETER(ProgramDataTypes::Float3);
							ADD_PARAMETER(ProgramDataTypes::Float3);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "distance");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "distance");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float);
						{
							ADD_PARAMETER(ProgramDataTypes::Float4);
							ADD_PARAMETER(ProgramDataTypes::Float4);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "distance");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "distance");
						}
						END_OVERRIDE();
					}
					END_FUNCTION();

					BEGIN_FUNCTION("Transpose", 1);
					{
						BEGIN_OVERRIDE(ProgramDataTypes::Matrix4);
						{
							ADD_PARAMETER(ProgramDataTypes::Matrix4);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "transpose");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "transpose");
						}
						END_OVERRIDE();
					}
					END_FUNCTION();

					BEGIN_FUNCTION("Sin", 1);
					{
						BEGIN_OVERRIDE(ProgramDataTypes::Float);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "sin");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "sin");
						}
						END_OVERRIDE();
					}
					END_FUNCTION();

					BEGIN_FUNCTION("Cos", 1);
					{
						BEGIN_OVERRIDE(ProgramDataTypes::Float);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "cos");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "cos");
						}
						END_OVERRIDE();
					}
					END_FUNCTION();

					BEGIN_FUNCTION("Tan", 1);
					{
						BEGIN_OVERRIDE(ProgramDataTypes::Float);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "tan");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "tan");
						}
						END_OVERRIDE();
					}
					END_FUNCTION();

					BEGIN_FUNCTION("ASos", 1);
					{
						BEGIN_OVERRIDE(ProgramDataTypes::Float);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "asin");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "asin");
						}
						END_OVERRIDE();
					}
					END_FUNCTION();

					BEGIN_FUNCTION("ACos", 1);
					{
						BEGIN_OVERRIDE(ProgramDataTypes::Float);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "acos");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "acos");
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
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "pow");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "pow");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float2);
						{
							ADD_PARAMETER(ProgramDataTypes::Float2);
							ADD_PARAMETER(ProgramDataTypes::Float2);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "pow");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "pow");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float3);
						{
							ADD_PARAMETER(ProgramDataTypes::Float3);
							ADD_PARAMETER(ProgramDataTypes::Float3);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "pow");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "pow");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float4);
							ADD_PARAMETER(ProgramDataTypes::Float4);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "pow");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "pow");
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
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "clamp");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "clamp");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float2);
						{
							ADD_PARAMETER(ProgramDataTypes::Float2);
							ADD_PARAMETER(ProgramDataTypes::Float2);
							ADD_PARAMETER(ProgramDataTypes::Float2);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "clamp");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "clamp");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float3);
						{
							ADD_PARAMETER(ProgramDataTypes::Float3);
							ADD_PARAMETER(ProgramDataTypes::Float3);
							ADD_PARAMETER(ProgramDataTypes::Float3);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "clamp");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "clamp");
						}
						END_OVERRIDE();
						BEGIN_OVERRIDE(ProgramDataTypes::Float4);
						{
							ADD_PARAMETER(ProgramDataTypes::Float4);
							ADD_PARAMETER(ProgramDataTypes::Float4);
							ADD_PARAMETER(ProgramDataTypes::Float4);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "clamp");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "clamp");
						}
						END_OVERRIDE();
					}
					END_FUNCTION();
				}

				bool IntrinsicFunctions::BuildIntrinsicFunctionCallStatement(FunctionCallStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					auto function = FindOverride(Statement);
					if (function == nullptr)
						return false;

					if (function->BuildCustom == nullptr)
					{
						Shader += function->NativeFunctionName;
						Shader += '(';

						BuildArguments(Statement->GetArguments(), Type, Stage, Shader);

						Shader += ')';
					}
					else
						(*function->BuildCustom)(Statement, Type, Stage, Shader);

					return true;
				}

				ProgramDataTypes IntrinsicFunctions::EvaluateIntrinsicFunctionReturnValue(FunctionCallStatement* Statement) const
				{
					auto function = FindOverride(Statement);
					if (function == nullptr)
						return ProgramDataTypes::Unknown;

					return function->ReturnType;
				}

				const IntrinsicFunctions::FunctionInfo* IntrinsicFunctions::FindOverride(FunctionCallStatement* Statement) const
				{
					const String& functionName = Statement->GetFunctionName();

					if (!m_Functions.Contains(functionName))
						return nullptr;

					uint32 hash = CalculateFunctionSignatureHash(Statement);

					auto& overrides = m_Functions[functionName];
					int32 index = overrides.FindIf([hash](auto item) { return item.Hash == hash; });
					if (index == -1)
						return nullptr;

					return &overrides[index];
				}

				uint32 IntrinsicFunctions::CalculateFunctionSignatureHash(FunctionCallStatement* Statement) const
				{
					ProgramDataTypes parameterTypes[MAX_PARAMETER_COUNT];
					uint8 parameterTypeCount = 0;
					const auto& arguments = Statement->GetArguments().GetItems();
					for (auto& statement : arguments)
						parameterTypes[parameterTypeCount++] = EvaluateProgramDataType(statement);

					return CalculateFunctionSignatureHash(Statement->GetFunctionName(), parameterTypes, parameterTypeCount);
				}

				uint32 IntrinsicFunctions::CalculateFunctionSignatureHash(const String& Name, const ProgramDataTypes* ParameterTypes, uint8 ParameterTypeCount)
				{
					return Hash::CRC32(Name.GetValue(), Name.GetLength()) + Hash::CRC32(ParameterTypes, ParameterTypeCount);
				}
			}
		}
	}
}