// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Common\PrimitiveTypes.h>
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <Rendering\Private\ShaderCompiler\ShaderParser.h>
#include <Rendering\Private\ShaderCompiler\VariableType.h>
#include <Rendering\Private\ShaderCompiler\FunctionType.h>
#include <Rendering\Private\ShaderCompiler\AssignmentStatement.h>
#include <Rendering\Private\ShaderCompiler\VariableStatement.h>
#include <Rendering\Private\ShaderCompiler\FunctionCallStatement.h>
#include <Rendering\Private\ShaderCompiler\ConstantStatement.h>
#include <Rendering\Private\ShaderCompiler\SemicolonStatement.h>
#include <Containers\Strings.h>
#include <Containers\StringUtility.h>
#include <Containers\Map.h>


#include <Platform\PlatformFile.h>

using namespace Engine::Platform;

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

				class IAPICompiler
				{
				public:
					virtual bool Compile(const ShaderParser::VariableTypeList &Variables, const ShaderParser::FunctionTypeList &Functions, String &VertexShader, String &FragmentShader) = 0;
				};

				class OpenGLCompiler : public IAPICompiler
				{
				private:
					typedef Map<String, String> OutputMap;

				public:
					bool Compile(const ShaderParser::VariableTypeList &Variables, const ShaderParser::FunctionTypeList &Functions, String &VertexShader, String &FragmentShader) override
					{
						BuildVertexShader(Variables, Functions, VertexShader);

						BuildFragmentShader(Variables, Functions, FragmentShader);


						PlatformFile::Handle handle = PlatformFile::Open(L"D:/vert.shader", PlatformFile::OpenModes::Output);
						PlatformFile::Write(handle, VertexShader.GetValue());
						PlatformFile::Close(handle);

						handle = PlatformFile::Open(L"D:/frag.shader", PlatformFile::OpenModes::Output);
						PlatformFile::Write(handle, FragmentShader.GetValue());
						PlatformFile::Close(handle);



						return true;
					}

				private:
					void BuildVertexShader(const ShaderParser::VariableTypeList &Variables, const ShaderParser::FunctionTypeList &Functions, String &Shader)
					{
						BuildHeader(Shader);

						BuildVariabes(Variables, true, Shader);

						BuildFunctions(Functions, FunctionType::Types::VertexMain, true, Shader);
					}

					void BuildFragmentShader(const ShaderParser::VariableTypeList &Variables, const ShaderParser::FunctionTypeList &Functions, String &Shader)
					{
						BuildHeader(Shader);

						BuildVariabes(Variables, false, Shader);

						BuildFunctions(Functions, FunctionType::Types::FragmentMain, false, Shader);
					}

					void BuildVariabes(const ShaderParser::VariableTypeList &Variables, bool OutputMode, String &Shader)
					{
						for each (auto var in Variables)
							BuildVariable(var->GetName(), var->GetRegister(), var->GetDataType(), OutputMode, Shader);
					}

					void BuildVariable(String Name, const String &Register, DataTypes DataType, bool OutputMode, String &Shader)
					{
						bool buildOutVarialbe = false;

						if (Register.GetLength() == 0)
							Shader += "uniform ";
						else
						{
							if (m_Outputs.Contains(Name))
							{
								Name = m_Outputs[Name];

								Shader += (OutputMode ? "out " : "in ");
							}
							else
							{
								m_Outputs[Name] = Name + "Out";

								Shader += "layout(location=";
								Shader += StringUtility::ToString<char8>(GetRegisterIndex(Register));
								Shader += ") in ";

								buildOutVarialbe = true;
							}
						}

						Shader += GetDataTypeName(DataType);
						Shader += " ";
						Shader += Name;
						Shader += ";";

						if (buildOutVarialbe)
							BuildVariable(Name, Register, DataType, true, Shader);
					}

					void BuildFunctions(const ShaderParser::FunctionTypeList & Functions, FunctionType::Types Type, bool OutputMode, String & Shader)
					{
						for each (auto fn in Functions)
						{
							FunctionType::Types funcType = fn->GetType();

							if (!(funcType == FunctionType::Types::None || funcType == Type))
								continue;

							Shader += GetDataTypeName((funcType == Type ? DataTypes::Void : fn->GetReturnDataType()));
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

								Shader += GetDataTypeName(par->GetDataType());
								Shader += " ";
								Shader += par->GetName();
							}

							Shader += "){";

							BuildStatements(fn->GetStatements(), Shader);

							if (OutputMode)
								for each (auto output in m_Outputs)
								{
									Shader += output.GetSecond();
									Shader += " = ";
									Shader += output.GetFirst();
									Shader += ";";
								}

							Shader += "}";
						}
					}

					static void BuildStatements(const StatementList &Statements, String &Shader)
					{
						for each (auto statement in Statements)
							BuildStatement(statement, Shader);
					}

					static void BuildStatement(Statement *Statement, String &Shader)
					{
						if (IsAssignableFrom(Statement, AssignmentStatement))
						{
							AssignmentStatement *stm = ReinterpretCast(AssignmentStatement*, Statement);
						}
						else if (IsAssignableFrom(Statement, ConstantStatement))
						{
							ConstantStatement *stm = ReinterpretCast(ConstantStatement*, Statement);
							Shader += StringUtility::ToString<char8>(stm->GetValue());
						}
						else if (IsAssignableFrom(Statement, FunctionCallStatement))
						{
							FunctionCallStatement *stm = ReinterpretCast(FunctionCallStatement*, Statement);

							auto &funcName = stm->GetFunctionName();
							DataTypes type = ShaderParser::GetDataType(funcName);

							Shader += (type == DataTypes::Unknown ? funcName : GetDataTypeName(type));
							Shader += "(";

							bool isFirst = true;
							for each (auto argument in stm->GetArguments())
							{
								if (!isFirst)
									Shader += ",";
								isFirst = false;

								BuildStatement(argument, Shader);
							}

							Shader += ")";
						}
						else if (IsAssignableFrom(Statement, VariableStatement))
						{
							VariableStatement *stm = ReinterpretCast(VariableStatement*, Statement);

							Shader += GetDataTypeName(stm->GetDataType());
							Shader += " ";
							Shader += stm->GetName();

							ShaderCompiler::Statement *initialStm = stm->GetInitialStatement();
							if (initialStm != nullptr)
							{
								Shader += "=";
								BuildStatement(initialStm, Shader);
							}
						}
						else if (IsAssignableFrom(Statement, SemicolonStatement))
						{
							Shader += ";";
						}
					}

					static void BuildHeader(String &Shader)
					{
						Shader += "#version 330 core\n";
					}

					static String GetDataTypeName(DataTypes Type)
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

				private:
					OutputMap m_Outputs;
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
					{
						OpenGLCompiler openGL;
						return openGL.Compile(variables, functions, VertexShader, FragmentShader);
					}
					}

					return false;
				}
			}
		}
	}
}