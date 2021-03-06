// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ProgramCompiler\APICompiler.h>
#include <Rendering\GPUAlignedType.h>

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

				APICompiler::APICompiler(DeviceTypes DeviceType) :
					IntrinsicFunctions(DeviceType),
					m_OpenScopeCount(0)
				{
				}

				bool APICompiler::Compile(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, CompileOutputInfo& Output)
				{
					m_OpenScopeCount = 0;

					m_Structs = Structs;
					m_Functions = Functions;

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

				void APICompiler::BuildStageShader(Stages Stage, const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
				{
					ResetPerStageValues(Stage);

					BuildHeader(Shader);

					BuildStructs(Structs, Stage, Shader);

					BuildVariables(Variables, Stage, Shader);

					BuildFunctions(Functions, Stage, Shader);
				}

				void APICompiler::BuildVertexShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
				{
					BuildStageShader(Stages::Vertex, Structs, Variables, Functions, Shader);
				}

				void APICompiler::BuildTessellationShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
				{
					BuildStageShader(Stages::Tessellation, Structs, Variables, Functions, Shader);
				}

				void APICompiler::BuildGeometryShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
				{
					BuildStageShader(Stages::Geometry, Structs, Variables, Functions, Shader);
				}

				void APICompiler::BuildFragmentShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
				{
					BuildStageShader(Stages::Fragment, Structs, Variables, Functions, Shader);
				}

				void APICompiler::BuildComputeShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
				{
					BuildStageShader(Stages::Compute, Structs, Variables, Functions, Shader);
				}

				void APICompiler::ResetPerStageValues(Stages Stage)
				{
					m_Variables.Clear();
				}

				void APICompiler::BuildHeader(String& Shader)
				{
				}

				void APICompiler::BuildStructs(const StructList& Structs, Stages Stage, String& Shader)
				{
					for (auto structType : Structs)
						BuildStruct(structType, Stage, Shader);
				}

				void APICompiler::BuildVariables(const VariableList& Variables, Stages Stage, String& Shader)
				{
					for (auto variable : Variables)
					{
						m_Variables[variable->GetName()] = variable->GetDataType();

						BuildVariable(variable, Stage, Shader);
					}
				}

				void APICompiler::BuildFunctions(const FunctionList& Functions, Stages Stage, String& Shader)
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

				void APICompiler::BuildStatementHolder(StatementItemHolder* Holder, FunctionType::Types Type, Stages Stage, String& Shader)
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

				void APICompiler::BuildStatement(Statement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
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

				void APICompiler::BuildOperatorStatement(OperatorStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					OperatorStatement::Operators op = Statement->GetOperator();

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

				void APICompiler::BuildUnaryOperatorStatement(UnaryOperatorStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					Shader += "(";

					Shader += UnaryOperatorStatement::GetOperatorSymbol(Statement->GetOperator());

					BuildStatement(Statement->GetStatement(), Type, Stage, Shader);

					Shader += ")";
				}

				void APICompiler::BuildConstantStatement(ConstantStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					if (Statement->GetType() == ProgramDataTypes::Bool)
						Shader += StringUtility::ToString<char8>(Statement->GetBool());
					else if (Statement->GetFloat32() == 0 || Statement->GetFloat32() / (int32)Statement->GetFloat32() == 1)
						Shader += StringUtility::ToString<char8>((int32)Statement->GetFloat32());
					else
						Shader += StringUtility::ToString<char8>(Statement->GetFloat32());
				}

				void APICompiler::BuildFunctionCallStatement(FunctionCallStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					auto& funcName = Statement->GetFunctionName();

					if (m_Functions.ContainsIf([funcName](auto item) { return item->GetName() == funcName; }))
					{
						Shader += funcName;

						Shader += "(";

						BuildArguments(Statement->GetArguments(), Type, Stage, Shader);

						Shader += ")";
					}
					else
						BuildIntrinsicFunctionCallStatement(Statement, Type, Stage, Shader);
				}

				void APICompiler::BuildArguments(const StatementItemHolder& Statements, FunctionType::Types Type, Stages Stage, String& Shader)
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

				void APICompiler::BuildVariableStatement(VariableStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
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

				void APICompiler::BuildArrayElementAccessStatement(ArrayElementAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					BuildStatement(Statement->GetArrayStatement(), Type, Stage, Shader);

					Shader += "[";

					BuildStatement(Statement->GetElementStatement(), Type, Stage, Shader);

					Shader += "]";
				}

				void APICompiler::BuildMemberAccessStatement(MemberAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					BuildStatement(Statement->GetLeft(), Type, Stage, Shader);

					Shader += ".";

					BuildStatement(Statement->GetRight(), Type, Stage, Shader);
				}

				void APICompiler::BuildSemicolonStatement(SemicolonStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					Shader += ";";

					ADD_NEW_LINE();
				}

				void APICompiler::BuildIfStatement(IfStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
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

				void APICompiler::BuildElseStatement(ElseStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					Shader += "else";

					ADD_NEW_LINE();

					Shader += "{";

					ADD_NEW_LINE();

					BuildStatementHolder(Statement, Type, Stage, Shader);

					Shader += "}";

					ADD_NEW_LINE();
				}

				void APICompiler::BuildDiscardStatement(DiscardStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					Shader += "discard";
				}

				void APICompiler::BuildDataType(const DataType& Type, String& Shader)
				{
					if (Type.IsBuiltIn())
						BuildType(Type.GetType(), Shader);
					else
						Shader += Type.GetUserDefined();
				}

				bool APICompiler::ContainsReturnStatement(StatementItemHolder* Statement)
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

				DataType APICompiler::EvaluateDataType(Statement* CurrentStatement, Statement* TopStatement) const
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

					if (IsAssignableFrom(CurrentStatement, OperatorStatement))
					{
						OperatorStatement* stm = ReinterpretCast(OperatorStatement*, CurrentStatement);

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
					else if (IsAssignableFrom(CurrentStatement, UnaryOperatorStatement))
					{
						UnaryOperatorStatement* stm = ReinterpretCast(UnaryOperatorStatement*, CurrentStatement);

						return EvaluateDataType(stm->GetStatement());
					}
					else if (IsAssignableFrom(CurrentStatement, ConstantStatement))
					{
						ConstantStatement* stm = ReinterpretCast(ConstantStatement*, CurrentStatement);

						return stm->GetType();
					}
					else if (IsAssignableFrom(CurrentStatement, FunctionCallStatement))
					{
						FunctionCallStatement* stm = ReinterpretCast(FunctionCallStatement*, CurrentStatement);

						uint32 hash = IntrinsicFunctions::CalculateFunctionSignatureHash(stm);

						int32 index = m_Functions.FindIf([hash](auto item)
							{
								ProgramDataTypes parameterTypes[16];
								uint8 parameterTypeCount = 0;
								const auto& parameters = item->GetParameters();
								for (auto& parameter : parameters)
									parameterTypes[parameterTypeCount++] = parameter->GetDataType().GetType();

								return (hash == IntrinsicFunctions::CalculateFunctionSignatureHash(item->GetName(), parameterTypes, parameterTypeCount));
							});

						if (index != -1)
							return m_Functions[index]->GetReturnDataType();

						return EvaluateIntrinsicFunctionReturnValue(stm);
					}
					else if (IsAssignableFrom(CurrentStatement, VariableAccessStatement))
					{
						VariableAccessStatement* stm = ReinterpretCast(VariableAccessStatement*, CurrentStatement);

						const String& variableName = stm->GetName();

						if (m_Variables.Contains(variableName))
							return m_Variables[stm->GetName()];

						if (TopStatement != nullptr)
						{
							DataType topDataType = EvaluateDataType(TopStatement);

							const StructType* structType = FindStructType(topDataType.GetUserDefined());
							if (structType != nullptr)
							{
								const VariableType* variableType = FindVariableType(structType, variableName);
								if (variableType != nullptr)
									return variableType->GetDataType();
							}
						}
					}
					else if (IsAssignableFrom(CurrentStatement, ArrayElementAccessStatement))
					{
						ArrayElementAccessStatement* stm = ReinterpretCast(ArrayElementAccessStatement*, CurrentStatement);

						return EvaluateDataType(stm->GetArrayStatement());
					}
					else if (IsAssignableFrom(CurrentStatement, MemberAccessStatement))
					{
						MemberAccessStatement* stm = ReinterpretCast(MemberAccessStatement*, CurrentStatement);

						DataType leftDataType = EvaluateDataType(stm->GetLeft(), TopStatement);

						if (leftDataType.IsBuiltIn())
						{
							ProgramDataTypes leftType = leftDataType.GetType();
							uint8 advance = (stm->GetRight()->ToString().GetLength() - 1) * 2;

							if (leftType == ProgramDataTypes::Float2 ||
								leftType == ProgramDataTypes::Float3 ||
								leftType == ProgramDataTypes::Float4)
								return (ProgramDataTypes)((uint8)ProgramDataTypes::Float + advance);

							if (leftType == ProgramDataTypes::Double2 ||
								leftType == ProgramDataTypes::Double3 ||
								leftType == ProgramDataTypes::Double4)
								return (ProgramDataTypes)((uint8)ProgramDataTypes::Double + advance);

							if (leftType == ProgramDataTypes::Matrix4)
								return ProgramDataTypes::Float;
						}
						else if (IsAssignableFrom(stm->GetRight(), VariableAccessStatement))
						{
							const StructType* structType = FindStructType(leftDataType.GetUserDefined());
							if (structType != nullptr)
							{
								const VariableType* variableType = FindVariableType(structType, stm->GetRight()->ToString());
								if (variableType != nullptr)
									return variableType->GetDataType();
							}
						}
						else
							return EvaluateDataType(stm->GetRight(), stm->GetLeft());
					}

					return {};
				}

				const StructType* APICompiler::FindStructType(const String& Name) const
				{
					int32 index = m_Structs.FindIf([&Name](auto structType) { return structType->GetName() == Name; });
					if (index == -1)
						return nullptr;

					return m_Structs[index];
				}

				const VariableType* APICompiler::FindVariableType(const StructType* StructType, const String& Name) const
				{
					int32 index = StructType->GetItems().FindIf([&Name](auto variableType) { return variableType->GetName() == Name; });
					if (index == -1)
						return nullptr;

					return StructType->GetItems()[index];
				}

				void APICompiler::GetAlignedOffset(ProgramDataTypes DataType, uint16& Offset, uint8& Size)
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

				uint16 APICompiler::GetAlignedSize(uint16 Size, uint8 Alignment)
				{
					if (Size % Alignment != 0)
						Size = ((Size / Alignment) + 1) * Alignment;

					return Size;
				}

				uint16 APICompiler::GetStructSize(const StructType* Struct)
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

				ProgramDataTypes APICompiler::EvaluateProgramDataType(Statement* Statement) const
				{
					return EvaluateDataType(Statement).GetType();
				}

#undef ADD_NEW_LINE
			}
		}
	}
}