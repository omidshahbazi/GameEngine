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

			VertexLayouts GetLayout(const String& Name)
			{
				static bool initialized = false;
				static Map<String, VertexLayouts> registers;

				if (!initialized)
				{
					initialized = true;

					registers["POSITION"] = VertexLayouts::Position;
					registers["NORMAL"] = VertexLayouts::Normal;
					registers["TEXCOORD"] = VertexLayouts::TexCoord;
				}

				if (registers.Contains(Name))
					return registers[Name];

				THROW_PROGRAM_COMPILER_EXCEPTION("Register not defined", Name);
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
				m_BindingCount(0)
			{
			}

			void ASTToGLSLCompiler::Initialize(DeviceTypes DeviceType)
			{
				ASTCompilerBase::Initialize(DeviceType);
			}

			void ASTToGLSLCompiler::Compile(AllocatorBase* Allocator, const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, OutputInfo& Output)
			{
				m_Outputs = OutputMap(Allocator);
				m_AdditionalLayoutCount = 0;
				m_BindingCount = 0;
				m_Parameters = ParameterList(Allocator);

				try
				{
					ASTCompilerBase::Compile(Allocator, Structs, Variables, Functions, Output);
				}
				catch (...)
				{
					m_Outputs = OutputMap();
					m_Parameters = ParameterList();

					throw;
				}

				m_Outputs = OutputMap();
				m_Parameters = ParameterList();
			}

			void ASTToGLSLCompiler::ResetPerStageValues(Stages Stage)
			{
				ASTCompilerBase::ResetPerStageValues(Stage);

				m_Parameters.Clear();
				m_AdditionalLayoutCount = SubMeshInfo::GetLayoutCount();
				m_BindingCount = 0;
			}

			void ASTToGLSLCompiler::BuildHeader(String& Shader)
			{
				Shader += "#version 460 core\n";
			}

			void ASTToGLSLCompiler::BuildStruct(StructType* Struct, Stages Stage, String& Shader)
			{
				auto variables = Struct->GetItems();

				if (!variables.ContainsIf([](auto item) { return item->GetRegister() != StructVariableType::Registers::None; }))
					return;

				for (auto variable : variables)
				{
					BuildStructVariable(variable, Stage, Shader);
				}
			}

			void ASTToGLSLCompiler::BuildStructVariable(StructVariableType* Variable, Stages Stage, String& Shader)
			{
				String name = Variable->GetName();
				DataTypeStatement* dataType = Variable->GetDataType();

				if (Variable->GetRegister() != StructVariableType::Registers::None)
				{
					int8 location = 0;

					if (m_Outputs.Contains(name))
					{
						name = m_Outputs[name];

						location = m_AdditionalLayoutCount++;
					}

					Shader += "layout(location=";
					Shader += StringUtility::ToString<char8>(location);
					Shader += ")";
					//Shader += (IsOutputMode ? "out " : "in ");
					Shader += "in ";
				}

				if (dataType->GetUserDefined() != String::Empty)
				{
					int32 index = GetStructs().FindIf([dataType](auto item) { return item->GetName() == dataType->GetUserDefined(); });
					if (index != -1)
					{
						BuildUniformBlock(GetStructs()[index], name, Stages::Vertex, Shader);

						return;
					}
				}

				for (auto allowedDataType : ALLOWED_CONTEXT_FREE_DATA_TYPES)
				{
					if (allowedDataType == dataType->GetType())
						continue;

					Shader += "uniform ";
					break;
				}

				BuildDataTypeStatement(dataType, Shader);
				Shader += " ";
				Shader += name;
				Shader += ";";

				ADD_NEW_LINE();
			}

			void ASTToGLSLCompiler::BuildGlobalVariable(GlobalVariableType* Variable, Stages Stage, String& Shader)
			{
				const String& name = Variable->GetName();
				DataTypeStatement* dataType = Variable->GetDataType();

				if (dataType->GetUserDefined() != String::Empty)
				{
					int32 index = GetStructs().FindIf([dataType](auto item) { return item->GetName() == dataType->GetUserDefined(); });
					if (index != -1)
					{
						BuildUniformBlock(GetStructs()[index], name, Stages::Vertex, Shader);

						return;
					}
				}

				for (auto allowedDataType : ALLOWED_CONTEXT_FREE_DATA_TYPES)
				{
					if (allowedDataType != dataType->GetType())
						continue;

					if (allowedDataType == ProgramDataTypes::Texture2D)
					{
						Shader += "layout(location=";
						Shader += StringUtility::ToString<char8>(m_BindingCount);
						Shader += ",binding=";
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
				FunctionType::Types funcType = Function->GetType();

				ASTCompilerBase::BuildAttributes(Function->GetAttributes(), funcType, Stage, Shader);

				if (funcType == FunctionType::Types::FragmentMain)
				{
					uint8 elementCount = EvaluateDataTypeElementCount(Function->GetReturnDataType());

					for (uint8 i = 0; i < elementCount; ++i)
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
					BuildDataTypeStatement(Function->GetReturnDataType(), Shader);

				Shader += " ";

				if (Function->IsEntrypoint())
					Shader += Constants::ENTRY_POINT_NAME;
				else
					Shader += Function->GetName();

				Shader += "(";

				if (Function->IsEntrypoint())
					m_Parameters.AddRange(Function->GetParameters());
				else
					BuildParameters(Function->GetParameters(), funcType, Stage, Shader);

				Shader += ")";

				ADD_NEW_LINE();

				Shader += "{";

				ADD_NEW_LINE();

				BuildType(ProgramDataTypes::Bool, Shader);
				Shader += String(" ") + GetReturnBoolName() + "=false;";

				ADD_NEW_LINE();

				BuildStatementHolder(Function, funcType, Stage, Shader);

				Shader += "}";

				ADD_NEW_LINE();
			}

			void ASTToGLSLCompiler::BuildDomainAttributeType(DomainAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)
			{
			}

			void ASTToGLSLCompiler::BuildPartitioningAttributeType(PartitioningAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)
			{
			}

			void ASTToGLSLCompiler::BuildTopologyAttributeType(TopologyAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)
			{
			}

			void ASTToGLSLCompiler::BuildControlPointsAttributeType(ControlPointsAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)
			{
			}

			void ASTToGLSLCompiler::BuildConstantEntrypointAttributeType(ConstantEntrypointAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)
			{
			}

			void ASTToGLSLCompiler::BuildThreadCountAttributeType(ThreadCountAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)
			{
			}

			void ASTToGLSLCompiler::BuildStatementHolder(StatementItemHolder* Holder, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				ASTCompilerBase::BuildStatementHolder(Holder, Type, Stage, Shader);

				if (Type != FunctionType::Types::None && ContainsReturnStatement(Holder))
				{
					Shader += String("if (!") + GetReturnBoolName() + ")";

					ADD_NEW_LINE();

					Shader += "{";

					IncreamentOpenScopeCount();
				}
			}

			void ASTToGLSLCompiler::BuildVariableAccessStatement(VariableAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				String name = Statement->GetName();

				if (Stage == Stages::Fragment)
				{
					if (m_Outputs.Contains(name))
						name = m_Outputs[name];
				}

				Shader += name;
			}

			void ASTToGLSLCompiler::BuildMemberAccessStatement(MemberAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				String leftStm;
				BuildStatement(Statement->GetLeft(), Type, Stage, leftStm);

				int32 index = m_Parameters.FindIf([&leftStm](auto item) { return item->GetName() == leftStm; });
				if (index == -1)
					ASTCompilerBase::BuildMemberAccessStatement(Statement, Type, Stage, Shader);
				else
					BuildStatement(Statement->GetRight(), Type, Stage, Shader);
			}

			void ASTToGLSLCompiler::BuildIfStatement(IfStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				ASTCompilerBase::BuildIfStatement(Statement, Type, Stage, Shader);
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
					uint8 elementCount = BuildReturnValue(Statement->GetStatement(), Type, Stage, Shader);

					for (uint8 i = 0; i < elementCount; ++i)
					{
						Shader += GetFragmentVariableName(i) + "=";

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

					return;
				}
				else
					Shader += "return ";

				BuildStatement(Statement->GetStatement(), Type, Stage, Shader);

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

			void ASTToGLSLCompiler::BuildUniformBlock(const StructType* Struct, const String& Name, Stages Stage, String& Shader)
			{
				auto variables = Struct->GetItems();

				if (variables.ContainsIf([](auto item) { return item->GetRegister() != StructVariableType::Registers::None; }))
					THROW_PROGRAM_COMPILER_EXCEPTION("Cannot compile an struct with input layout in GLSL", Struct->GetName());

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

					offset += size;

					BuildStructVariable(variable, Stage, Shader);
				}

				Shader += "}";

				Shader += Name;

				Shader += ";";

				ADD_NEW_LINE();
			}

#undef ADD_NEW_LINE
		}
	}
}