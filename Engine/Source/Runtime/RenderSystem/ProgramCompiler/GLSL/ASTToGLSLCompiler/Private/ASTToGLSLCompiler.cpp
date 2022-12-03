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

			void ASTToGLSLCompiler::ResetPerStageValues(StageData& Data)
			{
				ASTCompilerBase::ResetPerStageValues(Data);

				m_AdditionalLayoutCount = SubMeshInfo::GetLayoutCount();
				m_BindingCount = 0;
			}

			void ASTToGLSLCompiler::BuildStageShader(StageData& Data)
			{
				ASTCompilerBase::BuildStageShader(Data);

				Data.Shader = "#version 460 core\n" + Data.Shader;
			}

			void ASTToGLSLCompiler::BuildStruct(StructType* Struct, StageData& Data)
			{
				AddCode("struct ", Data);
				AddCode(Struct->GetName(), Data);
				AddNewLine(Data);

				AddCode('{', Data);
				AddNewLine(Data);

				IncreaseBlockIndex();

				for (auto& variable : Struct->GetItems())
				{
					BuildDataTypeStatement(variable->GetDataType(), Data);

					AddCode(' ', Data);

					AddCode(variable->GetName(), Data);

					BuildPostDataType(variable->GetDataType(), Data);

					AddCode(';', Data);

					AddNewLine(Data);
				}

				DecreaseBlockIndex();

				AddCode("};", Data);
				AddNewLine(Data);
			}

			void ASTToGLSLCompiler::BuildGlobalVariable(GlobalVariableType* Variable, StageData& Data)
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
						AddCode("layout(location = ", Data);
						AddCode(StringUtility::ToString<char8>(m_BindingCount), Data);
						AddCode(", binding = ", Data);
						AddCode(StringUtility::ToString<char8>(m_BindingCount), Data);
						AddCode(')', Data);

						++m_BindingCount;
					}

					AddCode("uniform ", Data);
					break;
				}

				BuildDataTypeStatement(dataType, Data);
				AddCode(' ', Data);
				AddCode(name, Data);
				AddCode(';', Data);

				AddNewLine(Data);
			}

			void ASTToGLSLCompiler::BuildFunction(FunctionType* Function, StageData& Data)
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
						AddCode("layout(location = ", Data);
						AddCode(StringUtility::ToString<char8>(i), Data);
						AddCode(") out ", Data);
						BuildType(ProgramDataTypes::Float4, Data);
						AddCode(' ', Data);
						AddCode(GetFragmentVariableName(i), Data);
						AddCode(';', Data);

						AddNewLine(Data);
					}
				}

				if (Function->IsEntrypoint())
					BuildType(ProgramDataTypes::Void, Data);
				else
					BuildDataTypeStatement(Function->GetReturnDataType(), Data);

				AddCode(' ', Data);

				if (Function->IsEntrypoint())
					AddCode(Constants::ENTRY_POINT_NAME, Data);
				else
					AddCode(Function->GetName(), Data);

				IncreaseBlockIndex();

				AddCode('(', Data);

				if (Function->IsEntrypoint())
				{
					for (auto& parameter : Function->GetParameters())
						PushVariable(parameter);
				}
				else
					BuildParameters(Function->GetParameters(), Data);

				AddCode(')', Data);
				AddNewLine(Data);

				--Data.IndentOffset;
				AddCode('{', Data);
				++Data.IndentOffset;

				AddNewLine(Data);

				BuildStatementHolder(Function, false, Data);

				--Data.IndentOffset;
				AddCode('}', Data);
				++Data.IndentOffset;

				AddNewLine(Data);

				DecreaseBlockIndex();
			}

			void ASTToGLSLCompiler::BuildConstantEntrypointAttributeType(ConstantEntrypointAttributeType* Attribute, StageData& Data)
			{
				//THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
			}

			void ASTToGLSLCompiler::BuildVariableAccessStatement(VariableAccessStatement* Statement, StageData& Data)
			{
				AddCode(Statement->GetName(), Data);
			}

			void ASTToGLSLCompiler::BuildMemberAccessStatement(MemberAccessStatement* Statement, StageData& Data)
			{
				if (Data.FunctionType != FunctionType::Types::None)
				{
					String leftStm;
					StageData data = { Data.FunctionType, Data.Stage, Data.Structs, Data.Variables, Data.Functions, leftStm, 0 };
					BuildStatement(Statement->GetLeft(), data);

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
							StageData data = { Data.FunctionType, Data.Stage, Data.Structs, Data.Variables, Data.Functions, rightStm, 0 };
							BuildStatement(rightStatement, data);
							rightStm = rightStm.Split('.')[0];

							const StructVariableType* rightVariable = GetVariableType(structType, rightStm);

							BuildFlattenStructMemberVariableName(structType, rightVariable, name, isParameter, Data);

							if (isMemberAccess)
							{
								AddCode('.', Data);

								BuildStatement(ReinterpretCast(MemberAccessStatement*, Statement->GetRight())->GetRight(), Data);
							}

							return;
						}
					}
				}

				ASTCompilerBase::BuildMemberAccessStatement(Statement, Data);
			}

			void ASTToGLSLCompiler::BuildReturnStatement(ReturnStatement* Statement, StageData& Data)
			{
				if (Data.FunctionType == FunctionType::Types::None)
				{
					AddCode("return ", Data);

					BuildStatement(Statement->GetStatement(), Data);

					AddCode(';', Data);
					AddNewLine(Data);

					return;
				}

				const String& returnDataType = GetLastFunction()->GetReturnDataType()->GetUserDefined();
				const StructType* structType = GetStructType(returnDataType);
				const StructVariableType* variableType = nullptr;

				if (Data.FunctionType == FunctionType::Types::VertexMain)
				{
					AddCode("gl_Position = ", Data);

					variableType = FindVariableType(structType, [](const StructVariableType* item) { return item->GetRegister() == StructVariableType::Registers::Position; });
				}
				else if (Data.FunctionType == FunctionType::Types::FragmentMain)
				{
					variableType = FindVariableType(structType, [](const StructVariableType* item) { return item->GetRegister() == StructVariableType::Registers::Color; });

					uint8 elementCount = BuildReturnValue(Statement->GetStatement(), Data);
					for (uint8 i = 0; i < elementCount; ++i)
					{
						AddCode(GetFragmentVariableName(i) + " = ", Data);

						AddCode(GetStageResultArrayVariableName(), Data);

						if (elementCount > 1)
						{
							AddCode('[', Data);
							AddCode(StringUtility::ToString<char8>(i), Data);
							AddCode(']', Data);
						}

						AddCode(';', Data);
						AddNewLine(Data);
					}
				}

				if (variableType == nullptr)
					return;

				BuildFlattenStructMemberVariableName(structType, variableType, String::Empty, false, Data);

				AddCode(';', Data);
				AddNewLine(Data);

				AddCode("return;", Data);
				AddNewLine(Data);
			}

			void ASTToGLSLCompiler::BuildArrayStatement(ArrayStatement* Statement, StageData& Data)
			{
				DataTypeStatement dataType = EvaluateDataType(Statement);

				BuildType(dataType.GetType(), Data);

				AddCode("[](", Data);

				BuildArguments(Statement, Data);

				AddCode(')', Data);
			}

			void ASTToGLSLCompiler::BuildExplicitCast(Statement* Statement, const DataTypeStatement* DataType, StageData& Data)
			{
				bool needsCasting = !CompareDataTypes(EvaluateDataType(Statement), *DataType);

				if (needsCasting)
				{
					BuildDataTypeStatement(DataType, Data);

					AddCode('(', Data);
				}

				BuildStatement(Statement, Data);

				if (needsCasting)
					AddCode(')', Data);
			}

			void ASTToGLSLCompiler::BuildType(ProgramDataTypes Type, StageData& Data)
			{
				switch (Type)
				{
				case ProgramDataTypes::Void:
					AddCode("void", Data);
					break;

				case ProgramDataTypes::Bool:
					AddCode("bool", Data);
					break;

				case ProgramDataTypes::Integer:
					AddCode("int", Data);
					break;

				case ProgramDataTypes::UnsignedInteger:
					AddCode("uint", Data);
					break;

				case ProgramDataTypes::Float:
					AddCode("float", Data);
					break;

				case ProgramDataTypes::Double:
					AddCode("double", Data);
					break;

				case ProgramDataTypes::Integer2:
					AddCode("ivec2", Data);
					break;

				case ProgramDataTypes::UnsignedInteger2:
					AddCode("uvec2", Data);
					break;

				case ProgramDataTypes::Float2:
					AddCode("vec2", Data);
					break;

				case ProgramDataTypes::Double2:
					AddCode("dvec2", Data);
					break;

				case ProgramDataTypes::Integer3:
					AddCode("ivec3", Data);
					break;

				case ProgramDataTypes::UnsignedInteger3:
					AddCode("uvec3", Data);
					break;

				case ProgramDataTypes::Float3:
					AddCode("vec3", Data);
					break;

				case ProgramDataTypes::Double3:
					AddCode("dvec3", Data);
					break;

				case ProgramDataTypes::Integer4:
					AddCode("ivec4", Data);
					break;

				case ProgramDataTypes::UnsignedInteger4:
					AddCode("uvec4", Data);
					break;

				case ProgramDataTypes::Float4:
					AddCode("vec4", Data);
					break;

				case ProgramDataTypes::Double4:
					AddCode("dvec4", Data);
					break;

				case ProgramDataTypes::Matrix4F:
				case ProgramDataTypes::Matrix4D:
					AddCode("mat4", Data);
					break;

				case ProgramDataTypes::Texture2D:
					AddCode("sampler2D", Data);
					break;
				}
			}

			void ASTToGLSLCompiler::BuildPostDataType(const DataTypeStatement* Type, StageData& Data)
			{
				if (Type->GetPostElementCount() != nullptr)
				{
					AddCode('[', Data);
					BuildStatement(Type->GetPostElementCount(), Data);
					AddCode(']', Data);
				}
			}

			void ASTToGLSLCompiler::BuildInOutStruct(const DataTypeStatement* DataType, const String& Name, bool IsInput, StageData& Data)
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

					AddCode("layout(location = ", Data);
					AddCode(StringUtility::ToString<char8>(location), Data);
					AddCode(") ", Data);
					AddCode((IsInput ? "in " : "out "), Data);

					BuildDataTypeStatement(variable->GetDataType(), Data);
					AddCode(' ', Data);

					BuildFlattenStructMemberVariableName(structType, variable, Name, IsInput, Data);
					AddCode(';', Data);

					AddNewLine(Data);
				}
			}

			void ASTToGLSLCompiler::BuildUniformBlock(const StructType* Struct, const String& Name, StageData& Data)
			{
				auto variables = Struct->GetItems();

				if (FindVariableType(Struct, [](auto item) { return item->GetRegister() != StructVariableType::Registers::None; }) != nullptr)
					CoreDebugAssert(Categories::ProgramCompiler, false, "Struct %S cannot have variables with register specified", Struct->GetName());

				AddCode("layout(std140, binding = ", Data);
				AddCode(StringUtility::ToString<char8>(m_BindingCount++), Data);
				AddCode(") uniform " + Struct->GetName(), Data);
				AddCode('_', Data);
				AddCode(Name, Data);
				AddNewLine(Data);
				AddCode('{', Data);
				AddNewLine(Data);

				IncreaseBlockIndex();

				uint16 offset = 0;
				for (auto variable : variables)
				{
					ProgramDataTypes dataType = variable->GetDataType()->GetType();

					uint8 size = 0;
					StructType::GetAlignedOffset(dataType, offset, size);

					AddCode("layout(offset = ", Data);
					AddCode(StringUtility::ToString<char8>(offset), Data);
					AddCode(") ", Data);

					BuildDataTypeStatement(variable->GetDataType(), Data);

					AddCode(' ', Data);
					AddCode(variable->GetName(), Data);
					AddCode(';', Data);

					AddNewLine(Data);

					offset += size;
				}

				DecreaseBlockIndex();

				AddCode("} ", Data);

				AddCode(Name, Data);

				AddCode(';', Data);

				AddNewLine(Data);
			}

			void ASTToGLSLCompiler::BuildFlattenStructMemberVariableName(const StructType* Parent, const StructVariableType* Variable, const String& Name, bool IsInput, StageData& Data)
			{
				AddCode(Name, Data);
				AddCode('_', Data);
				AddCode((IsInput ? "In" : "Out"), Data);
				AddCode('_', Data);
				AddCode(Parent->GetName(), Data);
				AddCode('_', Data);
				AddCode(Variable->GetName(), Data);
			}
		}
	}
}