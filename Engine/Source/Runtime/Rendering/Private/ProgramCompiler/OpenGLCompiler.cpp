// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ProgramCompiler\OpenGLCompiler.h>
#include <Rendering\Private\ProgramCompiler\Compiler.h>
#include <Rendering\Private\ProgramCompiler\ProgramParser.h>
#include <Rendering\Private\ProgramCompiler\ProgramCompilerException.h>
#include <Rendering\MeshInfo.h>

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
						registers["TEXCOORD"] = SubMesh::VertexLayouts::TexCoord;
					}

					if (registers.Contains(Name))
						return registers[Name];

					THROW_PROGRAM_COMPILER_EXCEPTION("Register not defined", Name);
				}

				String GetFragmentVariableName(uint8 Index)
				{
					return String(FRAGMENT_ENTRY_POINT_NAME) + "_FragColor" + StringUtility::ToString<char8>(Index);
				}

				cstr GetReturnBoolName(void)
				{
					static cstr name = "_MustReturn";
					return name;
				}

				OpenGLCompiler::OpenGLCompiler(AllocatorBase* Allocator) :
					APICompiler(Allocator, DeviceTypes::OpenGL),
					m_AdditionalLayoutCount(0),
					m_BindingCount(0),
					m_Parameters(GetAllocator())
				{
				}

				void OpenGLCompiler::Compile(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, CompileOutputInfo& Output)
				{
					m_Outputs.Clear();
					m_AdditionalLayoutCount = 0;
					m_BindingCount = 0;

					APICompiler::Compile(Structs, Variables, Functions, Output);
				}

				void OpenGLCompiler::ResetPerStageValues(Stages Stage)
				{
					APICompiler::ResetPerStageValues(Stage);

					m_Parameters.Clear();
					m_AdditionalLayoutCount = SubMeshInfo::GetLayoutCount();
					m_BindingCount = 0;
				}

				void OpenGLCompiler::BuildHeader(String& Shader)
				{
					Shader += "#version 460 core\n";
				}

				void OpenGLCompiler::BuildStruct(StructType* Struct, Stages Stage, String& Shader)
				{
					auto variables = Struct->GetItems();

					if (variables.ContainsIf([](auto item) { return item->GetRegister().GetLength() != 0; }))
					{
						if (variables.ContainsIf([](auto item) { return item->GetRegister().GetLength() == 0; }))
						{
							THROW_PROGRAM_COMPILER_EXCEPTION("Combination of input layout variables and user layout variables is forbidden", Struct->GetName());
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

				void OpenGLCompiler::BuildVariable(VariableType* Variable, Stages Stage, String& Shader)
				{
					BuildVariable(Variable->GetName(), Variable->GetRegister(), Variable->GetDataType(), false, Shader);
				}

				void OpenGLCompiler::BuildFunction(FunctionType* Function, Stages Stage, String& Shader)
				{
					FunctionType::Types funcType = Function->GetType();

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

							BuildDataTypeStatement(par->GetDataType(), Shader);
							Shader += " ";
							Shader += par->GetName();
						}
					}

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

				void OpenGLCompiler::BuildStatementHolder(StatementItemHolder* Holder, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					APICompiler::BuildStatementHolder(Holder, Type, Stage, Shader);

					if (Type != FunctionType::Types::None && ContainsReturnStatement(Holder))
					{
						Shader += String("if (!") + GetReturnBoolName() + ")";

						ADD_NEW_LINE();

						Shader += "{";

						IncreamentOpenScopeCount();
					}
				}

				void OpenGLCompiler::BuildVariableAccessStatement(VariableAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					String name = Statement->GetName();

					if (IntrinsicConstants::BuildIntrinsicConstantStatement(name, Type, Stage, Shader))
						return;

					if (Stage == Stages::Fragment)
					{
						if (m_Outputs.Contains(name))
							name = m_Outputs[name];
					}

					Shader += name;
				}

				void OpenGLCompiler::BuildMemberAccessStatement(MemberAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					String leftStm;
					BuildStatement(Statement->GetLeft(), Type, Stage, leftStm);

					int32 index = m_Parameters.FindIf([&leftStm](auto item) { return item->GetName() == leftStm; });
					if (index != -1)
					{
						BuildStatement(Statement->GetRight(), Type, Stage, Shader);

						return;
					}

					APICompiler::BuildMemberAccessStatement(Statement, Type, Stage, Shader);
				}

				void OpenGLCompiler::BuildIfStatement(IfStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					APICompiler::BuildIfStatement(Statement, Type, Stage, Shader);
				}

				void OpenGLCompiler::BuildReturnStatement(ReturnStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
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
				}

				void OpenGLCompiler::BuildArrayStatement(ArrayStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					DataTypeStatement dataType = EvaluateDataType(Statement);

					BuildType(dataType.GetType(), Shader);

					Shader += "[](";

					BuildArguments(Statement, Type, Stage, Shader);

					Shader += ')';
				}

				void OpenGLCompiler::BuildType(ProgramDataTypes Type, String& Shader)
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

				void OpenGLCompiler::BuildVariable(String Name, const String& Register, DataTypeStatement* DataType, bool IsOutputMode, String& Shader)
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

					if (DataType->GetUserDefined().GetLength() != 0)
					{
						int32 index = GetStructs().FindIf([&DataType](auto item) { return item->GetName() == DataType->GetUserDefined(); });
						if (index != -1)
						{
							BuildUniformBlock(GetStructs()[index], Name, Stages::Vertex, Shader);

							return;
						}
					}

					for (auto dataType : ALLOWED_CONTEXT_FREE_DATA_TYPES)
					{
						if (dataType != DataType->GetType())
							continue;

						if (dataType == ProgramDataTypes::Texture2D)
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

					BuildDataTypeStatement(DataType, Shader);
					Shader += " ";
					Shader += Name;
					Shader += ";";

					ADD_NEW_LINE();

					if (buildOutVarialbe)
						BuildVariable(Name, Register, DataType, true, Shader);
				}

				void OpenGLCompiler::BuildUniformBlock(const StructType* Struct, const String& Name, Stages Stage, String& Shader)
				{
					auto variables = Struct->GetItems();

					if (variables.ContainsIf([](auto item) { return item->GetRegister().GetLength() != 0; }))
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

						BuildVariable(variable, Stage, Shader);
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
}