// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ASTToGLSLCompiler\Private\ASTToGLSLCompiler.h>
#include <ProgramCompilerCommon\Common.h>
#include <ProgramCompilerCommon\ProgramCompilerException.h>
#include <RenderDevice\MeshInfo.h>

namespace Engine
{
	using namespace ProgramCompilerCommon;
	using namespace RenderDevice;

	namespace ASTToGLSLCompiler
	{
		namespace Private
		{
#ifdef DEBUG_MODE
#define ADD_NEW_LINE() Data.Shader += "\n"
#else
#define ADD_NEW_LINE()
#endif

			VertexLayouts GetLayout(StructVariableType::Registers Register)
			{
				switch (Register)
				{
				case StructVariableType::Registers::Position:
					return VertexLayouts::Position;

				case StructVariableType::Registers::Normal:
					return VertexLayouts::Normal;

				case StructVariableType::Registers::UV:
					return VertexLayouts::TexCoord;

				case StructVariableType::Registers::Color:
				case StructVariableType::Registers::TessellationFactor:
				case StructVariableType::Registers::InsideTessellationFactor:
				case StructVariableType::Registers::PrimitiveID:
				case StructVariableType::Registers::DomainLocation:
				case StructVariableType::Registers::InstanceID:
				case StructVariableType::Registers::FragmentPosition:
				case StructVariableType::Registers::Target:
				case StructVariableType::Registers::DispatchThreadID:
				case StructVariableType::Registers::GroupID:
				case StructVariableType::Registers::GroupIndex:
				case StructVariableType::Registers::GroupThreadID:
					return VertexLayouts::Position;

				default:
					THROW_PROGRAM_COMPILER_EXCEPTION("Register not defined", String::Empty);
				}

			}

			String GetFragmentVariableName(uint8 Index)
			{
				return String(Constants::FRAGMENT_ENTRY_POINT_NAME) + "_FragColor" + StringUtility::ToString<char8>(Index);
			}

			ASTToGLSLCompiler::ASTToGLSLCompiler() :
				m_AdditionalLayoutCount(0),
				m_BindingCount(0)
			{
			}

			void ASTToGLSLCompiler::ResetPerStageValues(const StageData& Data)
			{
				ASTCompilerBase::ResetPerStageValues(Data);

				m_AdditionalLayoutCount = SubMeshInfo::GetLayoutCount();
				m_BindingCount = 0;
			}

			void ASTToGLSLCompiler::BuildStageShader(const StageData& Data)
			{
				ASTCompilerBase::BuildStageShader(Data);

				Data.Shader = "#version 460 core\n" + Data.Shader;
			}

			void ASTToGLSLCompiler::BuildStruct(StructType* Struct, const StageData& Data)
			{
				Data.Shader += "struct ";
				Data.Shader += Struct->GetName();
				ADD_NEW_LINE();

				Data.Shader += '{';
				ADD_NEW_LINE();

				for (auto& variable : Struct->GetItems())
				{
					BuildDataTypeStatement(variable->GetDataType(), Data);

					Data.Shader += ' ';

					Data.Shader += variable->GetName();

					BuildPostDataType(variable->GetDataType(), Data);

					Data.Shader += ';';

					ADD_NEW_LINE();
				}

				Data.Shader += "};";
				ADD_NEW_LINE();
			}

			void ASTToGLSLCompiler::BuildGlobalVariable(GlobalVariableType* Variable, const StageData& Data)
			{
				const String& name = Variable->GetName();
				DataTypeStatement* dataType = Variable->GetDataType();

				if (!dataType->IsBuiltIn())
				{
					const StructType* structType = GetStructType(dataType->GetUserDefined());

					BuildUniformBlock(structType, name, Data);

					return;
				}

				for (auto allowedDataType : ALLOWED_CONTEXT_FREE_DATA_TYPES)
				{
					if (allowedDataType != dataType->GetType())
						continue;

					if (allowedDataType == ProgramDataTypes::Texture2D)
					{
						Data.Shader += "layout(location = ";
						Data.Shader += StringUtility::ToString<char8>(m_BindingCount);
						Data.Shader += ", binding = ";
						Data.Shader += StringUtility::ToString<char8>(m_BindingCount);
						Data.Shader += ")";

						++m_BindingCount;
					}

					Data.Shader += "uniform ";
					break;
				}

				BuildDataTypeStatement(dataType, Data);
				Data.Shader += " ";
				Data.Shader += name;
				Data.Shader += ";";

				ADD_NEW_LINE();
			}

			void ASTToGLSLCompiler::BuildFunction(FunctionType* Function, const StageData& Data)
			{
				FunctionType::Types funcType = Function->GetType();

				if (funcType == FunctionType::Types::VertexMain ||
					funcType == FunctionType::Types::HullMain ||
					funcType == FunctionType::Types::DomainMain ||
					funcType == FunctionType::Types::FragmentMain)
				{
					const auto* parameter = Function->GetParameters()[0];
					BuildInOutStruct(parameter->GetDataType(), parameter->GetName(), true, Data);
					BuildInOutStruct(Function->GetReturnDataType(), String::Empty, false, Data);
				}

				if (funcType == FunctionType::Types::FragmentMain)
				{
					uint8 elementCount = EvaluateDataTypeElementCount(Function->GetReturnDataType());

					for (uint8 i = 0; i < elementCount; ++i)
					{
						Data.Shader += "layout(location = ";
						Data.Shader += StringUtility::ToString<char8>(i);
						Data.Shader += ") out ";
						BuildType(ProgramDataTypes::Float4, Data);
						Data.Shader += " ";
						Data.Shader += GetFragmentVariableName(i);
						Data.Shader += ";";

						ADD_NEW_LINE();
					}
				}

				if (Function->IsEntrypoint())
					BuildType(ProgramDataTypes::Void, Data);
				else
					BuildDataTypeStatement(Function->GetReturnDataType(), Data);

				Data.Shader += " ";

				if (Function->IsEntrypoint())
					Data.Shader += Constants::ENTRY_POINT_NAME;
				else
					Data.Shader += Function->GetName();

				IncreaseBlockIndex();

				Data.Shader += "(";

				if (Function->IsEntrypoint())
				{
					for (auto& parameter : Function->GetParameters())
						PushVariable(parameter);
				}
				else
					BuildParameters(Function->GetParameters(), Data);

				Data.Shader += ")";
				ADD_NEW_LINE();

				Data.Shader += "{";
				ADD_NEW_LINE();

				BuildStatementHolder(Function, Data);

				Data.Shader += "}";
				ADD_NEW_LINE();

				DecreaseBlockIndex();
			}

			void ASTToGLSLCompiler::BuildConstantEntrypointAttributeType(ConstantEntrypointAttributeType* Attribute, const StageData& Data)
			{
				//THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
			}

			void ASTToGLSLCompiler::BuildVariableAccessStatement(VariableAccessStatement* Statement, const StageData& Data)
			{
				Data.Shader += Statement->GetName();
			}

			void ASTToGLSLCompiler::BuildMemberAccessStatement(MemberAccessStatement* Statement, const StageData& Data)
			{
				if (Data.FunctionType != FunctionType::Types::None)
				{
					String leftStm;
					BuildStatement(Statement->GetLeft(), { Data.FunctionType, Data.Stage, Data.Structs, Data.Variables, Data.Functions, leftStm });

					const VariableType* leftVariable = FindVariableType(leftStm);
					if (leftVariable != nullptr)
					{
						bool isParameter = IsAssignableFrom(leftVariable, const ParameterType);
						bool isOutputVariable = (IsAssignableFrom(leftVariable, const VariableStatement) && leftVariable->GetDataType()->GetUserDefined() == GetLastFunction()->GetReturnDataType()->GetUserDefined());

						String name = String::Empty;
						if (isParameter)
							name = ReinterpretCast(const ParameterType*, leftVariable)->GetName();

						if (isParameter || isOutputVariable)
						{
							const StructType* structType = GetStructType(leftVariable->GetDataType()->GetUserDefined());

							bool isMemberAccess = IsAssignableFrom(Statement->GetRight(), MemberAccessStatement);
							auto* rightStatement = (isMemberAccess ? ReinterpretCast(MemberAccessStatement*, Statement->GetRight())->GetLeft() : Statement->GetRight());

							String rightStm;
							BuildStatement(rightStatement, { Data.FunctionType, Data.Stage, Data.Structs, Data.Variables, Data.Functions, rightStm });
							rightStm = rightStm.Split('.')[0];

							const StructVariableType* rightVariable = GetVariableType(structType, rightStm);

							BuildFlattenStructMemberVariableName(structType, rightVariable, name, isParameter, Data);

							if (isMemberAccess)
							{
								Data.Shader += '.';

								BuildStatement(ReinterpretCast(MemberAccessStatement*, Statement->GetRight())->GetRight(), Data);
							}

							return;
						}
					}
				}

				ASTCompilerBase::BuildMemberAccessStatement(Statement, Data);
			}

			void ASTToGLSLCompiler::BuildSwitchStatement(SwitchStatement* Statement, const StageData& Data)
			{
				bool isFirstOne = true;
				bool isCoupling = false;

				for (auto statement : Statement->GetItems())
				{
					if (IsAssignableFrom(statement, BreakStatement))
						continue;

					if (IsAssignableFrom(statement, CaseStatement))
					{
						if (!isCoupling)
						{
							if (isFirstOne)
							{
								Data.Shader += "if ";
								isFirstOne = false;
							}
							else
								Data.Shader += "else if ";

							Data.Shader += "(";
						}

						isCoupling = false;

						BuildStatement(Statement->GetSelector(), Data);

						Data.Shader += " == ";

						CaseStatement* caseStatement = ReinterpretCast(CaseStatement*, statement);

						BuildStatement(caseStatement->GetCondition(), Data);

						if (caseStatement->GetItems().GetSize() == 0)
						{
							Data.Shader += " || ";

							isCoupling = true;
							continue;
						}

						Data.Shader += ")";
						ADD_NEW_LINE();
						Data.Shader += "{";

						BuildStatementHolder(caseStatement, Data);

						Data.Shader += "}";
					}
					else if (IsAssignableFrom(statement, DefaultStatement))
					{
						if (!isCoupling)
						{
							if (isFirstOne)
							{
								Data.Shader += "if ";
								isFirstOne = false;
							}
							else
								Data.Shader += "else if ";

							Data.Shader += "(";
						}

						isCoupling = false;

						Data.Shader += "true";

						DefaultStatement* defaultStatement = ReinterpretCast(DefaultStatement*, statement);

						if (defaultStatement->GetItems().GetSize() == 0)
						{
							Data.Shader += " || ";

							isCoupling = true;
							continue;
						}

						Data.Shader += ")";
						ADD_NEW_LINE();
						Data.Shader += "{";

						BuildStatementHolder(defaultStatement, Data);

						Data.Shader += "}";
					}
				}
			}

			void ASTToGLSLCompiler::BuildReturnStatement(ReturnStatement* Statement, const StageData& Data)
			{
				if (Data.FunctionType == FunctionType::Types::None)
				{
					Data.Shader += "return ";

					BuildStatement(Statement->GetStatement(), Data);

					Data.Shader += ';';
					ADD_NEW_LINE();

					return;
				}

				const String& returnDataType = GetLastFunction()->GetReturnDataType()->GetUserDefined();
				const StructType* structType = GetStructType(returnDataType);
				const StructVariableType* variableType = nullptr;

				if (Data.FunctionType == FunctionType::Types::VertexMain)
				{
					Data.Shader += "gl_Position = ";

					variableType = FindVariableType(structType, [](const StructVariableType* item) { return item->GetRegister() == StructVariableType::Registers::Position; });
				}
				else if (Data.FunctionType == FunctionType::Types::FragmentMain)
				{
					variableType = FindVariableType(structType, [](const StructVariableType* item) { return item->GetRegister() == StructVariableType::Registers::Color; });

					uint8 elementCount = BuildReturnValue(Statement->GetStatement(), Data);
					for (uint8 i = 0; i < elementCount; ++i)
					{
						Data.Shader += GetFragmentVariableName(i) + " = ";

						Data.Shader += GetStageResultArrayVariableName();

						if (elementCount > 1)
						{
							Data.Shader += '[';
							Data.Shader += StringUtility::ToString<char8>(i);
							Data.Shader += "]";
						}

						Data.Shader += ';';
						ADD_NEW_LINE();
					}
				}

				if (variableType == nullptr)
					return;

				BuildFlattenStructMemberVariableName(structType, variableType, String::Empty, false, Data);

				Data.Shader += ';';
				ADD_NEW_LINE();

				Data.Shader += "return;";
				ADD_NEW_LINE();
			}

			void ASTToGLSLCompiler::BuildArrayStatement(ArrayStatement* Statement, const StageData& Data)
			{
				DataTypeStatement dataType = EvaluateDataType(Statement);

				BuildType(dataType.GetType(), Data);

				Data.Shader += "[](";

				BuildArguments(Statement, Data);

				Data.Shader += ')';
			}

			void ASTToGLSLCompiler::BuildExplicitCast(Statement* Statement, const DataTypeStatement* DataType, const StageData& Data)
			{
				bool needsCasting = !CompareDataTypes(EvaluateDataType(Statement), *DataType);

				if (needsCasting)
				{
					BuildDataTypeStatement(DataType, Data);

					Data.Shader += '(';
				}

				BuildStatement(Statement, Data);

				if (needsCasting)
					Data.Shader += ')';
			}

			void ASTToGLSLCompiler::BuildType(ProgramDataTypes Type, const StageData& Data)
			{
				switch (Type)
				{
				case ProgramDataTypes::Void:
					Data.Shader += "void";
					break;

				case ProgramDataTypes::Bool:
					Data.Shader += "bool";
					break;

				case ProgramDataTypes::Integer:
					Data.Shader += "int";
					break;

				case ProgramDataTypes::UnsignedInteger:
					Data.Shader += "uint";
					break;

				case ProgramDataTypes::Float:
					Data.Shader += "float";
					break;

				case ProgramDataTypes::Double:
					Data.Shader += "double";
					break;

				case ProgramDataTypes::Integer2:
					Data.Shader += "ivec2";
					break;

				case ProgramDataTypes::UnsignedInteger2:
					Data.Shader += "uvec2";
					break;

				case ProgramDataTypes::Float2:
					Data.Shader += "vec2";
					break;

				case ProgramDataTypes::Double2:
					Data.Shader += "dvec2";
					break;

				case ProgramDataTypes::Integer3:
					Data.Shader += "ivec3";
					break;

				case ProgramDataTypes::UnsignedInteger3:
					Data.Shader += "uvec3";
					break;

				case ProgramDataTypes::Float3:
					Data.Shader += "vec3";
					break;

				case ProgramDataTypes::Double3:
					Data.Shader += "dvec3";
					break;

				case ProgramDataTypes::Integer4:
					Data.Shader += "ivec4";
					break;

				case ProgramDataTypes::UnsignedInteger4:
					Data.Shader += "uvec4";
					break;

				case ProgramDataTypes::Float4:
					Data.Shader += "vec4";
					break;

				case ProgramDataTypes::Double4:
					Data.Shader += "dvec4";
					break;

				case ProgramDataTypes::Matrix4F:
				case ProgramDataTypes::Matrix4D:
					Data.Shader += "mat4";
					break;

				case ProgramDataTypes::Texture2D:
					Data.Shader += "sampler2D";
					break;
				}
			}

			void ASTToGLSLCompiler::BuildPostDataType(const DataTypeStatement* Type, const StageData& Data)
			{
				if (Type->GetPostElementCount() != nullptr)
				{
					Data.Shader += "[";
					BuildStatement(Type->GetPostElementCount(), Data);
					Data.Shader += "]";
				}
			}

			void ASTToGLSLCompiler::BuildInOutStruct(const DataTypeStatement* DataType, const String& Name, bool IsInput, const StageData& Data)
			{
				CoreDebugAssert(Categories::ProgramCompiler, !DataType->IsBuiltIn(), "DataType must be user-defined");

				const StructType* structType = GetStructType(DataType->GetUserDefined());
				const auto& variables = structType->GetItems();

				if (variables.ContainsIf([](auto item) { return item->GetRegister() == StructVariableType::Registers::None; }))
					THROW_PROGRAM_COMPILER_EXCEPTION("Struct cannot have variables without register specified", structType->GetName());

				for (auto& variable : variables)
				{
					int8 location = 0;
					if (!IsInput)
						location = m_AdditionalLayoutCount++;
					else
						location = SubMeshInfo::GetLayoutIndex(GetLayout(variable->GetRegister()));

					Data.Shader += "layout(location = ";
					Data.Shader += StringUtility::ToString<char8>(location);
					Data.Shader += ") ";
					Data.Shader += (IsInput ? "in " : "out ");

					BuildDataTypeStatement(variable->GetDataType(), Data);
					Data.Shader += ' ';

					BuildFlattenStructMemberVariableName(structType, variable, Name, IsInput, Data);
					Data.Shader += ';';

					ADD_NEW_LINE();
				}
			}

			void ASTToGLSLCompiler::BuildUniformBlock(const StructType* Struct, const String& Name, const StageData& Data)
			{
				auto variables = Struct->GetItems();

				if (FindVariableType(Struct, [](auto item) { return item->GetRegister() != StructVariableType::Registers::None; }) != nullptr)
					CoreDebugAssert(Categories::ProgramCompiler, false, "Struct %S cannot have variables with register specified", Struct->GetName());

				Data.Shader += "layout(std140, binding = ";
				Data.Shader += StringUtility::ToString<char8>(m_BindingCount++);
				Data.Shader += ") uniform " + Struct->GetName();
				Data.Shader += "_";
				Data.Shader += Name;
				ADD_NEW_LINE();
				Data.Shader += "{";
				ADD_NEW_LINE();

				uint16 offset = 0;
				for (auto variable : variables)
				{
					ProgramDataTypes dataType = variable->GetDataType()->GetType();

					uint8 size = 0;
					StructType::GetAlignedOffset(dataType, offset, size);

					Data.Shader += "layout(offset = ";
					Data.Shader += StringUtility::ToString<char8>(offset);
					Data.Shader += ") ";

					BuildDataTypeStatement(variable->GetDataType(), Data);

					Data.Shader += " ";
					Data.Shader += variable->GetName();
					Data.Shader += ";";

					ADD_NEW_LINE();

					offset += size;
				}

				Data.Shader += "}";

				Data.Shader += Name;

				Data.Shader += ";";

				ADD_NEW_LINE();
			}

			void ASTToGLSLCompiler::BuildFlattenStructMemberVariableName(const StructType* Parent, const StructVariableType* Variable, const String& Name, bool IsInput, const StageData& Data)
			{
				Data.Shader += Name;
				Data.Shader += '_';
				Data.Shader += (IsInput ? "In" : "Out");
				Data.Shader += '_';
				Data.Shader += Parent->GetName();
				Data.Shader += '_';
				Data.Shader += Variable->GetName();
			}

#undef ADD_NEW_LINE
		}
	}
}