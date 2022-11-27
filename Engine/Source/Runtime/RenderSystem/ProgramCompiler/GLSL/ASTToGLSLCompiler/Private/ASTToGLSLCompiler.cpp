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
#define ADD_NEW_LINE() Shader += "\n"
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

			cstr GetReturnBoolName(void)
			{
				static cstr name = "_MustReturn";
				return name;
			}

			ASTToGLSLCompiler::ASTToGLSLCompiler() :
				m_AdditionalLayoutCount(0),
				m_BindingCount(0),
				m_BlockIndex(0),
				m_CurrentBuildingFunction(nullptr)
			{
			}

			void ASTToGLSLCompiler::Initialize(DeviceTypes DeviceType)
			{
				ASTCompilerBase::Initialize(DeviceType);
			}

			void ASTToGLSLCompiler::Compile(AllocatorBase* Allocator, const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, OutputInfo& Output)
			{
				//m_Outputs = OutputMap(Allocator);
				m_AdditionalLayoutCount = 0;
				m_BindingCount = 0;
				m_BlockVariables = BlockVariablesList(Allocator);

				try
				{
					ASTCompilerBase::Compile(Allocator, Structs, Variables, Functions, Output);
				}
				catch (...)
				{
					m_BlockVariables = BlockVariablesList();

					throw;
				}

				m_BlockVariables = BlockVariablesList();
			}

			void ASTToGLSLCompiler::ResetPerStageValues(Stages Stage)
			{
				ASTCompilerBase::ResetPerStageValues(Stage);

				m_AdditionalLayoutCount = SubMeshInfo::GetLayoutCount();
				m_BindingCount = 0;

				m_BlockVariables.Clear();
				m_BlockIndex = -1;

				m_CurrentBuildingFunction = nullptr;

				IncreaseBlockIndex();
			}

			void ASTToGLSLCompiler::BuildStageShader(Stages Stage, const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, String& Shader)
			{
				ASTCompilerBase::BuildStageShader(Stage, Structs, Variables, Functions, Shader);

				Shader = "#version 460 core\n" + Shader;
			}

			void ASTToGLSLCompiler::BuildGlobalVariable(GlobalVariableType* Variable, Stages Stage, String& Shader)
			{
				PushVariable(Variable);

				const String& name = Variable->GetName();
				DataTypeStatement* dataType = Variable->GetDataType();

				if (!dataType->IsBuiltIn())
				{
					const StructType* structType = GetStructType(dataType->GetUserDefined());

					BuildUniformBlock(structType, name, Stages::Vertex, Shader);

					return;
				}

				for (auto allowedDataType : ALLOWED_CONTEXT_FREE_DATA_TYPES)
				{
					if (allowedDataType != dataType->GetType())
						continue;

					if (allowedDataType == ProgramDataTypes::Texture2D)
					{
						Shader += "layout(location = ";
						Shader += StringUtility::ToString<char8>(m_BindingCount);
						Shader += ", binding = ";
						Shader += StringUtility::ToString<char8>(m_BindingCount);
						Shader += ")";

						++m_BindingCount;
					}

					Shader += "uniform ";
					break;
				}

				BuildDataTypeStatement(dataType, Shader);
				Shader += " ";
				Shader += name;
				Shader += ";";

				ADD_NEW_LINE();
			}

			void ASTToGLSLCompiler::BuildFunction(FunctionType* Function, Stages Stage, String& Shader)
			{
				m_CurrentBuildingFunction = Function;

				FunctionType::Types funcType = Function->GetType();

				if (funcType == FunctionType::Types::VertexMain ||
					funcType == FunctionType::Types::HullMain ||
					funcType == FunctionType::Types::DomainMain ||
					funcType == FunctionType::Types::FragmentMain)
				{
					const auto* parameter = Function->GetParameters()[0];
					BuildInOutStruct(parameter->GetDataType(), parameter->GetName(), true, Shader);
					BuildInOutStruct(Function->GetReturnDataType(), String::Empty, false, Shader);
				}

				if (funcType == FunctionType::Types::FragmentMain)
				{
					uint8 elementCount = EvaluateDataTypeElementCount(Function->GetReturnDataType());

					for (uint8 i = 0; i < elementCount; ++i)
					{
						Shader += "layout(location=";
						Shader += StringUtility::ToString<char8>(i);
						Shader += ") out ";
						BuildType(ProgramDataTypes::Float4, Shader);
						Shader += " ";
						Shader += GetFragmentVariableName(i);
						Shader += ";";

						ADD_NEW_LINE();
					}
				}

				BuildType(ProgramDataTypes::Void, Shader);

				Shader += " ";

				if (Function->IsEntrypoint())
					Shader += Constants::ENTRY_POINT_NAME;
				else
					Shader += Function->GetName();

				Shader += "(";

				if (Function->IsEntrypoint())
				{
					for (auto& parameter : Function->GetParameters())
						PushVariable(parameter);
				}
				else
				{
					BuildParameters(Function->GetParameters(), funcType, Stage, Shader);

					BuildFlattenParameters(Function->GetReturnDataType(), String::Empty, Function->GetParameters().GetSize() == 0, false, Stage, Shader);
				}

				Shader += ")";

				ADD_NEW_LINE();

				Shader += "{";

				ADD_NEW_LINE();

				BuildType(ProgramDataTypes::Bool, Shader);
				Shader += String(" ") + GetReturnBoolName() + " = false;";

				ADD_NEW_LINE();

				BuildStatementHolder(Function, funcType, Stage, Shader);

				Shader += "}";

				ADD_NEW_LINE();
			}

			void ASTToGLSLCompiler::BuildConstantEntrypointAttributeType(ConstantEntrypointAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				//THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
			}

			void ASTToGLSLCompiler::BuildParameters(const ParameterList& Parameters, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				int32 i = 0;
				for (auto& parameter : Parameters)
				{
					if (i != 0)
						Shader += ", ";

					PushVariable(parameter);

					BuildFlattenParameters(parameter->GetDataType(), parameter->GetName(), i++ == 0, true, Stage, Shader);
				}
			}

			void ASTToGLSLCompiler::BuildStatementHolder(StatementItemHolder* Holder, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				IncreaseBlockIndex();

				ASTCompilerBase::BuildStatementHolder(Holder, Type, Stage, Shader);

				if (Type != FunctionType::Types::None && ContainsReturnStatement(Holder))
				{
					Shader += String("if (!") + GetReturnBoolName() + ")";

					ADD_NEW_LINE();

					Shader += "{";

					IncreamentOpenScopeCount();
				}

				DecreaseBlockIndex();
			}

			void ASTToGLSLCompiler::BuildVariableStatement(VariableStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				PushVariable(Statement);

				if (Statement->GetDataType()->IsBuiltIn())
				{
					ASTCompilerBase::BuildVariableStatement(Statement, Type, Stage, Shader);

					return;
				}

				const StructType* structType = GetStructType(Statement->GetDataType()->GetUserDefined());
				for (const auto& variable : structType->GetItems())
				{
					BuildDataTypeStatement(variable->GetDataType(), Shader);

					Shader += ' ';

					BuildFlattenStructMemberVariableName(structType, variable, Statement->GetName(), true, Shader);

					Shader += ';';

					ADD_NEW_LINE();
				}
			}

			void ASTToGLSLCompiler::BuildVariableAccessStatement(VariableAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				String name = Statement->GetName();

				if (Stage == Stages::Fragment)
				{
					//if (m_Outputs.Contains(name))
					//	name = m_Outputs[name];
				}

				Shader += name;
			}

			void ASTToGLSLCompiler::BuildMemberAccessStatement(MemberAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				String leftStm;
				BuildStatement(Statement->GetLeft(), Type, Stage, leftStm);

				const VariableType* leftVariable = FindVariableTypeInBlocks(leftStm);
				if (leftVariable != nullptr)
				{
					bool isParameter = IsAssignableFrom(leftVariable, const ParameterType);
					bool isOutputVariable = (IsAssignableFrom(leftVariable, const VariableStatement) && leftVariable->GetDataType()->GetUserDefined() == m_CurrentBuildingFunction->GetReturnDataType()->GetUserDefined());

					String name = String::Empty;
					if (isParameter)
						name = ReinterpretCast(const ParameterType*, leftVariable)->GetName();

					if (isParameter || isOutputVariable)
					{
						const StructType* structType = GetStructType(leftVariable->GetDataType()->GetUserDefined());

						bool isMemberAccess = IsAssignableFrom(Statement->GetRight(), MemberAccessStatement);
						auto* rightStatement = (isMemberAccess ? ReinterpretCast(MemberAccessStatement*, Statement->GetRight())->GetLeft() : Statement->GetRight());

						String rightStm;
						BuildStatement(rightStatement, Type, Stage, rightStm);
						rightStm = rightStm.Split('.')[0];

						const StructVariableType* rightVariable = GetVariableType(structType, rightStm);

						BuildFlattenStructMemberVariableName(structType, rightVariable, name, isParameter, Shader);

						if (isMemberAccess)
						{
							Shader += '.';

							BuildStatement(ReinterpretCast(MemberAccessStatement*, Statement->GetRight())->GetRight(), Type, Stage, Shader);
						}

						return;
					}
				}

				ASTCompilerBase::BuildMemberAccessStatement(Statement, Type, Stage, Shader);
			}

			void ASTToGLSLCompiler::BuildSwitchStatement(SwitchStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
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
								Shader += "if";
								isFirstOne = false;
							}
							else
								Shader += "else if";

							Shader += "(";
						}

						isCoupling = false;

						BuildStatement(Statement->GetSelector(), Type, Stage, Shader);

						Shader += " == ";

						CaseStatement* caseStatement = ReinterpretCast(CaseStatement*, statement);

						BuildStatement(caseStatement->GetCondition(), Type, Stage, Shader);

						if (caseStatement->GetItems().GetSize() == 0)
						{
							Shader += " || ";

							isCoupling = true;
							continue;
						}

						Shader += ")";
						ADD_NEW_LINE();
						Shader += "{";

						BuildStatementHolder(caseStatement, Type, Stage, Shader);

						Shader += "}";
					}
					else if (IsAssignableFrom(statement, DefaultStatement))
					{
						if (!isCoupling)
						{
							if (isFirstOne)
							{
								Shader += "if";
								isFirstOne = false;
							}
							else
								Shader += "else if";

							Shader += "(";
						}

						isCoupling = false;

						Shader += "true";

						DefaultStatement* defaultStatement = ReinterpretCast(DefaultStatement*, statement);

						if (defaultStatement->GetItems().GetSize() == 0)
						{
							Shader += " || ";

							isCoupling = true;
							continue;
						}

						Shader += ")";
						ADD_NEW_LINE();
						Shader += "{";

						BuildStatementHolder(defaultStatement, Type, Stage, Shader);

						Shader += "}";
					}
				}
			}

			void ASTToGLSLCompiler::BuildReturnStatement(ReturnStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				if (Type == FunctionType::Types::None)
					return;

				Shader += String(GetReturnBoolName()) + " = true;";

				ADD_NEW_LINE();

				const String& returnDataType = m_CurrentBuildingFunction->GetReturnDataType()->GetUserDefined();
				const StructType* structType = GetStructType(returnDataType);
				const StructVariableType* variableType = nullptr;

				if (Type == FunctionType::Types::VertexMain)
				{
					Shader += "gl_Position = ";

					variableType = FindVariableType(structType, [](const StructVariableType* item) { return item->GetRegister() == StructVariableType::Registers::Position; });
				}
				else if (Type == FunctionType::Types::FragmentMain)
				{
					variableType = FindVariableType(structType, [](const StructVariableType* item) { return item->GetRegister() == StructVariableType::Registers::Color; });

					uint8 elementCount = BuildReturnValue(Statement->GetStatement(), Type, Stage, Shader);
					for (uint8 i = 0; i < elementCount; ++i)
					{
						Shader += GetFragmentVariableName(i) + " = ";

						Shader += GetStageResultArrayVariableName();

						if (elementCount > 1)
						{
							Shader += '[';
							Shader += StringUtility::ToString<char8>(i);
							Shader += "]";
						}

						Shader += ";";

						ADD_NEW_LINE();
					}
				}

				if (variableType == nullptr)
					return;

				BuildFlattenStructMemberVariableName(structType, variableType, String::Empty, false, Shader);

				//Shader += "return ";

				//BuildStatement(Statement->GetStatement(), Type, Stage, Shader);

				Shader += ";";

				ADD_NEW_LINE();
			}

			void ASTToGLSLCompiler::BuildArrayStatement(ArrayStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				DataTypeStatement dataType = EvaluateDataType(Statement);

				BuildType(dataType.GetType(), Shader);

				Shader += "[](";

				BuildArguments(Statement, Type, Stage, Shader);

				Shader += ')';
			}

			void ASTToGLSLCompiler::BuildType(ProgramDataTypes Type, String& Shader)
			{
				switch (Type)
				{
				case ProgramDataTypes::Void:
					Shader += "void";
					break;

				case ProgramDataTypes::Bool:
					Shader += "bool";
					break;

				case ProgramDataTypes::Integer:
					Shader += "int";
					break;

				case ProgramDataTypes::UnsignedInteger:
					Shader += "uint";
					break;

				case ProgramDataTypes::Float:
					Shader += "float";
					break;

				case ProgramDataTypes::Double:
					Shader += "double";
					break;

				case ProgramDataTypes::Integer2:
					Shader += "ivec2";
					break;

				case ProgramDataTypes::UnsignedInteger2:
					Shader += "uvec2";
					break;

				case ProgramDataTypes::Float2:
					Shader += "vec2";
					break;

				case ProgramDataTypes::Double2:
					Shader += "dvec2";
					break;

				case ProgramDataTypes::Integer3:
					Shader += "ivec3";
					break;

				case ProgramDataTypes::UnsignedInteger3:
					Shader += "uvec3";
					break;

				case ProgramDataTypes::Float3:
					Shader += "vec3";
					break;

				case ProgramDataTypes::Double3:
					Shader += "dvec3";
					break;

				case ProgramDataTypes::Integer4:
					Shader += "ivec4";
					break;

				case ProgramDataTypes::UnsignedInteger4:
					Shader += "uvec4";
					break;

				case ProgramDataTypes::Float4:
					Shader += "vec4";
					break;

				case ProgramDataTypes::Double4:
					Shader += "dvec4";
					break;

				case ProgramDataTypes::Matrix4F:
				case ProgramDataTypes::Matrix4D:
					Shader += "mat4";
					break;

				case ProgramDataTypes::Texture2D:
					Shader += "sampler2D";
					break;
				}
			}

			void ASTToGLSLCompiler::BuildPostDataType(const DataTypeStatement* Type, Stages Stage, String& Shader)
			{
				if (Type->GetPostElementCount() != nullptr)
				{
					Shader += "[";
					BuildStatement(Type->GetPostElementCount(), FunctionType::Types::None, Stage, Shader);
					Shader += "]";
				}
			}

			void ASTToGLSLCompiler::BuildInOutStruct(const DataTypeStatement* DataType, const String& Name, bool IsInput, String& Shader)
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

					Shader += "layout(location=";
					Shader += StringUtility::ToString<char8>(location);
					Shader += ") ";
					Shader += (IsInput ? "in " : "out ");

					BuildDataTypeStatement(variable->GetDataType(), Shader);
					Shader += ' ';

					BuildFlattenStructMemberVariableName(structType, variable, Name, IsInput, Shader);
					Shader += ';';

					ADD_NEW_LINE();
				}
			}

			void ASTToGLSLCompiler::BuildUniformBlock(const StructType* Struct, const String& Name, Stages Stage, String& Shader)
			{
				auto variables = Struct->GetItems();

				if (FindVariableType(Struct, [](auto item) { return item->GetRegister() != StructVariableType::Registers::None; }) != nullptr)
					CoreDebugAssert(Categories::ProgramCompiler, false, "Struct %S cannot have variables with register specified", Struct->GetName());

				Shader += "layout(std140, binding=";
				Shader += StringUtility::ToString<char8>(m_BindingCount++);
				Shader += ") uniform " + Struct->GetName();
				Shader += "_";
				Shader += Name;
				ADD_NEW_LINE();
				Shader += "{";
				ADD_NEW_LINE();

				uint16 offset = 0;
				for (auto variable : variables)
				{
					ProgramDataTypes dataType = variable->GetDataType()->GetType();

					uint8 size = 0;
					StructType::GetAlignedOffset(dataType, offset, size);

					Shader += "layout(offset=";
					Shader += StringUtility::ToString<char8>(offset);
					Shader += ") ";

					BuildDataTypeStatement(variable->GetDataType(), Shader);

					Shader += " ";
					Shader += variable->GetName();
					Shader += ";";

					ADD_NEW_LINE();

					offset += size;
				}

				Shader += "}";

				Shader += Name;

				Shader += ";";

				ADD_NEW_LINE();
			}

			void ASTToGLSLCompiler::BuildFlattenParameters(const DataTypeStatement* DataType, const String& Name, bool IsFirst, bool IsInput, Stages Stage, String& Shader)
			{
				if (DataType->IsBuiltIn())
				{
					BuildDataTypeStatement(DataType, Shader);

					Shader += ' ';

					Shader += Name;

					BuildPostDataType(DataType, Stage, Shader);
				}
				else
				{
					const StructType* structType = GetStructType(DataType->GetUserDefined());
					for (auto& variable : structType->GetItems())
					{
						if (!IsFirst)
							Shader += ", ";

						BuildFlattenParameter(structType, variable, Name, IsInput, Stage, Shader);
					}
				}
			}

			void ASTToGLSLCompiler::BuildFlattenParameter(const StructType* Parent, const StructVariableType* Variable, const String& Name, bool IsInput, Stages Stage, String& Shader)
			{
				Shader += (IsInput ? "in " : "out ");

				BuildDataTypeStatement(Variable->GetDataType(), Shader);

				Shader += ' ';

				BuildFlattenParameterName(Parent, Variable, Name, IsInput, Stage, Shader);
			}

			void ASTToGLSLCompiler::BuildFlattenParameterName(const StructType* Parent, const StructVariableType* Variable, const String& Name, bool IsInput, Stages Stage, String& Shader)
			{
				BuildFlattenStructMemberVariableName(Parent, Variable, Name, IsInput, Shader);

				BuildPostDataType(Variable->GetDataType(), Stage, Shader);
			}

			void ASTToGLSLCompiler::BuildFlattenStructMemberVariableName(const StructType* Parent, const StructVariableType* Variable, const String& Name, bool IsInput, String& Shader)
			{
				Shader += Name;
				Shader += '_';
				Shader += (IsInput ? "In" : "Out");
				Shader += '_';
				Shader += Parent->GetName();
				Shader += '_';
				Shader += Variable->GetName();
			}

			const VariableType* ASTToGLSLCompiler::FindVariableTypeInBlocks(const String& Name)
			{
				for (int8 i = m_BlockVariables.GetSize() - 1; i >= 0; --i)
				{
					const auto& variables = m_BlockVariables[i];

					int32 index = variables.FindIf([&Name](auto item) { return item->GetName() == Name; });
					if (index == -1)
						continue;

					return variables[index];
				}

				return nullptr;
			}

			void ASTToGLSLCompiler::IncreaseBlockIndex(void)
			{
				CoreDebugAssert(Categories::ProgramCompiler, m_BlockIndex + 1 == m_BlockVariables.GetSize(), "Mismatch detected");

				++m_BlockIndex;

				m_BlockVariables.Add(VariableList(m_BlockVariables.GetAllocator()));
			}

			void ASTToGLSLCompiler::DecreaseBlockIndex(void)
			{
				CoreDebugAssert(Categories::ProgramCompiler, m_BlockIndex + 1 == m_BlockVariables.GetSize(), "Mismatch detected");

				--m_BlockIndex;

				m_BlockVariables.RemoveAt(m_BlockIndex);
			}

			void ASTToGLSLCompiler::PushVariable(VariableType* Variable)
			{
				m_BlockVariables[m_BlockIndex].Add(Variable);
			}

#undef ADD_NEW_LINE
		}
	}
}