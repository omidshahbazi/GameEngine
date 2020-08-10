// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <Rendering\Private\ShaderCompiler\ShaderParser.h>
#include <Rendering\Private\ShaderCompiler\ShaderParserPreprocess.h>
#include <Rendering\Private\ShaderCompiler\Syntax\VariableType.h>
#include <Rendering\Private\ShaderCompiler\Syntax\FunctionType.h>
#include <Rendering\Private\ShaderCompiler\Syntax\IfStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\ElseStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\SwitchStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\CaseStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\ForStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\DoStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\WhileStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\ContinueStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\BreakStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\ReturnStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\DiscardStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\OperatorStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\UnaryOperatorStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\VariableStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\FunctionCallStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\ConstantStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\VariableAccessStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\ArrayElementAccessStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\MemberAccessStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\SemicolonStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\ArrayStatement.h>
#include <Common\PrimitiveTypes.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <MemoryManagement\Allocator\FrameAllocator.h>
#include <Containers\Strings.h>
#include <Containers\StringUtility.h>
#include <Containers\Map.h>

#ifdef DEBUG_MODE
#define ADD_NEW_LINE() Shader += "\n"
#else
#define ADD_NEW_LINE()
#endif

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				using namespace Syntax;

				const String ENTRY_POINT_NAME = "main";
				const String MUST_RETURN_NAME = "_MustReturn";

				SubMesh::VertexLayouts GetLayout(const String& Name)
				{
					static bool initialized = false;
					static Map<String, SubMesh::VertexLayouts> registers;

					if (!initialized)
					{
						initialized = true;

						registers["POSITION"] = SubMesh::VertexLayouts::Position;
						registers["NORMAL"] = SubMesh::VertexLayouts::Normal;
						registers["UV"] = SubMesh::VertexLayouts::UV;
					}

					if (registers.Contains(Name))
						return registers[Name];

					return (SubMesh::VertexLayouts)0;
				}

				class APICompiler
				{
				protected:
					enum class Stages
					{
						Vertex = 0,
						Fragment
					};

				public:
					APICompiler(const String& Version) :
						m_Version(Version)
					{
					}

					virtual bool Compile(const ShaderParser::VariableTypeList& Variables, const ShaderParser::FunctionTypeList& Functions, String& VertexShader, String& FragmentShader)
					{
						BuildVertexShader(Variables, Functions, VertexShader);

						BuildFragmentShader(Variables, Functions, FragmentShader);

						return true;
					}

				protected:
					virtual void BuildVertexShader(const ShaderParser::VariableTypeList& Variables, const ShaderParser::FunctionTypeList& Functions, String& Shader)
					{
						BuildHeader(Shader);

						BuildVariables(Variables, Stages::Vertex, Shader);

						BuildFunctions(Functions, FunctionType::Types::VertexMain, Stages::Vertex, Shader);
					}

					virtual void BuildFragmentShader(const ShaderParser::VariableTypeList& Variables, const ShaderParser::FunctionTypeList& Functions, String& Shader)
					{
						BuildHeader(Shader);

						BuildVariables(Variables, Stages::Fragment, Shader);

						BuildFunctions(Functions, FunctionType::Types::FragmentMain, Stages::Fragment, Shader);
					}

					virtual void BuildHeader(String& Shader)
					{
					}

					virtual void BuildVariables(const ShaderParser::VariableTypeList& Variables, Stages Stage, String& Shader)
					{
						for each (auto var in Variables)
							BuildVariable(var->GetName(), var->GetRegister(), var->GetDataType(), var->GetIsConstant(), Stage == Stages::Vertex, Shader);
					}

					virtual void BuildVariable(String Name, const String& Register, const ShaderDataType& DataType, bool IsConstant, bool IsOutputMode, String& Shader) = 0;

					virtual void BuildFunctions(const ShaderParser::FunctionTypeList& Functions, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildStatementHolder(StatementsHolder* Holder, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildStatement(Statement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
					{
						if (IsAssignableFrom(Statement, OperatorStatement))
						{
							OperatorStatement* stm = ReinterpretCast(OperatorStatement*, Statement);

							BuildOperatorStatement(stm, Type, Stage, Shader);
						}
						else if (IsAssignableFrom(Statement, UnaryOperatorStatement))
						{
							UnaryOperatorStatement* stm = ReinterpretCast(UnaryOperatorStatement*, Statement);

							BuildUnaryOperatorStatement(stm, Type, Stage, Shader);
						}
						else if (IsAssignableFrom(Statement, ConstantStatement))
						{
							ConstantStatement* stm = ReinterpretCast(ConstantStatement*, Statement);

							BuildConstantStatement(stm, Type, Stage, Shader);
						}
						else if (IsAssignableFrom(Statement, FunctionCallStatement))
						{
							FunctionCallStatement* stm = ReinterpretCast(FunctionCallStatement*, Statement);

							BuildFunctionCallStatement(stm, Type, Stage, Shader);
						}
						else if (IsAssignableFrom(Statement, VariableStatement))
						{
							VariableStatement* stm = ReinterpretCast(VariableStatement*, Statement);

							BuildVariableStatement(stm, Type, Stage, Shader);
						}
						else if (IsAssignableFrom(Statement, VariableAccessStatement))
						{
							VariableAccessStatement* stm = ReinterpretCast(VariableAccessStatement*, Statement);

							BuildVariableAccessStatement(stm, Type, Stage, Shader);
						}
						else if (IsAssignableFrom(Statement, ArrayElementAccessStatement))
						{
							ArrayElementAccessStatement* stm = ReinterpretCast(ArrayElementAccessStatement*, Statement);

							BuildArrayElementAccessStatement(stm, Type, Stage, Shader);
						}
						else if (IsAssignableFrom(Statement, MemberAccessStatement))
						{
							MemberAccessStatement* stm = ReinterpretCast(MemberAccessStatement*, Statement);

							BuildMemberAccessStatement(stm, Type, Stage, Shader);
						}
						else if (IsAssignableFrom(Statement, SemicolonStatement))
						{
							SemicolonStatement* stm = ReinterpretCast(SemicolonStatement*, Statement);

							BuildSemicolonStatement(stm, Type, Stage, Shader);
						}
						else if (IsAssignableFrom(Statement, IfStatement))
						{
							IfStatement* stm = ReinterpretCast(IfStatement*, Statement);

							BuildIfStatement(stm, Type, Stage, Shader);
						}
						else if (IsAssignableFrom(Statement, ElseStatement))
						{
							ElseStatement* stm = ReinterpretCast(ElseStatement*, Statement);

							BuildElseStatement(stm, Type, Stage, Shader);
						}
						else if (IsAssignableFrom(Statement, ReturnStatement))
						{
							ReturnStatement* stm = ReinterpretCast(ReturnStatement*, Statement);

							BuildReturnStatement(stm, Type, Stage, Shader);
						}
						else if (IsAssignableFrom(Statement, ArrayStatement))
						{
							ArrayStatement* stm = ReinterpretCast(ArrayStatement*, Statement);

							BuildArrayStatement(stm, Type, Stage, Shader);
						}
						else if (IsAssignableFrom(Statement, DiscardStatement))
						{
							DiscardStatement* stm = ReinterpretCast(DiscardStatement*, Statement);

							BuildDiscardStatement(stm, Type, Stage, Shader);
						}
						else
							Assert(false, "Unsupported Statement");
					}

					virtual void BuildOperatorStatement(OperatorStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildUnaryOperatorStatement(UnaryOperatorStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildConstantStatement(ConstantStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildFunctionCallStatement(FunctionCallStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildVariableStatement(VariableStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildVariableAccessStatement(VariableAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildArrayElementAccessStatement(ArrayElementAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildMemberAccessStatement(MemberAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildSemicolonStatement(SemicolonStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildIfStatement(IfStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildElseStatement(ElseStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildReturnStatement(ReturnStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildArrayStatement(ArrayStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildDiscardStatement(DiscardStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildDataType(const ShaderDataType& Type, String& Shader)
					{
						BuildType(Type.GetType(), Shader);
					}

					virtual void BuildType(ShaderDataType::Types Type, String& Shader) = 0;

					bool ContainsReturnStatement(StatementsHolder* Statement)
					{
						const auto& statements = Statement->GetStatements();
						for each (auto statement in statements)
						{
							if (IsAssignableFrom(statement, ReturnStatement))
								return true;

							if (!IsAssignableFrom(statement, StatementsHolder))
								continue;

							if (ContainsReturnStatement(DynamicCast(StatementsHolder*, statement)))
								return true;
						}

						return false;
					}

					const String& GetVersion(void) const
					{
						return m_Version;
					}

				private:
					String m_Version;
				};

				class OpenGLCompiler : public APICompiler
				{
				private:
					typedef Map<String, String> OutputMap;

				public:
					OpenGLCompiler(const String& Version) :
						APICompiler(Version),
						m_OpenScopeCount(0)
					{
					}

					virtual bool Compile(const ShaderParser::VariableTypeList& Variables, const ShaderParser::FunctionTypeList& Functions, String& VertexShader, String& FragmentShader) override
					{
						m_Outputs.Clear();
						m_OpenScopeCount = 0;

						return APICompiler::Compile(Variables, Functions, VertexShader, FragmentShader);
					}

				private:
					virtual void BuildHeader(String& Shader) override
					{
						String ver = GetVersion();
						ver = ver.Split(' ')[0];
						ver = ver.Replace(".", "");

						Shader += "#version " + ver + " core\n";
					}

					virtual void BuildVariable(String Name, const String& Register, const ShaderDataType& DataType, bool IsConstant, bool IsOutputMode, String& Shader) override
					{
						bool buildOutVarialbe = false;

						if (IsConstant)
							Shader += "uniform ";
						else
						{
							if (m_Outputs.Contains(Name))
							{
								Name = m_Outputs[Name];

								Shader += (IsOutputMode ? "out " : "in ");
							}
							else
							{
								m_Outputs[Name] = Name + "Out";

								if (Register.GetLength() != 0)
								{
									Shader += "layout(location=";
									Shader += StringUtility::ToString<char8>(SubMeshInfo::GetLayoutIndex(GetLayout(Register)));
									Shader += ") in ";
								}

								buildOutVarialbe = true;
							}
						}

						BuildDataType(DataType, Shader);
						Shader += " ";
						Shader += Name;
						Shader += ";";

						ADD_NEW_LINE();

						if (buildOutVarialbe)
							BuildVariable(Name, Register, DataType, false, true, Shader);
					}

					virtual void BuildFunctions(const ShaderParser::FunctionTypeList& Functions, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						m_OpenScopeCount = 0;

						for each (auto fn in Functions)
						{
							FunctionType::Types funcType = fn->GetType();

							if (!(funcType == FunctionType::Types::None || funcType == Type))
								continue;

							if (funcType == FunctionType::Types::FragmentMain)
							{
								for (uint8 i = 0; i < fn->GetReturnDataType().GetElementCount(); ++i)
								{
									Shader += "layout (location=";
									Shader += StringUtility::ToString<char8>(i);
									Shader += ") out ";
									BuildType(ShaderDataType::Types::Float4, Shader);
									Shader += " ";
									Shader += GetFragmentVariableName(i);
									Shader += ";";

									ADD_NEW_LINE();
								}
							}

							if (funcType == Type)
								BuildType(ShaderDataType::Types::Void, Shader);
							else
								BuildDataType(fn->GetReturnDataType(), Shader);

							Shader += " ";

							if (funcType == Type)
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

								BuildDataType(par->GetDataType(), Shader);
								Shader += " ";
								Shader += par->GetName();
							}

							Shader += ")";

							ADD_NEW_LINE();

							Shader += "{";

							ADD_NEW_LINE();

							BuildDataType(ShaderDataType::Types::Bool, Shader);
							Shader += " " + MUST_RETURN_NAME + "=false;";

							ADD_NEW_LINE();

							BuildStatementHolder(fn, funcType, Stage, Shader);

							while (m_OpenScopeCount > 0)
							{
								--m_OpenScopeCount;

								Shader += "}";

								ADD_NEW_LINE();
							}

							Shader += "}";

							ADD_NEW_LINE();
						}
					}

					virtual void BuildStatementHolder(StatementsHolder* Holder, FunctionType::Types Type, Stages Stage, String& Shader)
					{
						// We move one statement forward, because of SemicolonStatement
						bool prevWasReturn = false;

						const auto& statements = Holder->GetStatements();
						for each (auto statement in statements)
						{
							BuildStatement(statement, Type, Stage, Shader);

							if (prevWasReturn)
								break;

							if (IsAssignableFrom(statement, ReturnStatement))
								prevWasReturn = true;
						}
					}

					virtual void BuildOperatorStatement(OperatorStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						OperatorStatement::Operators op = Statement->GetOperator();
						bool isAssignment =
							op == OperatorStatement::Operators::Assignment ||
							op == OperatorStatement::Operators::AdditionAssignment ||
							op == OperatorStatement::Operators::DivisionAssignment ||
							op == OperatorStatement::Operators::MultipicationAssignment ||
							op == OperatorStatement::Operators::SubtractionAssignment;

						bool isRemainder = (op == OperatorStatement::Operators::Remainder);

						if (isRemainder)
							Shader += "mod";

						if (!isAssignment)
							Shader += "(";

						BuildStatement(Statement->GetLeft(), Type, Stage, Shader);

						if (isRemainder)
							Shader += ',';
						else
							Shader += OperatorStatement::GetOperatorSymbol(op);

						BuildStatement(Statement->GetRight(), Type, Stage, Shader);

						if (!isAssignment)
							Shader += ")";
					}

					virtual void BuildUnaryOperatorStatement(UnaryOperatorStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						Shader += "(";

						Shader += UnaryOperatorStatement::GetOperatorSymbol(Statement->GetOperator());

						BuildStatement(Statement->GetStatement(), Type, Stage, Shader);

						Shader += ")";
					}

					virtual void BuildConstantStatement(ConstantStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						if (Statement->GetType() == ConstantStatement::Types::Boolean)
							Shader += StringUtility::ToString<char8>(Statement->GetBool());
						else if (Statement->GetFloat32() == 0 || Statement->GetFloat32() / (int32)Statement->GetFloat32() == 1)
							Shader += StringUtility::ToString<char8>((int32)Statement->GetFloat32());
						else
							Shader += StringUtility::ToString<char8>(Statement->GetFloat32());
					}

					virtual void BuildFunctionCallStatement(FunctionCallStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						auto& funcName = Statement->GetFunctionName();
						ShaderDataType::Types type = ShaderParser::GetDataType(funcName);

						if (type == ShaderDataType::Types::Unknown)
							Shader += funcName;
						else
							BuildDataType(type, Shader);

						Shader += "(";

						bool isFirst = true;
						for each (auto argument in Statement->GetArguments())
						{
							if (!isFirst)
								Shader += ",";
							isFirst = false;

							BuildStatement(argument, Type, Stage, Shader);
						}

						Shader += ")";
					}

					virtual void BuildVariableStatement(VariableStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						BuildDataType(Statement->GetDataType(), Shader);

						Shader += " ";
						Shader += Statement->GetName();

						if (Statement->GetInitialStatement() == nullptr)
							Shader += ';';
						else
						{
							Shader += "=";
							BuildStatement(Statement->GetInitialStatement(), Type, Stage, Shader);
						}
					}

					virtual void BuildVariableAccessStatement(VariableAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						String name = Statement->GetName();

						if (Stage == Stages::Fragment && m_Outputs.Contains(name))
							name = m_Outputs[Statement->GetName()];
						else if (Stage == Stages::Fragment && name == "_FragPosition") // TODO: add to Shader parameter supplier someway
						{
							name = "";

							BuildType(ShaderDataType::Types::Float2, name);

							name += "(gl_FragCoord.x, gl_FragCoord.y)";
						}

						Shader += name;
					}

					virtual void BuildArrayElementAccessStatement(ArrayElementAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						BuildStatement(Statement->GetArrayStatement(), Type, Stage, Shader);

						Shader += "[";

						BuildStatement(Statement->GetElementStatement(), Type, Stage, Shader);

						Shader += "]";
					}

					virtual void BuildMemberAccessStatement(MemberAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						BuildStatement(Statement->GetLeft(), Type, Stage, Shader);

						Shader += ".";

						BuildStatement(Statement->GetRight(), Type, Stage, Shader);
					}

					virtual void BuildSemicolonStatement(SemicolonStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						Shader += ";";

						ADD_NEW_LINE();
					}

					virtual void BuildIfStatement(IfStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						Shader += "if (";

						BuildStatement(Statement->GetCondition(), Type, Stage, Shader);

						Shader += ")";

						ADD_NEW_LINE();

						Shader += "{";

						ADD_NEW_LINE();

						BuildStatementHolder(Statement, Type, Stage, Shader);

						Shader += "}";

						ADD_NEW_LINE();

						if (Statement->GetElse() != nullptr)
							BuildStatement(Statement->GetElse(), Type, Stage, Shader);

						if (ContainsReturnStatement(Statement))
						{
							Shader += "if (!" + MUST_RETURN_NAME + ")";

							ADD_NEW_LINE();

							Shader += "{";

							++m_OpenScopeCount;
						}
					}

					virtual void BuildElseStatement(ElseStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						Shader += "else";

						ADD_NEW_LINE();

						Shader += "{";

						ADD_NEW_LINE();

						BuildStatementHolder(Statement, Type, Stage, Shader);

						Shader += "}";

						ADD_NEW_LINE();
					}

					virtual void BuildReturnStatement(ReturnStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						Shader += MUST_RETURN_NAME + "=true;";

						ADD_NEW_LINE();

						if (Type == FunctionType::Types::VertexMain)
						{
							for each (auto output in m_Outputs)
							{
								Shader += output.GetSecond();
								Shader += "=";
								Shader += output.GetFirst();
								Shader += ";";

								ADD_NEW_LINE();
							}

							Shader += "gl_Position=";
						}
						else if (Type == FunctionType::Types::FragmentMain)
						{
							if (IsAssignableFrom(Statement->GetStatement(), ArrayStatement))
							{
								ArrayStatement* arrStm = ReinterpretCast(ArrayStatement*, Statement->GetStatement());
								auto& stms = arrStm->GetELements();

								for (uint32 i = 0; i < stms.GetSize(); ++i)
								{
									Shader += GetFragmentVariableName(i) + "=";

									BuildStatement(stms[i], Type, Stage, Shader);

									Shader += ";";

									ADD_NEW_LINE();
								}

								return;
							}
							else
								Shader += GetFragmentVariableName(0) + "=";
						}
						else
							Shader += "return ";

						BuildStatement(Statement->GetStatement(), Type, Stage, Shader);
					}

					virtual void BuildArrayStatement(ArrayStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						Assert(false, "Unsupported Location for Statement");
					}

					virtual void BuildDiscardStatement(DiscardStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						Shader += "discard";
					}

					virtual void BuildType(ShaderDataType::Types Type, String& Shader) override
					{
						switch (Type)
						{
						case ShaderDataType::Types::Void:
							Shader += "void";
							break;

						case ShaderDataType::Types::Bool:
							Shader += "bool";
							break;

						case ShaderDataType::Types::Float:
							Shader += "float";
							break;

						case ShaderDataType::Types::Double:
							Shader += "double";
							break;

						case ShaderDataType::Types::Float2:
							Shader += "vec2";
							break;

						case ShaderDataType::Types::Double2:
							Shader += "dvec2";
							break;

						case ShaderDataType::Types::Float3:
							Shader += "vec3";
							break;

						case ShaderDataType::Types::Double3:
							Shader += "dvec3";
							break;

						case ShaderDataType::Types::Float4:
							Shader += "vec4";
							break;

						case ShaderDataType::Types::Double4:
							Shader += "dvec4";
							break;

						case ShaderDataType::Types::Matrix4:
							Shader += "mat4";
							break;

						case ShaderDataType::Types::Texture2D:
							Shader += "sampler2D";
							break;
						}
					}

					static String GetFragmentVariableName(uint8 Index)
					{
						return FRAGMENT_ENTRY_POINT_NAME + "_FragColor" + StringUtility::ToString<char8>(Index);
					}

				private:
					OutputMap m_Outputs;
					int8 m_OpenScopeCount;
				};

				SINGLETON_DEFINITION(Compiler)

					bool Compiler::Compile(DeviceInterface::Type DeviceType, const String& Version, const ShaderInfo* Info, String& VertexShader, String& FragmentShader)
				{
					ShaderParserPreprocess parserPreprocessor(Info->Source);
					ShaderParserPreprocess::Parameters preprocessParameters;
					preprocessParameters.IncludeFunction = [&](const String& Name, String& Source)
					{
						for each (auto listener in m_IListener_List)
						{
							if (listener->FetchShaderSource(Name, Source))
								return true;
						}

						return false;
					};
					preprocessParameters.Defines = Info->Defines;
					parserPreprocessor.Process(preprocessParameters);

					FrameAllocator alloc("Shader Statements Allocator", &RenderingAllocators::ShaderCompilerAllocator, 200 * KiloByte);
					ShaderParser parser(&alloc, preprocessParameters.Result);
					ShaderParser::Parameters parameters;
					parser.Parse(parameters);

					switch (DeviceType)
					{
					case DeviceInterface::Type::OpenGL:
					{
						OpenGLCompiler openGL(Version);
						return openGL.Compile(parameters.Variables, parameters.Functions, VertexShader, FragmentShader);
					}
					}

					return false;
				}
			}
		}
	}
}