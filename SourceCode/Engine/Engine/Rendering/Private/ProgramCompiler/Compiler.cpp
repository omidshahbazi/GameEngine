// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ProgramCompiler\Compiler.h>
#include <Rendering\Private\ProgramCompiler\ProgramParser.h>
#include <Rendering\Private\ProgramCompiler\ProgramParserPreprocess.h>
#include <Rendering\Private\ProgramCompiler\Syntax\StructType.h>
#include <Rendering\Private\ProgramCompiler\Syntax\FunctionType.h>
#include <Rendering\Private\ProgramCompiler\Syntax\IfStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\ElseStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\SwitchStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\CaseStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\ForStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\DoStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\WhileStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\ContinueStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\BreakStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\ReturnStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\DiscardStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\OperatorStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\UnaryOperatorStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\VariableStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\FunctionCallStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\ConstantStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\VariableAccessStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\ArrayElementAccessStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\MemberAccessStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\SemicolonStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\ArrayStatement.h>
#include <Rendering\GPUAlignedType.h>
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
			namespace ProgramCompiler
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

				uint16 GetAlignedSize(uint16 Size, uint8 Alignment)
				{
					if (Size % Alignment != 0)
						Size = ((Size / Alignment) + 1) * Alignment;

					return Size;
				}

				void GetAlignedOffset(ProgramDataTypes DataType, uint16& Offset, uint8& Size)
				{
					uint8 alignment = 0;
					switch (DataType)
					{
					case ProgramDataTypes::Bool:
						Size = GPUAlignedBool::Size;
						alignment = GPUAlignedBool::Alignment;
						break;

					case ProgramDataTypes::Float:
						Size = GPUAlignedFloat32::Size;
						alignment = GPUAlignedFloat32::Alignment;
						break;

					case ProgramDataTypes::Double:
						Size = GPUAlignedFloat64::Size;
						alignment = GPUAlignedFloat64::Alignment;
						break;

					case ProgramDataTypes::Float2:
						Size = GPUAlignedVector2F::Size;
						alignment = GPUAlignedVector2F::Alignment;
						break;

					case ProgramDataTypes::Double2:
						Size = GPUAlignedVector2D::Size;
						alignment = GPUAlignedVector2D::Alignment;
						break;

					case ProgramDataTypes::Float3:
						Size = GPUAlignedVector3F::Size;
						alignment = GPUAlignedVector3F::Alignment;
						break;

					case ProgramDataTypes::Double3:
						Size = GPUAlignedVector3D::Size;
						alignment = GPUAlignedVector3D::Alignment;
						break;

					case ProgramDataTypes::Float4:
						Size = GPUAlignedVector4F::Size;
						alignment = GPUAlignedVector4F::Alignment;
						break;

					case ProgramDataTypes::Double4:
						Size = GPUAlignedVector4D::Size;
						alignment = GPUAlignedVector4D::Alignment;
						break;

					case ProgramDataTypes::Matrix4:
						Size = GPUAlignedMatrix4F::Size;
						alignment = GPUAlignedMatrix4F::Alignment;
						break;
					}

					Offset = GetAlignedSize(Offset, alignment);
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
					APICompiler(AllocatorBase* Allocator) :
						m_Allocator(Allocator),
						m_OpenScopeCount(0)
					{
					}

					virtual bool Compile(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, Compiler::OutputInfo& Output)
					{
						m_OpenScopeCount = 0;

						BuildVertexShader(Structs, Variables, Functions, Output.VertexShader);

						BuildFragmentShader(Structs, Variables, Functions, Output.FragmentShader);

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
						ProgramDataTypes type = ProgramParser::GetPrimitiveDataType(funcName);

						if (type == ProgramDataTypes::Unknown)
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

					virtual void BuildDataType(const DataType& Type, String& Shader)
					{
						if (Type.IsBuiltIn())
							BuildType(Type.GetType(), Shader);
						else
							Shader += Type.GetUserDefined();
					}

					virtual void BuildType(ProgramDataTypes Type, String& Shader) = 0;

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

					AllocatorBase* GetAllocator(void) const
					{
						return m_Allocator;
					}

				private:
					AllocatorBase* m_Allocator;
					int8 m_OpenScopeCount;
				};

				class OpenGLCompiler : public APICompiler
				{
				public:
					OpenGLCompiler(AllocatorBase* Allocator) :
						APICompiler(Allocator),
						m_AdditionalLayoutCount(0),
						m_UniformBlockBindingCount(0),
						m_TextureBlockBindingCount(0)
					{
					}

					virtual bool Compile(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, Compiler::OutputInfo& Output) override
					{
						m_Structs = Structs;
						//m_Variables = Variables;
						m_Outputs.Clear();

						return APICompiler::Compile(Structs, Variables, Functions, Output);
					}

				private:
					virtual void BuildVertexShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader) override
					{
						m_Parameters.Clear();
						m_AdditionalLayoutCount = SubMeshInfo::GetExtraIndex();
						m_UniformBlockBindingCount = 0;
						m_TextureBlockBindingCount = 0;

						APICompiler::BuildVertexShader(Structs, Variables, Functions, Shader);
					}

					virtual void BuildFragmentShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader) override
					{
						m_Parameters.Clear();
						m_AdditionalLayoutCount = SubMeshInfo::GetExtraIndex();
						m_UniformBlockBindingCount = 0;
						m_TextureBlockBindingCount = 0;

						APICompiler::BuildFragmentShader(Structs, Variables, Functions, Shader);
					}

					virtual void BuildHeader(String& Shader) override
					{
						Shader += "#version 460 core\n";
					}

					virtual void BuildStruct(StructType* Struct, Stages Stage, String& Shader) override
					{
						auto variables = Struct->GetItems();

						if (variables.ContainsIf([](auto item) { return item->GetRegister().GetLength() != 0; }))
						{
							if (variables.ContainsIf([](auto item) { return item->GetRegister().GetLength() == 0; }))
							{
								//->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>REJECT
								return;
							}
						}
						else
						{
							return;
						}

						for (auto variable : variables)
						{
							BuildVariable(variable, Stage, Shader);
						}
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
								BuildType(ProgramDataTypes::Float4, Shader);
								Shader += " ";
								Shader += GetFragmentVariableName(i);
								Shader += ";";

								ADD_NEW_LINE();
							}
						}

						if (Function->IsEntrypoint())
							BuildType(ProgramDataTypes::Void, Shader);
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

						BuildDataType(ProgramDataTypes::Bool, Shader);
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

								BuildType(ProgramDataTypes::Float2, name);

								name += "(gl_FragCoord.x, gl_FragCoord.y)";
							}
						}

						Shader += name;
					}

					virtual void BuildMemberAccessStatement(MemberAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						String temp;
						BuildStatement(Statement->GetLeft(), Type, Stage, temp);

						if (m_Parameters.ContainsIf([&temp](auto item) { return item->GetName() == temp; }))
						{
							BuildStatement(Statement->GetRight(), Type, Stage, Shader);

							return;
						}
						//else if (m_Variables.Contains([&temp](auto item) { return item->GetName() == temp; }))
						//{
						//	BuildStatement(Statement->GetRight(), Type, Stage, Shader);

						//	return;
						//}

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

					virtual void BuildType(ProgramDataTypes Type, String& Shader) override
					{
						switch (Type)
						{
						case ProgramDataTypes::Void:
							Shader += "void";
							break;

						case ProgramDataTypes::Bool:
							Shader += "bool";
							break;

						case ProgramDataTypes::Float:
							Shader += "float";
							break;

						case ProgramDataTypes::Double:
							Shader += "double";
							break;

						case ProgramDataTypes::Float2:
							Shader += "vec2";
							break;

						case ProgramDataTypes::Double2:
							Shader += "dvec2";
							break;

						case ProgramDataTypes::Float3:
							Shader += "vec3";
							break;

						case ProgramDataTypes::Double3:
							Shader += "dvec3";
							break;

						case ProgramDataTypes::Float4:
							Shader += "vec4";
							break;

						case ProgramDataTypes::Double4:
							Shader += "dvec4";
							break;

						case ProgramDataTypes::Matrix4:
							Shader += "mat4";
							break;

						case ProgramDataTypes::Texture2D:
							Shader += "sampler2D";
							break;
						}
					}

					void BuildVariableInternal(String Name, const String& Register, const DataType& DataType, bool IsOutputMode, String& Shader)
					{
						bool buildOutVarialbe = false;

						bool doesBoundToRegister = (Register.GetLength() != 0);

						if (doesBoundToRegister)
						{
							int8 location = 0;

							if (m_Outputs.Contains(Name))
							{
								Name = m_Outputs[Name];

								location = m_AdditionalLayoutCount++;
							}
							else
							{
								m_Outputs[Name] = Name + "Out";

								location = SubMeshInfo::GetLayoutIndex(GetLayout(Register));

								buildOutVarialbe = true;
							}

							Shader += "layout(location=";
							Shader += StringUtility::ToString<char8>(location);
							Shader += ")";
							Shader += (IsOutputMode ? "out " : "in ");
						}

						int32 index = m_Structs.FindIf([&DataType](auto item) { return item->GetName() == DataType.GetUserDefined(); });
						if (index != -1)
						{
							BuildUniformBlock(m_Structs[index], Name, Stages::Vertex, Shader);

							return;
						}

						for (auto dataType : ALLOWED_CONTEXT_FREE_DATA_TYPES)
						{
							if (dataType != DataType.GetType())
								continue;

							if (dataType == ProgramDataTypes::Texture2D)
							{
								Shader += "layout(location=";
								Shader += StringUtility::ToString<char8>(m_TextureBlockBindingCount++);
								Shader += ")";
							}

							Shader += "uniform ";
							break;
						}

						BuildDataType(DataType, Shader);
						Shader += " ";
						Shader += Name;
						Shader += ";";

						ADD_NEW_LINE();

						if (buildOutVarialbe)
							BuildVariableInternal(Name, Register, DataType, true, Shader);
					}

					void BuildUniformBlock(StructType* Struct, const String& Name, Stages Stage, String& Shader)
					{
						auto variables = Struct->GetItems();

						if (variables.ContainsIf([](auto item) { return item->GetRegister().GetLength() != 0; }))
						{
							//->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>REJECT
							return;
						}

						Shader += "layout(std140, binding=";
						Shader += StringUtility::ToString<char8>(m_UniformBlockBindingCount++);
						Shader += ") uniform " + Struct->GetName();
						Shader += "_";
						Shader += Name;
						ADD_NEW_LINE();
						Shader += "{";
						ADD_NEW_LINE();

						uint16 offset = 0;
						for (auto variable : variables)
						{
							ProgramDataTypes dataType = variable->GetDataType().GetType();

							uint8 size = 0;
							GetAlignedOffset(dataType, offset, size);

							Shader += "layout(offset=";
							Shader += StringUtility::ToString<char8>(offset);

							Shader += ") ";

							offset += size;

							BuildVariable(variable, Stage, Shader);
						}

						Shader += "}";

						Shader += Name;

						Shader += ";";

						ADD_NEW_LINE();
					}

					static String GetFragmentVariableName(uint8 Index)
					{
						return String(FRAGMENT_ENTRY_POINT_NAME) + "_FragColor" + StringUtility::ToString<char8>(Index);
					}

				private:
					StructList m_Structs;
					//VariableList m_Variables;
					uint8 m_AdditionalLayoutCount;
					uint8 m_UniformBlockBindingCount;
					uint8 m_TextureBlockBindingCount;
					OutputMap m_Outputs;
					ParameterList m_Parameters;
				};

				class DirectXCompiler : public APICompiler
				{
				private:
					typedef Map<String, String> OutputMap;

				public:
					DirectXCompiler(AllocatorBase* Allocator) :
						APICompiler(Allocator),
						m_Add_SV_Position(false)
					{
					}

					virtual bool Compile(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, Compiler::OutputInfo& Output) override
					{
						m_Outputs.Clear();

						bool result = APICompiler::Compile(Structs, Variables, Functions, Output);

						if (result)
						{
							if (m_Add_SV_Position)
								Output.FragmentShader = "float4 dx_frag_coord:SV_Position;" + Output.FragmentShader;
						}

						return result;
					}

				private:
					virtual void BuildStruct(StructType* Struct, Stages Stage, String& Shader) override
					{
						Shader += "struct ";
						Shader += Struct->GetName();
						ADD_NEW_LINE();
						Shader += "{";
						ADD_NEW_LINE();

						auto variables = Struct->GetItems();

						for (auto variable : variables)
						{
							BuildVariable(variable, Stage, Shader);
						}

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

						if (Function->IsEntrypoint())
							BuildType(ProgramDataTypes::Float4, Shader);
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

						BuildDataType(ProgramDataTypes::Bool, Shader);
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
							if (Statement->GetLeft()->EvaluateResultType() == ProgramDataTypes::Matrix4)
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

							BuildType(ProgramDataTypes::Float2, name);

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
						//Assert(false, "Unsupported Location for Statement");
					}

					virtual void BuildType(ProgramDataTypes Type, String& Shader) override
					{
						switch (Type)
						{
						case ProgramDataTypes::Void:
							Shader += "void";
							break;

						case ProgramDataTypes::Bool:
							Shader += "bool";
							break;

						case ProgramDataTypes::Float:
							Shader += "float";
							break;

						case ProgramDataTypes::Double:
							Shader += "double";
							break;

						case ProgramDataTypes::Float2:
							Shader += "float2";
							break;

						case ProgramDataTypes::Double2:
							Shader += "double2";
							break;

						case ProgramDataTypes::Float3:
							Shader += "float3";
							break;

						case ProgramDataTypes::Double3:
							Shader += "double3";
							break;

						case ProgramDataTypes::Float4:
							Shader += "float4";
							break;

						case ProgramDataTypes::Double4:
							Shader += "double4";
							break;

						case ProgramDataTypes::Matrix4:
							Shader += "float4x4";
							break;

						case ProgramDataTypes::Texture2D:
							Shader += "Texture2D<float4>";
							//Shader += "sampler2D";
							break;
						}
					}

					void BuildVariableInternal(String Name, const String& Register, const DataType& DataType, bool IsOutputMode, String& Shader)
					{
						bool buildOutVarialbe = false;

						if (m_Outputs.Contains(Name))
							Name = m_Outputs[Name];
						else
						{
							m_Outputs[Name] = Name + "Out";

							buildOutVarialbe = true;
						}

						BuildDataType(DataType, Shader);
						Shader += " ";
						Shader += Name;

						if (Register.GetLength() != 0)
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

				bool Compiler::Compile(const ProgramInfo* Info, DeviceTypes DeviceType, OutputInfo& Output, ErrorFunction OnError)
				{
					return Compile(Info, &DeviceType, 1, &Output, OnError);
				}

				bool Compiler::Compile(const ProgramInfo* Info, const DeviceTypes* DeviceTypes, uint8 DeviceTypeCount, OutputInfo* Outputs, ErrorFunction OnError)
				{
					if (Info->Source.GetLength() == 0)
						return false;

					ProgramParserPreprocess parserPreprocessor(Info->Source);
					ProgramParserPreprocess::Parameters preprocessParameters;
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

					FrameAllocator alloc("Program Statements Allocator", RenderingAllocators::ProgramCompilerAllocator);
					ProgramParser parser(&alloc, preprocessParameters.Result, OnError);
					ProgramParser::Parameters parameters;
					if (!parser.Parse(parameters))
						return false;

					bool result = false;

					for (uint8 i = 0; i < DeviceTypeCount; ++i)
					{
						OutputInfo& output = Outputs[i];

						switch (DeviceTypes[i])
						{
						case DeviceTypes::OpenGL:
						case DeviceTypes::Vulkan:
						{
							OpenGLCompiler openGL(&alloc);
							result = openGL.Compile(parameters.Structs, parameters.Variables, parameters.Functions, output);
						} break;

						case DeviceTypes::DirectX12:
						{
							DirectXCompiler directX(&alloc);
							result = directX.Compile(parameters.Structs, parameters.Variables, parameters.Functions, output);
						} break;
						}

						if (result)
						{
							for (auto& structType : parameters.Structs)
							{
								auto variables = structType->GetItems();
								variables.RemoveIf([](auto item) { return item->GetRegister().GetLength() != 0; });

								if (variables.GetSize() == 0)
									continue;

								output.MetaInfo.Structs.Add({});
								StructMetaInfo& structMeta = output.MetaInfo.Structs[output.MetaInfo.Structs.GetSize() - 1];

								structMeta.Name = structType->GetName();

								for (auto& variableType : variables)
								{
									ProgramDataTypes dataType = variableType->GetDataType().GetType();

									structMeta.Variables.Add({ dataType, variableType->GetName() });

									uint8 size = 0;
									GetAlignedOffset(dataType, structMeta.Size, size);

									structMeta.Size += size;
								}

								structMeta.Size = GetAlignedSize(structMeta.Size, 16);
							}

							for (auto& variableType : parameters.Variables)
							{
								output.MetaInfo.Variables.Add({});
								VariableMetaInfo& variableMeta = output.MetaInfo.Variables[output.MetaInfo.Variables.GetSize() - 1];

								variableMeta.Name = variableType->GetName();
								variableMeta.DataType = variableType->GetDataType().GetType();
								variableMeta.UserDefinedType = variableType->GetDataType().GetUserDefined();
							}
						}
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