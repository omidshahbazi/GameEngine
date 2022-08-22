// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ASTToHLSLCompiler\Private\ASTToHLSLCompiler.h>
#include <ProgramCompilerCommon\Common.h>
#include <ProgramCompilerCommon\ProgramCompilerException.h>
#include <RenderCommon\GPUAlignedType.h>

namespace Engine
{
	using namespace ProgramCompilerCommon;
	using namespace RenderCommon;

	namespace ASTToHLSLCompiler
	{
		namespace Private
		{
#ifdef DEBUG_MODE
#define ADD_NEW_LINE() Shader += "\n"
#define ADD_NEW_LINE_EX(StringVariable) StringVariable += "\n"
#else
#define ADD_NEW_LINE()
#define ADD_NEW_LINE_EX(StringVariable)
#endif

			String GetRegisterName(StructVariableType::Registers Register)
			{
				switch (Register)
				{
				case StructVariableType::Registers::Position:
					return "POSITION";

				case StructVariableType::Registers::Normal:
					return "NORMAL";

				case StructVariableType::Registers::Color:
					return "COLOR";

				case StructVariableType::Registers::UV:
					return "TEXCOORD";

				case StructVariableType::Registers::DomainLocation:
					return "SV_DomainLocation";

				case StructVariableType::Registers::InstanceID:
					return "SV_GSInstanceID";

				case StructVariableType::Registers::FragmentPosition:
					return "SV_Position";

				case StructVariableType::Registers::Target:
					return "SV_Target";

				case StructVariableType::Registers::DispatchThreadID:
					return "SV_DispatchThreadID";

				case StructVariableType::Registers::GroupID:
					return "SV_GroupID";

				case StructVariableType::Registers::GroupIndex:
					return "SV_GroupIndex";

				case StructVariableType::Registers::GroupThreadID:
					return "SV_GroupThreadID";
				}

				THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
			}

			ASTToHLSLCompiler::ASTToHLSLCompiler(void) :
				m_ConstantBufferBindingCount(0),
				m_TextureBindingCount(0)
			{
			}

			void ASTToHLSLCompiler::Initialize(DeviceTypes DeviceType)
			{
				ASTCompilerBase::Initialize(DeviceType);
			}

			void ASTToHLSLCompiler::Compile(AllocatorBase* Allocator, const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, OutputInfo& Output)
			{
				m_Functions = Functions;
				m_ConstantBufferBindingCount = 0;
				m_TextureBindingCount = 0;

				ASTCompilerBase::Compile(Allocator, Structs, Variables, Functions, Output);

				String rootSignature = "#define ";
				rootSignature += GetRootSignatureDefineName();
				rootSignature += " \"";

				rootSignature += "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)";

				uint8 cbvIndex = 0;
				uint8 textureIndex = 0;
				for (auto variableType : Variables)
				{
					const DataTypeStatement* dataType = variableType->GetDataType();

					rootSignature += ",";

					if (dataType->GetType() == ProgramDataTypes::Unknown)
					{
						rootSignature += "CBV(b";

						rootSignature += StringUtility::ToString<char8>(cbvIndex++);
					}
					else if (dataType->GetType() == ProgramDataTypes::Texture2D)
					{
						rootSignature += "DescriptorTable(SRV(t";
						rootSignature += StringUtility::ToString<char8>(textureIndex);
						rootSignature += ")),DescriptorTable(Sampler(s";
						rootSignature += StringUtility::ToString<char8>(textureIndex);
						rootSignature += ")";

						++textureIndex;
					}

					rootSignature += ")";
				}

				rootSignature += "\"\n";

				Output.VertexShader = rootSignature + Output.VertexShader;
			}

			void ASTToHLSLCompiler::ResetPerStageValues(Stages Stage)
			{
				ASTCompilerBase::ResetPerStageValues(Stage);

				m_ConstantBufferBindingCount = 0;
				m_TextureBindingCount = 0;
			}

			void ASTToHLSLCompiler::BuildStruct(StructType* Struct, Stages Stage, String& Shader)
			{
				Shader += "struct ";

				Shader += Struct->GetName();

				ADD_NEW_LINE();
				Shader += "{";
				ADD_NEW_LINE();

				auto variables = Struct->GetItems();
				for (auto variable : variables)
				{
					DataTypeStatement* dataType = variable->GetDataType();

					BuildVariable(variable->GetName(), variable->GetRegister(), variable->GetRegisterIndex(), dataType, Shader);

					if (variable->GetRegister() == StructVariableType::Registers::None)
					{
						uint8 size = 0;
						uint16 offset = 0;
						StructType::GetAlignedOffset(dataType->GetType(), offset, size);

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

				Shader += "};";
				ADD_NEW_LINE();
			}

			void ASTToHLSLCompiler::BuildVariable(VariableType* Variable, Stages Stage, String& Shader)
			{
				BuildVariable(Variable->GetName(), StructVariableType::Registers::None, 0, Variable->GetDataType(), Shader);
			}

			void ASTToHLSLCompiler::BuildFunction(FunctionType* Function, Stages Stage, String& Shader)
			{
				FunctionType::Types funcType = Function->GetType();

				if (funcType == FunctionType::Types::VertexMain ||
					funcType == FunctionType::Types::ComputeMain)
				{
					Shader += "[RootSignature(";
					Shader += GetRootSignatureDefineName();
					Shader += ")]";
					ADD_NEW_LINE();
				}

				BuildDataTypeStatement(Function->GetReturnDataType(), Shader);

				Shader += " ";

				if (Function->IsEntrypoint())
					Shader += Constants::ENTRY_POINT_NAME;
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

					BuildDataTypeStatement(par->GetDataType(), Shader);
					Shader += " ";
					Shader += par->GetName();

					parameter = par;
				}

				Shader += ")";

				ADD_NEW_LINE();

				Shader += "{";

				ADD_NEW_LINE();

				BuildStatementHolder(Function, funcType, Stage, Shader);

				Shader += "}";

				ADD_NEW_LINE();
			}

			void ASTToHLSLCompiler::BuildVariableAccessStatement(VariableAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				Shader += Statement->GetName();
			}

			void ASTToHLSLCompiler::BuildReturnStatement(ReturnStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				Shader += "return ";

				BuildStatement(Statement->GetStatement(), Type, Stage, Shader);

				Shader += ";";

				ADD_NEW_LINE();
			}

			void ASTToHLSLCompiler::BuildArrayStatement(ArrayStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				Shader += '{';

				BuildArguments(Statement, Type, Stage, Shader);

				Shader += '}';
			}

			void ASTToHLSLCompiler::BuildType(ProgramDataTypes Type, String& Shader)
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

			void ASTToHLSLCompiler::BuildVariable(const String& Name, StructVariableType::Registers Register, uint8 RegisterIndex, DataTypeStatement* DataType, String& Shader)
			{
				if (DataType->IsBuiltIn())
				{
					for (auto allowedDataType : ALLOWED_CONTEXT_FREE_DATA_TYPES)
					{
						if (allowedDataType != DataType->GetType())
							continue;

						break;
					}
				}
				else
				{
					Shader += "ConstantBuffer<";
				}

				BuildDataTypeStatement(DataType, Shader);

				if (!DataType->IsBuiltIn())
					Shader += ">";

				Shader += " ";
				Shader += Name;

				if (Register == StructVariableType::Registers::None)
				{
					if (DataType->IsBuiltIn())
					{
						if (DataType->GetType() == ProgramDataTypes::Texture2D)
						{
							Shader += ":register(t";
							Shader += StringUtility::ToString<char8>(m_TextureBindingCount);
							Shader += ")";
						}
					}
					else
					{
						Shader += ":register(b";
						Shader += StringUtility::ToString<char8>(m_ConstantBufferBindingCount++);
						Shader += ")";
					}
				}
				else
				{
					Shader += ":";
					Shader += GetRegisterName(Register);
					Shader += StringUtility::ToString<char8>(RegisterIndex);
				}

				Shader += ";";

				ADD_NEW_LINE();

				if (DataType->GetType() == ProgramDataTypes::Texture2D)
				{
					Shader += "SamplerState ";
					Shader += GetSamplerVariableName(Name);
					Shader += ":register(s";
					Shader += StringUtility::ToString<char8>(m_TextureBindingCount);
					Shader += ");";

					ADD_NEW_LINE();

					++m_TextureBindingCount;
				}
			}

			cstr ASTToHLSLCompiler::GetRootSignatureDefineName(void)
			{
				static cstr name = "__RS__";

				return name;
			}

			String ASTToHLSLCompiler::GetSamplerVariableName(const String& TextureVariableName)
			{
				return TextureVariableName + "Sampler";
			}
		};

#undef ADD_NEW_LINE
	}
}