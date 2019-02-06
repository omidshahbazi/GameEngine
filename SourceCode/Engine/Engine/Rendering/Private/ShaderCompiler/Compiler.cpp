// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Common\PrimitiveTypes.h>
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <Rendering\Private\ShaderCompiler\ShaderParser.h>
#include <Rendering\Private\ShaderCompiler\VariableType.h>
#include <Rendering\Private\ShaderCompiler\FunctionType.h>
#include <Rendering\Private\ShaderCompiler\IfStatement.h>
#include <Rendering\Private\ShaderCompiler\ElseStatement.h>
#include <Rendering\Private\ShaderCompiler\SwitchStatement.h>
#include <Rendering\Private\ShaderCompiler\CaseStatement.h>
#include <Rendering\Private\ShaderCompiler\ForStatement.h>
#include <Rendering\Private\ShaderCompiler\DoStatement.h>
#include <Rendering\Private\ShaderCompiler\WhileStatement.h>
#include <Rendering\Private\ShaderCompiler\ContinueStatement.h>
#include <Rendering\Private\ShaderCompiler\BreakStatement.h>
#include <Rendering\Private\ShaderCompiler\ReturnStatement.h>
#include <Rendering\Private\ShaderCompiler\DiscardStatement.h>
#include <Rendering\Private\ShaderCompiler\OperatorStatement.h>
#include <Rendering\Private\ShaderCompiler\VariableStatement.h>
#include <Rendering\Private\ShaderCompiler\FunctionCallStatement.h>
#include <Rendering\Private\ShaderCompiler\ConstantStatement.h>
#include <Rendering\Private\ShaderCompiler\VariableAccessStatement.h>
#include <Rendering\Private\ShaderCompiler\MemberAccessStatement.h>
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

				Mesh::SubMesh::VertexLayouts GetLayout(const String &Name)
				{
					static bool initialized = false;
					static Map<String, Mesh::SubMesh::VertexLayouts> registers;

					if (!initialized)
					{
						initialized = true;

						registers["POSITION"] = Mesh::SubMesh::VertexLayouts::Position;
						registers["NORMAL"] = Mesh::SubMesh::VertexLayouts::Normal;
						registers["UV"] = Mesh::SubMesh::VertexLayouts::UV;
					}

					if (registers.Contains(Name))
						return registers[Name];

					return (Mesh::SubMesh::VertexLayouts)0;
				}

				class IAPICompiler
				{
				public:
					virtual bool Compile(const ShaderParser::VariableTypeList &Variables, const ShaderParser::FunctionTypeList &Functions, String &VertexShader, String &FragmentShader) = 0;
				};

				class OpenGLCompiler : public IAPICompiler
				{
				private:
					enum class Stages
					{
						Vertex = 0,
						Fragment
					};

					typedef Map<String, String> OutputMap;

				public:
					bool Compile(const ShaderParser::VariableTypeList &Variables, const ShaderParser::FunctionTypeList &Functions, String &VertexShader, String &FragmentShader) override
					{
						BuildVertexShader(Variables, Functions, VertexShader);

						BuildFragmentShader(Variables, Functions, FragmentShader);

						//VertexShader =  "#version 330 core\n"
						// "layout (location = 0) in vec3 aPos;\n"
						// "layout (location = 1) in vec2 uv;\n"
						// "out vec2 uvOut;\n"
						// "void main()\n"
						// "{\n"
						// "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
						// "   uvOut = uv;\n"
						//	"}\0";

						//FragmentShader = "#version 330 core\n"
						// "out vec4 FragColor;\n"
						// "in vec2 uvOut;\n"
						// "void main()\n"
						// "{\n"
						// "   FragColor = vec4(uvOut.x, uvOut.y, 0.0f, 1.0f);\n"
						// "}\n\0";

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

						BuildVariables(Variables, Stages::Vertex, Shader);

						BuildFunctions(Functions, FunctionType::Types::VertexMain, Stages::Vertex, Shader);
					}

					void BuildFragmentShader(const ShaderParser::VariableTypeList &Variables, const ShaderParser::FunctionTypeList &Functions, String &Shader)
					{
						BuildHeader(Shader);

						BuildVariables(Variables, Stages::Fragment, Shader);

						BuildFunctions(Functions, FunctionType::Types::FragmentMain, Stages::Fragment, Shader);
					}

					void BuildVariables(const ShaderParser::VariableTypeList &Variables, Stages Stage, String &Shader)
					{
						for each (auto var in Variables)
							BuildVariable(var->GetName(), var->GetRegister(), var->GetDataType(), var->GetIsConstant(), Stage == Stages::Vertex, Shader);

						if (Stage == Stages::Fragment)
						{
							Shader += "out ";
							Shader += GetDataTypeName(DataTypes::Float4);
							Shader += " ";
							Shader += GetFragmentVariableName();
							Shader += ";";
						}
					}

					void BuildVariable(String Name, const String &Register, DataTypes DataType, bool IsConstant, bool IsOutputMode, String &Shader)
					{
						bool buildOutVarialbe = false;

						if (IsConstant)
							Shader += "uniform ";
						else if (Register.GetLength() != 0)
						{
							if (m_Outputs.Contains(Name))
							{
								Name = m_Outputs[Name];

								Shader += (IsOutputMode ? "out " : "in ");
							}
							else
							{
								m_Outputs[Name] = Name + "Out";

								Shader += "layout(location=";
								Shader += StringUtility::ToString<char8>(SubMeshInfo::GetLayoutIndex(GetLayout(Register)));
								Shader += ") in ";

								buildOutVarialbe = true;
							}
						}

						Shader += GetDataTypeName(DataType);
						Shader += " ";
						Shader += Name;
						Shader += ";";

						if (buildOutVarialbe)
							BuildVariable(Name, Register, DataType, false, true, Shader);
					}

					void BuildFunctions(const ShaderParser::FunctionTypeList & Functions, FunctionType::Types Type, Stages Stage, String & Shader)
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

							BuildStatements(fn->GetStatements(), fn->GetType(), Stage, Shader);

							if (Stage == Stages::Vertex)
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

					void BuildStatements(const StatementList &Statements, FunctionType::Types Type, Stages Stage, String &Shader)
					{
						for each (auto statement in Statements)
							BuildStatement(statement, Type, Stage, Shader);
					}

					void BuildStatement(Statement *Statement, FunctionType::Types Type, Stages Stage, String &Shader)
					{
						if (IsAssignableFrom(Statement, OperatorStatement))
						{
							OperatorStatement *stm = ReinterpretCast(OperatorStatement*, Statement);

							//Shader += "(";

							BuildStatement(stm->GetLeft(), Type, Stage, Shader);

							Shader += OperatorStatement::GetOperatorSymbol(stm->GetOperator());

							BuildStatement(stm->GetRight(), Type, Stage, Shader);

							//Shader += ")";
						}
						else if (IsAssignableFrom(Statement, ConstantStatement))
						{
							ConstantStatement *stm = ReinterpretCast(ConstantStatement*, Statement);

							if (stm->GetType() == ConstantStatement::Types::Boolean)
								Shader += StringUtility::ToString<char8>(stm->GetBool());
							else
								Shader += StringUtility::ToString<char8>(stm->GetFloat32());
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

								BuildStatement(argument, Type, Stage, Shader);
							}

							Shader += ")";
						}
						else if (IsAssignableFrom(Statement, VariableStatement))
						{
							VariableStatement *stm = ReinterpretCast(VariableStatement*, Statement);

							Shader += GetDataTypeName(stm->GetDataType());
							Shader += " ";
							Shader += stm->GetName();

							if (stm->GetInitialStatement() != nullptr)
							{
								Shader += "=";
								BuildStatement(stm->GetInitialStatement(), Type, Stage, Shader);
							}
						}
						else if (IsAssignableFrom(Statement, VariableAccessStatement))
						{
							VariableAccessStatement *stm = ReinterpretCast(VariableAccessStatement*, Statement);

							String name = stm->GetName();

							if (Stage == Stages::Fragment && m_Outputs.Contains(name))
								name = m_Outputs[stm->GetName()];

							Shader += name;
						}
						else if (IsAssignableFrom(Statement, MemberAccessStatement))
						{
							MemberAccessStatement *stm = ReinterpretCast(MemberAccessStatement*, Statement);

							BuildStatement(stm->GetLeft(), Type, Stage, Shader);

							Shader += ".";

							BuildStatement(stm->GetRight(), Type, Stage, Shader);
						}
						else if (IsAssignableFrom(Statement, SemicolonStatement))
						{
							Shader += ";";

#ifdef DEBUG_MODE
							Shader += "\n";
#endif
						}
						else if (IsAssignableFrom(Statement, IfStatement))
						{
							IfStatement *stm = ReinterpretCast(IfStatement*, Statement);

							Shader += "if (";

							BuildStatement(stm->GetCondition(), Type, Stage, Shader);

							Shader += "){";

							BuildStatements(stm->GetStatements(), Type, Stage, Shader);

							Shader += "}";

							if (stm->GetElse() != nullptr)
								BuildStatement(stm->GetElse(), Type, Stage, Shader);
						}
						else if (IsAssignableFrom(Statement, ElseStatement))
						{
							ElseStatement *stm = ReinterpretCast(ElseStatement*, Statement);

							Shader += "else {";

							BuildStatements(stm->GetStatements(), Type, Stage, Shader);

							Shader += "}";
						}
						else if (IsAssignableFrom(Statement, ReturnStatement))
						{
							ReturnStatement *stm = ReinterpretCast(ReturnStatement*, Statement);

							if (Type == FunctionType::Types::VertexMain)
								Shader += "gl_Position=";
							else if (Type == FunctionType::Types::FragmentMain)
								Shader += GetFragmentVariableName() + "=";
							else
								Shader += "return ";

							BuildStatement(stm->GetStatement(), Type, Stage, Shader);
						}
						else if (IsAssignableFrom(Statement, DiscardStatement))
						{
							Shader += "discard";
						}
						else
							Assert(false, "Unsupported Statement");
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

						case DataTypes::Texture2D:
							return "sampler2D";
						}

						return "";
					}

					INLINE static const String &GetFragmentVariableName(void)
					{
						static String name = FRAGMENT_ENTRY_POINT_NAME + "_FragColor";

						return name;
					}

				private:
					OutputMap m_Outputs;
				};

				bool Compiler::Compile(DeviceInterface::Type DeviceType, const String &Shader, String &VertexShader, String &FragmentShader)
				{
					ShaderParser parser(Shader);

					ShaderParser::VariableTypeList variables;
					ShaderParser::FunctionTypeList functions;
					parser.Parse(variables, functions);

					switch (DeviceType)
					{
					case DeviceInterface::Type::OpenGL:
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