// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <Rendering\Private\ShaderCompiler\ShaderParser.h>
#include <Rendering\Private\ShaderCompiler\ShaderParserPreprocess.h>
#include <Rendering\Private\ShaderCompiler\Syntax\StructType.h>
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
#include <Rendering\DeviceInterface.h>
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

				cstr MUST_RETURN_NAME = "_MustReturn";

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
					typedef Map<String, String> OutputMap;

					enum class Stages
					{
						Vertex = 0,
						Fragment
					};

				public:
					APICompiler(const String& Version) :
						m_Version(Version),
						m_OpenScopeCount(0)
					{
					}

					virtual bool Compile(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& VertexShader, String& FragmentShader)
					{
						m_OpenScopeCount = 0;

						BuildVertexShader(Structs, Variables, Functions, VertexShader);

						BuildFragmentShader(Structs, Variables, Functions, FragmentShader);

						return true;
					}

				protected:
					virtual void BuildVertexShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
					{
						BuildHeader(Shader);

						BuildStructs(Structs, Stages::Vertex, Shader);

						BuildVariables(Variables, Stages::Vertex, Shader);

						BuildFunctions(Functions, Stages::Vertex, Shader);
					}

					virtual void BuildFragmentShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
					{
						BuildHeader(Shader);

						BuildStructs(Structs, Stages::Fragment, Shader);

						BuildVariables(Variables, Stages::Fragment, Shader);

						BuildFunctions(Functions, Stages::Fragment, Shader);
					}

					virtual void BuildHeader(String& Shader)
					{
					}

					virtual void BuildStructs(const StructList& Structs, Stages Stage, String& Shader)
					{
						for (auto structType : Structs)
							BuildStruct(structType, Stage, Shader);
					}

					virtual void BuildStruct(StructType* Struct, Stages Stage, String& Shader) = 0;

					virtual void BuildVariables(const VariableList& Variables, Stages Stage, String& Shader)
					{
						for (auto variable : Variables)
							BuildVariable(variable, Stage, Shader);
					}

					virtual void BuildVariable(VariableType* VariableType, Stages Stage, String& Shader) = 0;

					virtual void BuildFunctions(const FunctionList& Functions, Stages Stage, String& Shader)
					{
						for (auto function : Functions)
						{
							m_OpenScopeCount = 0;

							BuildFunction(function, Stage, Shader);

							while (m_OpenScopeCount > 0)
							{
								--m_OpenScopeCount;

								Shader += "}";

								ADD_NEW_LINE();
							}
						}
					}

					virtual void BuildFunction(FunctionType* Function, Stages Stage, String& Shader) = 0;

					virtual void BuildStatementHolder(StatementItemHolder* Holder, FunctionType::Types Type, Stages Stage, String& Shader)
					{
						// We move one statement forward, because of SemicolonStatement
						bool prevWasReturn = false;

						const auto& statements = Holder->GetItems();
						for (auto statement : statements)
						{
							BuildStatement(statement, Type, Stage, Shader);

							if (prevWasReturn)
								break;

							if (IsAssignableFrom(statement, ReturnStatement))
								prevWasReturn = true;
						}
					}

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

					virtual void BuildUnaryOperatorStatement(UnaryOperatorStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
					{
						Shader += "(";

						Shader += UnaryOperatorStatement::GetOperatorSymbol(Statement->GetOperator());

						BuildStatement(Statement->GetStatement(), Type, Stage, Shader);

						Shader += ")";
					}

					virtual void BuildConstantStatement(ConstantStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
					{
						if (Statement->GetType() == ConstantStatement::Types::Boolean)
							Shader += StringUtility::ToString<char8>(Statement->GetBool());
						else if (Statement->GetFloat32() == 0 || Statement->GetFloat32() / (int32)Statement->GetFloat32() == 1)
							Shader += StringUtility::ToString<char8>((int32)Statement->GetFloat32());
						else
							Shader += StringUtility::ToString<char8>(Statement->GetFloat32());
					}

					virtual void BuildFunctionCallStatement(FunctionCallStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
					{
						auto& funcName = Statement->GetFunctionName();
						ShaderDataType::Types type = ShaderParser::GetPrimitiveDataType(funcName);

						if (type == ShaderDataType::Types::Unknown)
							Shader += funcName;
						else
							BuildDataType(type, Shader);

						Shader += "(";

						BuildArguments(Statement->GetArguments(), Type, Stage, Shader);

						Shader += ")";
					}

					virtual void BuildArguments(const StatementItemHolder& Statements, FunctionType::Types Type, Stages Stage, String& Shader)
					{
						bool isFirst = true;
						const auto& arguments = Statements.GetItems();
						for (auto argument : arguments)
						{
							if (!isFirst)
								Shader += ",";
							isFirst = false;

							BuildStatement(argument, Type, Stage, Shader);
						}
					}

					virtual void BuildVariableStatement(VariableStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
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

					virtual void BuildVariableAccessStatement(VariableAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildArrayElementAccessStatement(ArrayElementAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
					{
						BuildStatement(Statement->GetArrayStatement(), Type, Stage, Shader);

						Shader += "[";

						BuildStatement(Statement->GetElementStatement(), Type, Stage, Shader);

						Shader += "]";
					}

					virtual void BuildMemberAccessStatement(MemberAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
					{
						BuildStatement(Statement->GetLeft(), Type, Stage, Shader);

						Shader += ".";

						BuildStatement(Statement->GetRight(), Type, Stage, Shader);
					}

					virtual void BuildSemicolonStatement(SemicolonStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
					{
						Shader += ";";

						ADD_NEW_LINE();
					}

					virtual void BuildIfStatement(IfStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
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

						if (Type != FunctionType::Types::None && ContainsReturnStatement(Statement))
						{
							Shader += String("if (!") + MUST_RETURN_NAME + ")";

							ADD_NEW_LINE();

							Shader += "{";

							++m_OpenScopeCount;
						}
					}

					virtual void BuildElseStatement(ElseStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
					{
						Shader += "else";

						ADD_NEW_LINE();

						Shader += "{";

						ADD_NEW_LINE();

						BuildStatementHolder(Statement, Type, Stage, Shader);

						Shader += "}";

						ADD_NEW_LINE();
					}

					virtual void BuildReturnStatement(ReturnStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildArrayStatement(ArrayStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) = 0;

					virtual void BuildDiscardStatement(DiscardStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
					{
						Shader += "discard";
					}

					virtual void BuildDataType(const ShaderDataType& Type, String& Shader)
					{
						if (Type.GetType() == ShaderDataType::Types::Unknown)
							Shader += Type.GetUserDefined();
						else
							BuildType(Type.GetType(), Shader);
					}

					virtual void BuildType(ShaderDataType::Types Type, String& Shader) = 0;

					bool ContainsReturnStatement(StatementItemHolder* Statement)
					{
						const auto& statements = Statement->GetItems();
						for (auto statement : statements)
						{
							if (IsAssignableFrom(statement, ReturnStatement))
								return true;

							if (!IsAssignableFrom(statement, StatementItemHolder))
								continue;

							if (ContainsReturnStatement(DynamicCast(StatementItemHolder*, statement)))
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
					int8 m_OpenScopeCount;
				};

				class OpenGLCompiler : public APICompiler
				{
				public:
					OpenGLCompiler(const String& Version) :
						APICompiler(Version),
						m_AdditionalLayoutCount(0)
					{
					}

					virtual bool Compile(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& VertexShader, String& FragmentShader) override
					{
						m_Structs.Clear();
						m_AdditionalLayoutCount = 0;
						m_Outputs.Clear();

						m_Structs = Structs;

						return APICompiler::Compile(Structs, Variables, Functions, VertexShader, FragmentShader);
					}

				private:
					virtual void BuildVertexShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader) override
					{
						m_Parameters.Clear();

						APICompiler::BuildVertexShader(Structs, Variables, Functions, Shader);
					}

					virtual void BuildFragmentShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader) override
					{
						m_Parameters.Clear();

						APICompiler::BuildFragmentShader(Structs, Variables, Functions, Shader);
					}

					virtual void BuildHeader(String& Shader) override
					{
						String ver = GetVersion();
						ver = ver.Split(' ')[0];
						ver = ver.Replace(".", "");

						Shader += "#version " + ver + " core\n";
					}

					virtual void BuildStruct(StructType* Struct, Stages Stage, String& Shader) override
					{
						auto variables = Struct->GetItems();

						for (auto variable : variables)
						{
							if (variable->GetRegister().GetLength() == 0)
								continue;

							BuildVariable(variable, Stage, Shader);
						}

						variables.RemoveIf([](auto item) { return item->GetRegister().GetLength() != 0; });

						if (variables.GetSize() == 0)
							return;

						Shader += "struct " + Struct->GetName();
						ADD_NEW_LINE();
						Shader += "{";
						ADD_NEW_LINE();

						for (auto variable : variables)
							BuildVariable(variable, Stage, Shader);

						Shader += "};";
						ADD_NEW_LINE();
					}

					virtual void BuildVariable(VariableType* Variable, Stages Stage, String& Shader) override
					{
						BuildVariableInternal(Variable->GetName(), Variable->GetRegister(), Variable->GetDataType(), false, Shader);
					}

					virtual void BuildFunction(FunctionType* Function, Stages Stage, String& Shader) override
					{
						FunctionType::Types funcType = Function->GetType();

						if (Function->IsEntrypoint())
						{
							if (funcType == FunctionType::Types::VertexMain && Stage != Stages::Vertex)
								return;

							if (funcType == FunctionType::Types::FragmentMain && Stage != Stages::Fragment)
								return;
						}

						if (funcType == FunctionType::Types::FragmentMain)
						{
							for (uint8 i = 0; i < Function->GetReturnDataType().GetElementCount(); ++i)
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

						if (Function->IsEntrypoint())
							BuildType(ShaderDataType::Types::Void, Shader);
						else
							BuildDataType(Function->GetReturnDataType(), Shader);

						Shader += " ";

						if (Function->IsEntrypoint())
							Shader += Compiler::ENTRY_POINT_NAME;
						else
							Shader += Function->GetName();

						Shader += "(";

						if (Function->IsEntrypoint())
							m_Parameters.AddRange(Function->GetParameters());
						else
						{
							bool isFirst = true;
							for (auto par : Function->GetParameters())
							{
								if (!isFirst)
									Shader += ",";
								isFirst = false;

								BuildDataType(par->GetDataType(), Shader);
								Shader += " ";
								Shader += par->GetName();
							}
						}

						Shader += ")";

						ADD_NEW_LINE();

						Shader += "{";

						ADD_NEW_LINE();

						BuildDataType(ShaderDataType::Types::Bool, Shader);
						Shader += String(" ") + MUST_RETURN_NAME + "=false;";

						ADD_NEW_LINE();

						BuildStatementHolder(Function, funcType, Stage, Shader);

						Shader += "}";

						ADD_NEW_LINE();
					}

					virtual void BuildOperatorStatement(OperatorStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						OperatorStatement::Operators op = Statement->GetOperator();

						if (op == OperatorStatement::Operators::Remainder)
						{
							Shader += "mod(";

							BuildStatement(Statement->GetLeft(), Type, Stage, Shader);

							Shader += ',';

							BuildStatement(Statement->GetRight(), Type, Stage, Shader);

							Shader += ")";

							return;
						}

						bool isAssignment =
							op == OperatorStatement::Operators::Assignment ||
							op == OperatorStatement::Operators::AdditionAssignment ||
							op == OperatorStatement::Operators::DivisionAssignment ||
							op == OperatorStatement::Operators::MultipicationAssignment ||
							op == OperatorStatement::Operators::SubtractionAssignment;

						if (!isAssignment)
							Shader += "(";

						BuildStatement(Statement->GetLeft(), Type, Stage, Shader);

						Shader += OperatorStatement::GetOperatorSymbol(op);

						BuildStatement(Statement->GetRight(), Type, Stage, Shader);

						if (!isAssignment)
							Shader += ")";
					}

					virtual void BuildVariableAccessStatement(VariableAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						String name = Statement->GetName();

						if (Stage == Stages::Fragment)
						{
							if (m_Outputs.Contains(name))
								name = m_Outputs[name];
							else if (name == "_FragPosition")
							{
								name = "";

								BuildType(ShaderDataType::Types::Float2, name);

								name += "(gl_FragCoord.x, gl_FragCoord.y)";
							}
						}

						Shader += name;
					}

					virtual void BuildMemberAccessStatement(MemberAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						String temp;
						BuildStatement(Statement->GetLeft(), Type, Stage, temp);

						if (m_Parameters.Contains([&temp](auto item) {return item->GetName() == temp; }))
						{
							BuildStatement(Statement->GetRight(), Type, Stage, Shader);

							return;
						}

						APICompiler::BuildMemberAccessStatement(Statement, Type, Stage, Shader);
					}

					virtual void BuildReturnStatement(ReturnStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						Shader += String(MUST_RETURN_NAME) + "=true;";

						ADD_NEW_LINE();

						if (Type == FunctionType::Types::VertexMain)
						{
							for (auto output : m_Outputs)
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
								auto& stms = arrStm->GetELements().GetItems();

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

					void BuildVariableInternal(String Name, const String& Register, const ShaderDataType& DataType, bool IsOutputMode, String& Shader)
					{
						bool buildOutVarialbe = false;

						bool doesBoundToRegister = (Register.GetLength() != 0);

						if (doesBoundToRegister)
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

						int32 index = m_Structs.Find([&DataType](auto item) { return item->GetName() == DataType.GetUserDefined(); });
						if (index != -1)
						{
							StructType* structType = m_Structs[index];

							Shader += "layout(location=";
							Shader += StringUtility::ToString<char8>(SubMeshInfo::GetLayoutIndex(SubMesh::VertexLayouts::UV) + ++m_AdditionalLayoutCount);
							Shader += ") uniform ";
						}

						BuildDataType(DataType, Shader);
						Shader += " ";
						Shader += Name;
						Shader += ";";

						ADD_NEW_LINE();

						if (buildOutVarialbe)
							BuildVariableInternal(Name, Register, DataType, true, Shader);
					}

					static String GetFragmentVariableName(uint8 Index)
					{
						return String(FRAGMENT_ENTRY_POINT_NAME) + "_FragColor" + StringUtility::ToString<char8>(Index);
					}

				private:
					StructList m_Structs;
					uint8 m_AdditionalLayoutCount;
					OutputMap m_Outputs;
					ParameterList m_Parameters;
				};

				class DirectXCompiler : public APICompiler
				{
				private:
					typedef Map<String, String> OutputMap;

				public:
					DirectXCompiler(const String& Version) :
						APICompiler(Version),
						m_Add_SV_Position(false)
					{
					}

					virtual bool Compile(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& VertexShader, String& FragmentShader) override
					{
						m_Outputs.Clear();

						bool result = APICompiler::Compile(Structs, Variables, Functions, VertexShader, FragmentShader);

						if (result)
						{
							if (m_Add_SV_Position)
								FragmentShader = "float4 dx_frag_coord:SV_Position;" + FragmentShader;
						}

						return result;
					}

				private:
					virtual void BuildStruct(StructType* Struct, Stages Stage, String& Shader) override
					{
					}

					virtual void BuildVariable(VariableType* Variable, Stages Stage, String& Shader) override
					{
						BuildVariableInternal(Variable->GetName(), Variable->GetRegister(), Variable->GetDataType(), false, Shader);
					}

					virtual void BuildFunction(FunctionType* Function, Stages Stage, String& Shader) override
					{
						FunctionType::Types funcType = Function->GetType();

						if (Function->IsEntrypoint())
						{
							if (funcType == FunctionType::Types::VertexMain && Stage != Stages::Vertex)
								return;

							if (funcType == FunctionType::Types::FragmentMain && Stage != Stages::Fragment)
								return;
						}

						if (Function->IsEntrypoint())
							BuildType(ShaderDataType::Types::Float4, Shader);
						else
							BuildDataType(Function->GetReturnDataType(), Shader);

						Shader += " ";

						if (Function->IsEntrypoint())
							Shader += Compiler::ENTRY_POINT_NAME;
						else
							Shader += Function->GetName();

						Shader += "(";

						bool isFirst = true;
						for (auto par : Function->GetParameters())
						{
							if (!isFirst)
								Shader += ",";
							isFirst = false;

							BuildDataType(par->GetDataType(), Shader);
							Shader += " ";
							Shader += par->GetName();
						}

						Shader += ")";

						if (funcType == FunctionType::Types::VertexMain)
						{
							Shader += ":SV_POSITION";
						}
						else if (funcType == FunctionType::Types::FragmentMain)
						{
							for (uint8 i = 0; i < Function->GetReturnDataType().GetElementCount(); ++i)
								Shader += ":SV_TARGET";
						}

						ADD_NEW_LINE();

						Shader += "{";

						ADD_NEW_LINE();

						BuildDataType(ShaderDataType::Types::Bool, Shader);
						Shader += String(" ") + MUST_RETURN_NAME + "=false;";

						ADD_NEW_LINE();

						BuildStatementHolder(Function, funcType, Stage, Shader);

						Shader += "}";

						ADD_NEW_LINE();
					}

					virtual void BuildOperatorStatement(OperatorStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						OperatorStatement::Operators op = Statement->GetOperator();

						if (op == OperatorStatement::Operators::Multipication)
						{
							if (Statement->GetLeft()->EvaluateResultType() == ShaderDataType::Types::Matrix4)
							{
								Shader += "mul(";

								BuildStatement(Statement->GetLeft(), Type, Stage, Shader);

								Shader += ',';

								BuildStatement(Statement->GetRight(), Type, Stage, Shader);

								Shader += ")";

								return;
							}
						}
						else if (op == OperatorStatement::Operators::Remainder)
						{
							Shader += "fmod(";

							BuildStatement(Statement->GetLeft(), Type, Stage, Shader);

							Shader += ',';

							BuildStatement(Statement->GetRight(), Type, Stage, Shader);

							Shader += ")";

							return;
						}

						bool isAssignment =
							op == OperatorStatement::Operators::Assignment ||
							op == OperatorStatement::Operators::AdditionAssignment ||
							op == OperatorStatement::Operators::DivisionAssignment ||
							op == OperatorStatement::Operators::MultipicationAssignment ||
							op == OperatorStatement::Operators::SubtractionAssignment;

						if (!isAssignment)
							Shader += "(";

						BuildStatement(Statement->GetLeft(), Type, Stage, Shader);

						Shader += OperatorStatement::GetOperatorSymbol(op);

						BuildStatement(Statement->GetRight(), Type, Stage, Shader);

						if (!isAssignment)
							Shader += ")";
					}

					virtual void BuildFunctionCallStatement(FunctionCallStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						if (Statement->GetFunctionName() == "texture")
						{
							const auto& items = Statement->GetArguments().GetItems();

							BuildStatement(items[0], Type, Stage, Shader);

							Shader += "[";

							BuildStatement(items[1], Type, Stage, Shader);

							Shader += "]";

							//Shader += "tex2D(";

							//BuildArguments(Statement->GetArguments(), Type, Stage, Shader);

							//Shader += ")";

							return;
						}

						APICompiler::BuildFunctionCallStatement(Statement, Type, Stage, Shader);
					}

					virtual void BuildVariableAccessStatement(VariableAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						String name = Statement->GetName();

						if (Stage == Stages::Fragment && m_Outputs.Contains(name))
							name = m_Outputs[Statement->GetName()];
						else if (Stage == Stages::Fragment && name == "_FragPosition")
						{
							name = "";

							BuildType(ShaderDataType::Types::Float2, name);

							name += "(dx_frag_coord.x, dx_frag_coord.y)";

							m_Add_SV_Position = true;
						}

						Shader += name;
					}

					virtual void BuildReturnStatement(ReturnStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						Shader += String(MUST_RETURN_NAME) + "=true;";

						ADD_NEW_LINE();

						Shader += "return ";

						BuildStatement(Statement->GetStatement(), Type, Stage, Shader);
					}

					virtual void BuildArrayStatement(ArrayStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						Assert(false, "Unsupported Location for Statement");
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
							Shader += "float2";
							break;

						case ShaderDataType::Types::Double2:
							Shader += "double2";
							break;

						case ShaderDataType::Types::Float3:
							Shader += "float3";
							break;

						case ShaderDataType::Types::Double3:
							Shader += "double3";
							break;

						case ShaderDataType::Types::Float4:
							Shader += "float4";
							break;

						case ShaderDataType::Types::Double4:
							Shader += "double4";
							break;

						case ShaderDataType::Types::Matrix4:
							Shader += "float4x4";
							break;

						case ShaderDataType::Types::Texture2D:
							Shader += "Texture2D<float4>";
							//Shader += "sampler2D";
							break;
						}
					}

					void BuildVariableInternal(String Name, const String& Register, const ShaderDataType& DataType, bool IsOutputMode, String& Shader)
					{
						bool buildOutVarialbe = false;

						bool isUniform = (Register.GetLength() == 0);

						if (!isUniform)
							Shader += "uniform ";
						else
						{
							if (m_Outputs.Contains(Name))
								Name = m_Outputs[Name];
							else
							{
								m_Outputs[Name] = Name + "Out";

								buildOutVarialbe = true;
							}
						}

						BuildDataType(DataType, Shader);
						Shader += " ";
						Shader += Name;

						if (!isUniform)
						{
							Shader += ":";
							Shader += Register;
						}

						Shader += ";";

						ADD_NEW_LINE();

						//if (buildOutVarialbe)
							//BuildVariable(Name, Register, DataType, false, true, Shader);
					}

					static String GetFragmentVariableName(uint8 Index)
					{
						return String(FRAGMENT_ENTRY_POINT_NAME) + "_FragColor" + StringUtility::ToString<char8>(Index);
					}

				private:
					bool m_Add_SV_Position;
					OutputMap m_Outputs;
				};

				SINGLETON_DEFINITION(Compiler);

				cstr Compiler::ENTRY_POINT_NAME = "main";

				bool Compiler::Compile(DeviceTypes DeviceType, const String& Version, const ShaderInfo* Info, String& VertexShader, String& FragmentShader, ErrorFunction OnError)
				{
					ShaderParserPreprocess parserPreprocessor(Info->Source);
					ShaderParserPreprocess::Parameters preprocessParameters;
					preprocessParameters.IncludeFunction = [&](const String& Name, String& Source)
					{
						for (auto listener : m_IListener_List)
						{
							if (listener->FetchShaderSource(Name, Source))
								return true;
						}

						return false;
					};
					preprocessParameters.Defines = Info->Defines;
					if (!parserPreprocessor.Process(preprocessParameters))
						return false;

					FrameAllocator alloc("Shader Statements Allocator", RenderingAllocators::ShaderCompilerAllocator);
					ShaderParser parser(&alloc, preprocessParameters.Result, OnError);
					ShaderParser::Parameters parameters;
					if (!parser.Parse(parameters))
						return false;

					bool result = false;

					switch (DeviceType)
					{
					case DeviceTypes::OpenGL:
					{
						OpenGLCompiler openGL(Version);
						result = openGL.Compile(parameters.Structs, parameters.Variables, parameters.Functions, VertexShader, FragmentShader);
					} break;

					case DeviceTypes::DirectX12:
					{
						DirectXCompiler directX(Version);
						result = directX.Compile(parameters.Structs, parameters.Variables, parameters.Functions, VertexShader, FragmentShader);
					} break;
					}

					for (auto structType : parameters.Structs)
						Destruct(structType);

					for (auto variableType : parameters.Variables)
						Destruct(variableType);

					for (auto function : parameters.Functions)
						Destruct(function);

					return result;
				}
			}
		}
	}
}