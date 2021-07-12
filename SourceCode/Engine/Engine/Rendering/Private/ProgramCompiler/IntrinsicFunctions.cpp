// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ProgramCompiler\IntrinsicFunctions.h>
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
				uint32 CalculateFunctionSignatureHash(const String& Name, const ProgramDataTypes* ParameterTypes, uint8 ParameterTypeCount)
				{
					return Hash::CRC32(Name.GetValue(), Name.GetLength()) + Hash::CRC32(ParameterTypes, ParameterTypeCount);
				}

				IntrinsicFunctions::IntrinsicFunctions(void) :
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
							{ \
								FunctionInfo::NativeFunctionDescription& nativeFunction = function.Descriptions[(uint8)DeviceType]; \
								nativeFunction.Name = NativeName; \
							}

#define SET_CUSTOM_NATIVE_DESCRIPTION(DeviceType, CustomBuildCallback) \
							{ \
								FunctionInfo::NativeFunctionDescription& nativeFunction = function.Descriptions[(uint8)DeviceType]; \
								nativeFunction.BuildCustom = std::make_shared<FunctionInfo::NativeFunctionDescription::CustomBuildStatement>(CustomBuildCallback); \
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
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "vec2");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float2");
						}
						END_OVERRIDE();

						BEGIN_OVERRIDE(ProgramDataTypes::Float2);
						{
							ADD_PARAMETER(ProgramDataTypes::Float);
							ADD_PARAMETER(ProgramDataTypes::Float);
							SET_NATIVE_DESCRIPTION(DeviceTypes::OpenGL, "vec2");
							SET_NATIVE_DESCRIPTION(DeviceTypes::DirectX12, "float2");
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
									Shader += textureVarialeName;
									Shader += "Sampler,";
									BuildStatement(items[1], Type, Stage, Shader);
									Shader += ")";
								});
						}
						END_OVERRIDE();

					}
					END_FUNCTION();
				}

				bool IntrinsicFunctions::BuildIntrinsicFunctionCallStatement(DeviceTypes Device, FunctionCallStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					const String& functionName = Statement->GetFunctionName();

					if (!m_Functions.Contains(functionName))
						return false;

					ProgramDataTypes parameterTypes[MAX_PARAMETER_COUNT];
					uint8 parameterTypeCount = 0;
					const auto& arguments = Statement->GetArguments().GetItems();
					for (auto& statement : arguments)
						parameterTypes[parameterTypeCount++] = EvaluateProgramDataType(statement);

					uint32 hash = CalculateFunctionSignatureHash(functionName, parameterTypes, parameterTypeCount);

					auto& overrides = m_Functions[functionName];
					int32 index = overrides.FindIf([hash](auto item) { return item.Hash == hash; });
					if (index == -1)
						return false;

					auto& function = overrides[index];
					auto& description = function.Descriptions[(uint8)Device];

					if (*description.BuildCustom == nullptr)
					{
						Shader += description.Name;
						Shader += '(';

						bool isFirst = true;
						for (auto& statement : arguments)
						{
							if (!isFirst)
								Shader += ",";
							isFirst = false;

							BuildStatement(statement, Type, Stage, Shader);
						}

						Shader += ')';
					}
					else
						(*description.BuildCustom)(Statement, Type, Stage, Shader);

					return true;
				}
			}
		}
	}
}