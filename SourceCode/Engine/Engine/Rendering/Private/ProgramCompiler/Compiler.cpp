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

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				using namespace Syntax;

#ifdef DEBUG_MODE
#define ADD_NEW_LINE() Shader += "\n"
#else
#define ADD_NEW_LINE()
#endif

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

				uint16 GetStructSize(const StructType* Struct)
				{
					uint16 totalSize = 0;

					for (auto& variableType : Struct->GetItems())
					{
						ProgramDataTypes dataType = variableType->GetDataType().GetType();

						uint8 size = 0;
						GetAlignedOffset(dataType, totalSize, size);

						totalSize += size;
					}

					return GetAlignedSize(totalSize, 16);
				}

				class APICompiler
				{
				protected:
					typedef Map<String, String> OutputMap;
					typedef Map<String, DataType> VariableTypeMap;

					enum class Stages
					{
						Vertex = 0,
						Tessellation,
						Geometry,
						Fragment,
						Compute
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

						m_Structs = Structs;

						auto ContainsType = [&Functions](FunctionType::Types Type)
						{
							return Functions.ContainsIf([Type](auto functionType) {return functionType->GetType() == Type; });
						};

						if (ContainsType(FunctionType::Types::VertexMain))
							BuildVertexShader(Structs, Variables, Functions, Output.VertexShader);

						if (ContainsType(FunctionType::Types::TessellationMain))
							BuildTessellationShader(Structs, Variables, Functions, Output.TessellationShader);

						if (ContainsType(FunctionType::Types::GeometryMain))
							BuildGeometryShader(Structs, Variables, Functions, Output.GeometryShader);

						if (ContainsType(FunctionType::Types::FragmentMain))
							BuildFragmentShader(Structs, Variables, Functions, Output.FragmentShader);

						if (ContainsType(FunctionType::Types::ComputeMain))
							BuildComputeShader(Structs, Variables, Functions, Output.ComputeShader);

						return true;
					}

				protected:
					virtual void BuildStageShader(Stages Stage, const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
					{
						ResetPerStageValues(Stage);

						BuildHeader(Shader);

						BuildStructs(Structs, Stage, Shader);

						BuildVariables(Variables, Stage, Shader);

						BuildFunctions(Functions, Stage, Shader);
					}

					virtual void BuildVertexShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
					{
						BuildStageShader(Stages::Vertex, Structs, Variables, Functions, Shader);
					}

					virtual void BuildTessellationShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
					{
						BuildStageShader(Stages::Tessellation, Structs, Variables, Functions, Shader);
					}

					virtual void BuildGeometryShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
					{
						BuildStageShader(Stages::Geometry, Structs, Variables, Functions, Shader);
					}

					virtual void BuildFragmentShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
					{
						BuildStageShader(Stages::Fragment, Structs, Variables, Functions, Shader);
					}

					virtual void BuildComputeShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
					{
						BuildStageShader(Stages::Compute, Structs, Variables, Functions, Shader);
					}

					virtual void ResetPerStageValues(Stages Stage)
					{
						m_Variables.Clear();
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
						{
							m_Variables[variable->GetName()] = variable->GetDataType();

							BuildVariable(variable, Stage, Shader);
						}
					}

					virtual void BuildVariable(VariableType* VariableType, Stages Stage, String& Shader) = 0;

					virtual void BuildFunctions(const FunctionList& Functions, Stages Stage, String& Shader)
					{
						for (auto function : Functions)
						{
							if (function->IsEntrypoint())
							{
								FunctionType::Types funcType = function->GetType();

								if (funcType == FunctionType::Types::VertexMain && Stage != Stages::Vertex)
									continue;

								if (funcType == FunctionType::Types::FragmentMain && Stage != Stages::Fragment)
									continue;
							}

							m_OpenScopeCount = 0;

							for (auto parameter : function->GetParameters())
								m_Variables[parameter->GetName()] = parameter->GetDataType();

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
						if (Statement->GetType() == ProgramDataTypes::Bool)
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
						m_Variables[Statement->GetName()] = Statement->GetDataType();

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

					DataType EvaluateDataType(Statement* Statement)
					{
						static ProgramDataTypes MULTIPLY_RESULT[(uint8)ProgramDataTypes::Unknown][(uint8)ProgramDataTypes::Unknown] =
						{
							{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
							{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
							{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float,ProgramDataTypes::Double,ProgramDataTypes::Float2,ProgramDataTypes::Double2,ProgramDataTypes::Float3,ProgramDataTypes::Double3,ProgramDataTypes::Float4,ProgramDataTypes::Double4,ProgramDataTypes::Matrix4 },
							{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double,ProgramDataTypes::Double,ProgramDataTypes::Double2,ProgramDataTypes::Double2,ProgramDataTypes::Double3,ProgramDataTypes::Double3,ProgramDataTypes::Double4,ProgramDataTypes::Double4,ProgramDataTypes::Matrix4 },
							{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float2,ProgramDataTypes::Double2,ProgramDataTypes::Float2,ProgramDataTypes::Double2,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
							{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double2,ProgramDataTypes::Double2,ProgramDataTypes::Double2,ProgramDataTypes::Double2,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
							{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float3,ProgramDataTypes::Double3,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float3,ProgramDataTypes::Double3,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
							{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double3,ProgramDataTypes::Double3,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double3,ProgramDataTypes::Double3,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
							{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float4,ProgramDataTypes::Double4,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float4,ProgramDataTypes::Double4,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
							{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double4,ProgramDataTypes::Double4,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double4,ProgramDataTypes::Double4,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
							{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Matrix4,ProgramDataTypes::Matrix4,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float4,ProgramDataTypes::Double4,ProgramDataTypes::Matrix4 },
						};

						if (IsAssignableFrom(Statement, OperatorStatement))
						{
							OperatorStatement* stm = ReinterpretCast(OperatorStatement*, Statement);

							DataType leftType = EvaluateDataType(stm->GetLeft());
							DataType rightType = EvaluateDataType(stm->GetRight());

							OperatorStatement::Operators op = stm->GetOperator();

							if (op == OperatorStatement::Operators::Multiplication ||
								op == OperatorStatement::Operators::Division)
								return MULTIPLY_RESULT[(uint8)leftType.GetType()][(uint8)rightType.GetType()];

							if (op == OperatorStatement::Operators::Remainder ||
								op == OperatorStatement::Operators::Addition ||
								op == OperatorStatement::Operators::Subtraction)
								return leftType;

							if (op == OperatorStatement::Operators::EqualCheck ||
								op == OperatorStatement::Operators::NotEqualCheck ||
								op == OperatorStatement::Operators::LessCheck ||
								op == OperatorStatement::Operators::LessEqualCheck ||
								op == OperatorStatement::Operators::GreaterCheck ||
								op == OperatorStatement::Operators::GreaterEqualCheck ||
								op == OperatorStatement::Operators::LogicalAnd ||
								op == OperatorStatement::Operators::LogicalOr)
								return ProgramDataTypes::Bool;
						}
						else if (IsAssignableFrom(Statement, UnaryOperatorStatement))
						{
							UnaryOperatorStatement* stm = ReinterpretCast(UnaryOperatorStatement*, Statement);

							return EvaluateDataType(stm->GetStatement());
						}
						else if (IsAssignableFrom(Statement, ConstantStatement))
						{
							ConstantStatement* stm = ReinterpretCast(ConstantStatement*, Statement);

							return stm->GetType();
						}
						else if (IsAssignableFrom(Statement, FunctionCallStatement))
						{
							FunctionCallStatement* stm = ReinterpretCast(FunctionCallStatement*, Statement);

							//???
							//defined
							//builtin
						}
						else if (IsAssignableFrom(Statement, VariableAccessStatement))
						{
							VariableAccessStatement* stm = ReinterpretCast(VariableAccessStatement*, Statement);

							if (m_Variables.Contains(stm->GetName()))
								return m_Variables[stm->GetName()];
						}
						else if (IsAssignableFrom(Statement, ArrayElementAccessStatement))
						{
							ArrayElementAccessStatement* stm = ReinterpretCast(ArrayElementAccessStatement*, Statement);

							return EvaluateDataType(stm->GetArrayStatement());
						}
						else if (IsAssignableFrom(Statement, MemberAccessStatement))
						{
							MemberAccessStatement* stm = ReinterpretCast(MemberAccessStatement*, Statement);

							DataType leftDataType = EvaluateDataType(stm->GetLeft());

							if (leftDataType.IsBuiltIn())
							{
								ProgramDataTypes leftType = leftDataType.GetType();

								if (leftType == ProgramDataTypes::Float2 ||
									leftType == ProgramDataTypes::Float3 ||
									leftType == ProgramDataTypes::Float4)
									return ProgramDataTypes::Float;

								if (leftType == ProgramDataTypes::Double2 ||
									leftType == ProgramDataTypes::Double3 ||
									leftType == ProgramDataTypes::Double4)
									return ProgramDataTypes::Double;

								if (leftType == ProgramDataTypes::Matrix4)
									return ProgramDataTypes::Float;
							}
							else
							{
								int32 index = m_Structs.FindIf([&leftDataType](auto structType) { return structType->GetName() == leftDataType.GetUserDefined(); });
								if (index != -1)
								{
									const StructType* structType = m_Structs[index];

									index = structType->GetItems().FindIf([stm](auto variableType) { return variableType->GetName() == stm->GetRight()->ToString(); });
									if (index != -1)
									{
										const VariableType* variableType = structType->GetItems()[index];

										return variableType->GetDataType();
									}
								}
							}
						}

						return {};
					}

					AllocatorBase* GetAllocator(void) const
					{
						return m_Allocator;
					}

					const StructList& GetStructs(void) const
					{
						return m_Structs;
					}

					void IncreamentOpenScopeCount(void)
					{
						++m_OpenScopeCount;
					}

				private:
					DataType FindVariableType(const String& Name) const
					{

					}

				private:
					AllocatorBase* m_Allocator;
					int8 m_OpenScopeCount;

					StructList m_Structs;
					VariableTypeMap m_Variables;
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
						m_Outputs.Clear();

						return APICompiler::Compile(Structs, Variables, Functions, Output);
					}

				private:
					virtual void ResetPerStageValues(Stages Stage) override
					{
						APICompiler::ResetPerStageValues(Stage);

						m_Parameters.Clear();
						m_AdditionalLayoutCount = SubMeshInfo::GetExtraIndex();
						m_UniformBlockBindingCount = 0;
						m_TextureBlockBindingCount = 0;
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
						Shader += String(" ") + GetReturnBoolName() + "=false;";

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
							op == OperatorStatement::Operators::MultiplicationAssignment ||
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

						APICompiler::BuildMemberAccessStatement(Statement, Type, Stage, Shader);
					}

					virtual void BuildIfStatement(IfStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						APICompiler::BuildIfStatement(Statement, Type, Stage, Shader);

						if (Type != FunctionType::Types::None && ContainsReturnStatement(Statement))
						{
							Shader += String("if (!") + GetReturnBoolName() + ")";

							ADD_NEW_LINE();

							Shader += "{";

							IncreamentOpenScopeCount();
						}
					}

					virtual void BuildReturnStatement(ReturnStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						Shader += String(GetReturnBoolName()) + "=true;";

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

						if (DataType.GetUserDefined().GetLength() != 0)
						{
							int32 index = GetStructs().FindIf([&DataType](auto item) { return item->GetName() == DataType.GetUserDefined(); });
							if (index != -1)
							{
								BuildUniformBlock(GetStructs()[index], Name, Stages::Vertex, Shader);

								return;
							}
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

					void BuildUniformBlock(const StructType* Struct, const String& Name, Stages Stage, String& Shader)
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

					static cstr GetReturnBoolName(void)
					{
						static cstr name = "_MustReturn";
						return name;
					}

				private:
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
						m_InputAssemblerStruct(nullptr),
						m_LastFunction(nullptr),
						m_Add_SV_Position(false),
						m_TextureBufferCount(0),
						m_ConstantBufferCount(0)
					{
					}

					virtual bool Compile(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, Compiler::OutputInfo& Output) override
					{
						m_Functions = Functions;

						if (!APICompiler::Compile(Structs, Variables, Functions, Output))
							return false;

						if (m_Add_SV_Position)
							Output.FragmentShader = "float4 dx_frag_coord:SV_Position;" + Output.FragmentShader;

						String rootSignature = "#define ";
						rootSignature += GetRootSignatureDefineName();
						rootSignature += " \"";

						rootSignature += "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)";

						uint8 constantBufferIndex = 0;
						for (auto variableType : Variables)
						{
							const DataType& dataType = variableType->GetDataType();

							if (variableType->GetDataType().IsBuiltIn())
								continue;

							int32 structIndex = Structs.FindIf([dataType](auto item) { return item->GetName() == dataType.GetUserDefined(); });
							if (structIndex == -1)
								continue;

							uint16 size = GetStructSize(Structs[structIndex]);

							rootSignature += ",RootConstants(num32BitConstants=";
							rootSignature += StringUtility::ToString<char8>(size / 4);
							rootSignature += ",b";
							rootSignature += StringUtility::ToString<char8>(constantBufferIndex++);
							rootSignature += ")";
						}

						if (Variables.ContainsIf([](auto item) {return item->GetDataType().GetType() == ProgramDataTypes::Texture2D; }))
						{
							rootSignature += ",DescriptorTable(";

							uint8 textureIndex = 0;
							for (auto variableType : Variables)
							{
								const DataType& dataType = variableType->GetDataType();

								if (!variableType->GetDataType().IsBuiltIn())
									continue;

								if (dataType.GetType() != ProgramDataTypes::Texture2D)
									continue;

								if (textureIndex != 0)
									rootSignature += ",";

								rootSignature += "SRV(t";
								rootSignature += StringUtility::ToString<char8>(textureIndex++);
								rootSignature += ")";
							}

							rootSignature += ")";
						}

						rootSignature += "\"\n";

						Output.VertexShader = rootSignature + Output.VertexShader;

						return true;
					}

				private:
					virtual void ResetPerStageValues(Stages Stage) override
					{
						APICompiler::ResetPerStageValues(Stage);

						m_TextureBufferCount = 0;
						m_ConstantBufferCount = 0;
					}

					virtual void BuildStruct(StructType* Struct, Stages Stage, String& Shader) override
					{
						BuildStruct(Struct, Stage, false, Shader);

						if (Struct->GetItems().ContainsIf([](auto item) {return item->GetRegister().GetLength() != 0; }))
						{
							m_InputAssemblerStruct = Struct;

							BuildStruct(Struct, Stage, true, Shader);
						}
					}

					virtual void BuildVariable(VariableType* Variable, Stages Stage, String& Shader) override
					{
						DataType dataType = Variable->GetDataType();

						if (dataType.IsBuiltIn())
						{
							for (auto allowedDataType : ALLOWED_CONTEXT_FREE_DATA_TYPES)
							{
								if (allowedDataType != dataType.GetType())
									continue;

								break;
							}
						}
						else
						{
							Shader += "ConstantBuffer<";
						}

						BuildDataType(dataType, Shader);

						if (!dataType.IsBuiltIn())
							Shader += ">";

						Shader += " ";
						Shader += Variable->GetName();

						const String& registerName = Variable->GetRegister();
						if (registerName.GetLength() != 0)
						{
							Shader += ":";
							Shader += registerName;
						}
						else
						{
							if (dataType.IsBuiltIn())
							{
								if (dataType.GetType() == ProgramDataTypes::Texture2D)
								{
									Shader += ":register(t";
									Shader += StringUtility::ToString<char8>(m_TextureBufferCount++);
									Shader += ")";
								}
							}
							else
							{
								Shader += ":register(b";
								Shader += StringUtility::ToString<char8>(m_ConstantBufferCount++);
								Shader += ")";
							}
						}

						Shader += ";";

						ADD_NEW_LINE();
					}

					virtual void BuildFunction(FunctionType* Function, Stages Stage, String& Shader) override
					{
						m_LastFunction = Function;

						FunctionType::Types funcType = Function->GetType();

						if (funcType == FunctionType::Types::VertexMain)
						{
							Shader += "[RootSignature(";
							Shader += GetRootSignatureDefineName();
							Shader += ")]";
							ADD_NEW_LINE();
						}

						if (Function->IsEntrypoint())
						{
							Shader += GetOutputStructName();
							Shader += " ";
						}
						else
							BuildDataType(Function->GetReturnDataType(), Shader);

						Shader += " ";

						if (Function->IsEntrypoint())
							Shader += Compiler::ENTRY_POINT_NAME;
						else
							Shader += Function->GetName();

						Shader += "(";

						ParameterType* parameter = nullptr;
						bool isFirst = true;
						for (auto par : Function->GetParameters())
						{
							if (!isFirst)
								Shader += ",";
							isFirst = false;

							BuildDataType(par->GetDataType(), Shader);
							Shader += " ";
							Shader += par->GetName();

							parameter = par;
						}

						Shader += ")";

						ADD_NEW_LINE();

						Shader += "{";

						ADD_NEW_LINE();

						if (Function->IsEntrypoint())
						{
							Shader += GetOutputStructName();
							Shader += " ";
							Shader += GetStageResultVariableName();
							Shader += ";";
							ADD_NEW_LINE();

							for (auto variableType : m_InputAssemblerStruct->GetItems())
							{
								Shader += GetStageResultVariableName();
								Shader += ".";
								Shader += variableType->GetName();
								Shader += "=";
								Shader += parameter->GetName();
								Shader += ".";
								Shader += variableType->GetName();
								Shader += ";";

								ADD_NEW_LINE();
							}
						}

						BuildStatementHolder(Function, funcType, Stage, Shader);

						Shader += "}";

						ADD_NEW_LINE();
					}

					virtual void BuildOperatorStatement(OperatorStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						OperatorStatement::Operators op = Statement->GetOperator();

						if (op == OperatorStatement::Operators::Multiplication)
						{
							if (EvaluateDataType(Statement->GetLeft()).GetType() == ProgramDataTypes::Matrix4)
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
							op == OperatorStatement::Operators::MultiplicationAssignment ||
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

						if (Stage == Stages::Fragment && name == "_FragPosition")
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
						for (uint8 i = 0; i < m_LastFunction->GetReturnDataType().GetElementCount(); ++i)
						{
							Shader += GetStageResultVariableName();
							Shader += ".";
							Shader += GetStageResultFieldName(i);
							Shader += "=";

							BuildStatement(Statement->GetStatement(), Type, Stage, Shader);

							Shader += ";";
						}						

						Shader += "return ";
						Shader += GetStageResultVariableName();
					}

					virtual void BuildArrayStatement(ArrayStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader) override
					{
						//Assert(false, "Unsupported Location for Statement");
					}

					virtual void BuildDataType(const DataType& Type, String& Shader) override
					{
						if (Type.IsBuiltIn())
							BuildType(Type.GetType(), Shader);
						else
							Shader += Type.GetUserDefined();
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
							Shader += "Texture2D";
							break;
						}
					}

					void BuildStruct(StructType* Struct, Stages Stage, bool IsOutputStruct, String& Shader)
					{
						Shader += "struct ";

						if (IsOutputStruct)
							Shader += GetOutputStructName();
						else
							Shader += Struct->GetName();

						ADD_NEW_LINE();
						Shader += "{";
						ADD_NEW_LINE();

						auto variables = Struct->GetItems();

						for (auto variable : variables)
						{
							BuildVariable(variable, Stage, Shader);
						}

						if (IsOutputStruct)
						{
							ProgramDataTypes dataType = ProgramDataTypes::Unknown;
							String registerName;
							uint8 count = 1;

							switch (Stage)
							{
							case APICompiler::Stages::Vertex:
							{
								dataType = ProgramDataTypes::Float4;
								registerName = "SV_POSITION";
							} break;

							case APICompiler::Stages::Tessellation:
							{

							} break;

							case APICompiler::Stages::Geometry:
							{
							} break;

							case APICompiler::Stages::Fragment:
							{
								dataType = ProgramDataTypes::Float4;
								registerName = "SV_TARGET";

								int32 index = m_Functions.FindIf([](auto function) { return function->GetType() == FunctionType::Types::FragmentMain; });
								if (index == -1)
								{
									//error
								}
								count = m_Functions[index]->GetReturnDataType().GetElementCount();
							} break;

							case APICompiler::Stages::Compute:
							{
							} break;
							}

							for (uint8 i = 0; i < count; ++i)
							{
								BuildDataType(dataType, Shader);
								Shader += " ";
								Shader += GetStageResultFieldName(i);
								Shader += ":";
								Shader += registerName;
								Shader += StringUtility::ToString<char8>(i);
								Shader += ";";

								ADD_NEW_LINE();
							}
						}

						Shader += "};";
						ADD_NEW_LINE();
					}

					String GetOutputStructName(void) const
					{
						return m_InputAssemblerStruct->GetName() + "__Result__";
					}

					static String GetStageResultFieldName(uint8 Index)
					{
						return String("__Result__") + StringUtility::ToString<char8>(Index);
					}

					static cstr GetStageResultVariableName(void)
					{
						static cstr name = "__result__";

						return name;
					}

					static cstr GetRootSignatureDefineName(void)
					{
						static cstr name = "__RS__";

						return name;
					}

				private:
					StructType* m_InputAssemblerStruct;
					FunctionType* m_LastFunction;
					FunctionList m_Functions;
					bool m_Add_SV_Position;
					uint8 m_TextureBufferCount;
					uint8 m_ConstantBufferCount;
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
								}

								structMeta.Size = GetStructSize(structType);
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

#undef ADD_NEW_LINE
		}
	}
}