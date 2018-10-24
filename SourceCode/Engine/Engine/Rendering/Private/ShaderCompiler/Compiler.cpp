// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <Rendering\Private\ShaderCompiler\ShaderParser.h>
#include <Rendering\Private\ShaderCompiler\VariableType.h>
#include <Rendering\Private\ShaderCompiler\FunctionType.h>
#include <Containers\Strings.h>
#include <Containers\StringUtility.h>
#include <Containers\Map.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				const String ENTRY_POINT_NAME = "main";

				uint16 GetRegisterIndex(const String &Name)
				{
					static bool initialized = false;
					static Map<String, uint16> registers;

					if (!initialized)
					{
						initialized = true;

						registers["POSITION"] = 0;
						registers["NORMAL"] = 1;
						registers["TEXCOORD"] = 2;
					}

					if (registers.Contains(Name))
						return registers[Name];

					return -1;
				}

				class OpenGL
				{
				public:
					static bool Compile(const ShaderParser::VariableTypeList &Variables, const ShaderParser::FunctionTypeList &Functions, String & VertexShader, String & FragmentShader)
					{
						BuildVertexShader(Variables, Functions, VertexShader);
						BuildFragmentShader(Variables, Functions, FragmentShader);

						return false;
					}

				private:
					static void BuildVertexShader(const ShaderParser::VariableTypeList &Variables, const ShaderParser::FunctionTypeList &Functions, String &Shader)
					{
						BuildHeader(Shader);

						BuildVariabes(Variables, Shader);

						BuildFunctions(Functions, FunctionType::Types::VertexMain, Shader);
					}

					static void BuildFragmentShader(const ShaderParser::VariableTypeList &Variables, const ShaderParser::FunctionTypeList &Functions, String &Shader)
					{
						BuildHeader(Shader);

						BuildVariabes(Variables, Shader);

						BuildFunctions(Functions, FunctionType::Types::FragmentMain, Shader);
					}

					static void BuildHeader(String &Shader)
					{
						Shader += "#version 330 core\n";
					}

					static void BuildVariabes(const ShaderParser::VariableTypeList &Variables, String & Shader)
					{
						for each (auto var in Variables)
						{
							if (var->GetRegister().GetLength() == 0)
								Shader += "uniform ";
							else
							{
								Shader += "layout(location=";
								Shader += StringUtility::ToString<char8>(GetRegisterIndex(var->GetRegister()));
								Shader += ") in ";
							}

							Shader += GetTypeNameByDataType(var->GetDataType());
							Shader += " ";
							Shader += var->GetName();
							Shader += ";";
						}
					}

					static void BuildFunctions(const ShaderParser::FunctionTypeList & Functions, FunctionType::Types Type, String & Shader)
					{
						for each (auto fn in Functions)
						{
							FunctionType::Types funcType = fn->GetType();

							if (!(funcType == FunctionType::Types::None || funcType == Type))
								continue;

							Shader += GetTypeNameByDataType((funcType == Type ? DataTypes::Void : fn->GetReturnDataType()));
							Shader += " ";

							if (fn->GetType() == Type)
								Shader += ENTRY_POINT_NAME;
							else
								Shader += fn->GetName();

							Shader += "(";

							bool isFirst = true;
							for each (auto par in fn->GetParameters())
							{
								if (!isFirst)
									Shader += ",";
								isFirst = false;

								Shader += GetTypeNameByDataType(par->GetDataType());
								Shader += " ";
								Shader += par->GetName();
							}

							Shader += "){";

							Shader += "}";
						}
					}

					static String GetTypeNameByDataType(DataTypes Type)
					{
						switch (Type)
						{
						case DataTypes::Void:
							return "void";

						case DataTypes::Float:
							return "float";

						case DataTypes::Float2:
							return "vec2";

						case DataTypes::Float3:
							return "vec3";

						case DataTypes::Float4:
							return "vec4";

						case DataTypes::Matrix4:
							return "mat4";
						}

						return "";
					}
				};

				bool Compiler::Compile(DeviceInterfarce::Type DeviceType, const String &Shader, String &VertexShader, String &FragmentShader)
				{
					ShaderParser parser(Shader);

					ShaderParser::VariableTypeList variables;
					ShaderParser::FunctionTypeList functions;
					parser.Parse(variables, functions);

					switch (DeviceType)
					{
					case DeviceInterfarce::Type::OpenGL:
						return OpenGL::Compile(variables, functions, VertexShader, FragmentShader);
					}

					return false;
				}
			}
		}
	}
}