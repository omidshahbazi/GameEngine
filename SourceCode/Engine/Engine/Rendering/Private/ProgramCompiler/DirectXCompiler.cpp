// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ProgramCompiler\DirectXCompiler.h>
#include <Rendering\Private\ProgramCompiler\Compiler.h>
#include <Rendering\Private\ProgramCompiler\ProgramParser.h>
#include <Rendering\MeshInfo.h>
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

				DirectXCompiler::DirectXCompiler(void) :
					APICompiler(DeviceTypes::DirectX12),
					m_InputAssemblerStruct(nullptr),
					m_LastFunction(nullptr),
					m_Add_SV_Position(false),
					m_BindingCount(0)
				{
				}

				bool DirectXCompiler::Compile(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, CompileOutputInfo& Output)
				{
					m_Functions = Functions;
					m_InputAssemblerStruct = nullptr;
					m_LastFunction = nullptr;
					m_Add_SV_Position = false;
					m_BindingCount = 0;

					if (!APICompiler::Compile(Structs, Variables, Functions, Output))
						return false;

					if (m_Add_SV_Position)
						Output.FragmentShader = "float4 dx_frag_coord:SV_Position;" + Output.FragmentShader;

					String rootSignature = "#define ";
					rootSignature += GetRootSignatureDefineName();
					rootSignature += " \"";

					rootSignature += "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)";

					uint8 slotIndex = 0;
					for (auto variableType : Variables)
					{
						const DataType& dataType = variableType->GetDataType();

						rootSignature += ",";

						if (dataType.GetType() == ProgramDataTypes::Unknown)
							rootSignature += "CBV(b";
						else if (dataType.GetType() == ProgramDataTypes::Texture2D)
							rootSignature += "DescriptorTable(SRV(t";

						rootSignature += StringUtility::ToString<char8>(slotIndex);
						rootSignature += ")";

						if (dataType.GetType() == ProgramDataTypes::Texture2D)
						{
							rootSignature += "),StaticSampler(s";
							rootSignature += StringUtility::ToString<char8>(slotIndex);
							rootSignature += ",filter=FILTER_MIN_MAG_MIP_POINT, addressU=TEXTURE_ADDRESS_BORDER, addressV=TEXTURE_ADDRESS_BORDER, addressW=TEXTURE_ADDRESS_BORDER, mipLODBias=0, maxAnisotropy=0, comparisonFunc=COMPARISON_NEVER, borderColor=STATIC_BORDER_COLOR_TRANSPARENT_BLACK, minLOD=0, maxLOD=1000)";
						}

						++slotIndex;
					}

					rootSignature += "\"\n";

					Output.VertexShader = rootSignature + Output.VertexShader;

					return true;
				}

				void DirectXCompiler::ResetPerStageValues(Stages Stage)
				{
					APICompiler::ResetPerStageValues(Stage);

					m_BindingCount = 0;
				}

				void DirectXCompiler::BuildStruct(StructType* Struct, Stages Stage, String& Shader)
				{
					BuildStruct(Struct, Stage, false, Shader);

					if (Struct->GetItems().ContainsIf([](auto item) {return item->GetRegister().GetLength() != 0; }))
					{
						m_InputAssemblerStruct = Struct;

						BuildStruct(Struct, Stage, true, Shader);
					}
				}

				void DirectXCompiler::BuildVariable(VariableType* Variable, Stages Stage, String& Shader)
				{
					BuildVariable(Variable->GetName(), Variable->GetRegister(), Variable->GetDataType(), Shader);
				}

				void DirectXCompiler::BuildFunction(FunctionType* Function, Stages Stage, String& Shader)
				{
					m_LastFunction = Function;

					FunctionType::Types funcType = Function->GetType();

					if (funcType == FunctionType::Types::VertexMain ||
						funcType == FunctionType::Types::ComputeMain)
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

						if (Stage != Stages::Fragment)
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

				void DirectXCompiler::BuildOperatorStatement(OperatorStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
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

					APICompiler::BuildOperatorStatement(Statement, Type, Stage, Shader);
				}

				void DirectXCompiler::BuildVariableAccessStatement(VariableAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
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

				void DirectXCompiler::BuildReturnStatement(ReturnStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					if (Type == FunctionType::Types::None)
					{
						Shader += "return ";
						BuildStatement(Statement->GetStatement(), Type, Stage, Shader);
					}
					else
					{
						for (uint8 i = 0; i < m_LastFunction->GetReturnDataType().GetElementCount(); ++i)
						{
							Shader += GetStageResultVariableName();
							Shader += ".";
							Shader += GetStageResultFieldName(i);
							Shader += "=";

							BuildStatement(Statement->GetStatement(), Type, Stage, Shader);

							Shader += ";";

							ADD_NEW_LINE();
						}

						Shader += "return ";
						Shader += GetStageResultVariableName();
					}
				}

				void DirectXCompiler::BuildArrayStatement(ArrayStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
				{
					//Assert(false, "Unsupported Location for Statement");
				}

				void DirectXCompiler::BuildDataType(const DataType& Type, String& Shader)
				{
					if (Type.IsBuiltIn())
						BuildType(Type.GetType(), Shader);
					else
						Shader += Type.GetUserDefined();
				}

				void DirectXCompiler::BuildType(ProgramDataTypes Type, String& Shader)
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

				void DirectXCompiler::BuildStruct(StructType* Struct, Stages Stage, bool IsOutputStruct, String& Shader)
				{
					Shader += "struct ";

					if (IsOutputStruct)
						Shader += GetOutputStructName();
					else
						Shader += Struct->GetName();

					ADD_NEW_LINE();
					Shader += "{";
					ADD_NEW_LINE();

					if (!IsOutputStruct || Stage != Stages::Fragment)
					{
						auto variables = Struct->GetItems();
						for (auto variable : variables)
						{
							const DataType& dataType = variable->GetDataType();

							BuildVariable(variable->GetName(), variable->GetRegister(), dataType, Shader);

							if (variable->GetRegister().GetLength() == 0)
							{
								uint8 size = 0;
								uint16 offset = 0;
								GetAlignedOffset(dataType.GetType(), offset, size);

								uint8 overflowByteCount = size % GPUAlignedVector4F::Alignment;
								if (overflowByteCount != 0)
								{
									Shader += "float";
									Shader += StringUtility::ToString<char8>((GPUAlignedVector4F::Alignment - overflowByteCount) / GPUAlignedFloat32::Size);
									Shader += " ";
									Shader += variable->GetName();
									Shader += "Padding";
									Shader += ";";

									ADD_NEW_LINE();
								}
							}
						}
					}

					if (IsOutputStruct)
					{
						ProgramDataTypes dataType = ProgramDataTypes::Unknown;
						String registerName;
						uint8 count = 1;

						switch (Stage)
						{
						case Stages::Vertex:
						{
							dataType = ProgramDataTypes::Float4;
							registerName = "SV_POSITION";
						} break;

						case Stages::Tessellation:
						{

						} break;

						case Stages::Geometry:
						{
						} break;

						case Stages::Fragment:
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

						case Stages::Compute:
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

				void DirectXCompiler::BuildVariable(const String& Name, const String& Register, const DataType& DataType, String& Shader)
				{
					if (DataType.IsBuiltIn())
					{
						for (auto allowedDataType : ALLOWED_CONTEXT_FREE_DATA_TYPES)
						{
							if (allowedDataType != DataType.GetType())
								continue;

							break;
						}
					}
					else
					{
						Shader += "ConstantBuffer<";
					}

					BuildDataType(DataType, Shader);

					if (!DataType.IsBuiltIn())
						Shader += ">";

					Shader += " ";
					Shader += Name;

					if (Register.GetLength() != 0)
					{
						Shader += ":";
						Shader += Register;
					}
					else
					{
						if (DataType.IsBuiltIn())
						{
							if (DataType.GetType() == ProgramDataTypes::Texture2D)
							{
								Shader += ":register(t";
								Shader += StringUtility::ToString<char8>(m_BindingCount++);
								Shader += ")";
							}
						}
						else
						{
							Shader += ":register(b";
							Shader += StringUtility::ToString<char8>(m_BindingCount++);
							Shader += ")";
						}
					}

					Shader += ";";

					ADD_NEW_LINE();

					if (DataType.GetType() == ProgramDataTypes::Texture2D)
					{
						Shader += "SamplerState ";
						Shader += GetSamplerVariableName(Name);
						Shader += ":register(s";
						Shader += StringUtility::ToString<char8>(m_BindingCount - 1);
						Shader += ");";

						ADD_NEW_LINE();
					}
				}

				String DirectXCompiler::GetOutputStructName(void) const
				{
					return m_InputAssemblerStruct->GetName() + "__Result__";
				}

				String DirectXCompiler::GetStageResultFieldName(uint8 Index)
				{
					return String("__Result__") + StringUtility::ToString<char8>(Index);
				}

				cstr DirectXCompiler::GetStageResultVariableName(void)
				{
					static cstr name = "__result__";

					return name;
				}

				cstr DirectXCompiler::GetRootSignatureDefineName(void)
				{
					static cstr name = "__RS__";

					return name;
				}

				String DirectXCompiler::GetSamplerVariableName(const String& TextureVariableName)
				{
					return TextureVariableName + "Sampler";
				}
			};

#undef ADD_NEW_LINE
		}
	}
}