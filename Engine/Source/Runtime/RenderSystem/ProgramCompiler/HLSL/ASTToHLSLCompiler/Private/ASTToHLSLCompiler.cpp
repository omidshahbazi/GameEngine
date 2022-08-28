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

				case StructVariableType::Registers::PrimitiveID:
					return "SV_PrimitiveID";

				case StructVariableType::Registers::TessellationFactor:
					return "SV_TessFactor";

				case StructVariableType::Registers::InsideTessellationFactor:
					return "SV_InsideTessFactor";

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

			String GetDomainType(DomainAttributeType::Types Type)
			{
				switch (Type)
				{
				case DomainAttributeType::Types::Triangle:
					return "tri";

				case DomainAttributeType::Types::Quad:
					return "quad";

				case DomainAttributeType::Types::Isoline:
					return "isoline";
				}

				THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
			}

			String GetPartitioningType(PartitioningAttributeType::Types Type)
			{
				switch (Type)
				{
				case PartitioningAttributeType::Types::Integer:
					return "integer";

				case PartitioningAttributeType::Types::FractionalEven:
					return "fractional_even";

				case PartitioningAttributeType::Types::FractionalOdd:
					return "fractional_odd";

				case PartitioningAttributeType::Types::PowerOfTwo:
					return "pow2";
				}

				THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
			}

			String GetTopologyType(TopologyAttributeType::Types Type)
			{
				switch (Type)
				{
				case TopologyAttributeType::Types::Point:
					return "point";

				case TopologyAttributeType::Types::Line:
					return "line";

				case TopologyAttributeType::Types::TriangleClockwise:
					return "triangle_cw";

				case TopologyAttributeType::Types::TriangleCounterClockwise:
					return "triangle_ccw";
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

				const String& name = Struct->GetName();

				Shader += name;

				ADD_NEW_LINE();
				Shader += "{";
				ADD_NEW_LINE();

				auto variables = Struct->GetItems();
				for (auto variable : variables)
				{
					DataTypeStatement* dataType = variable->GetDataType();

					BuildStructVariable(variable, Stage, Shader);

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
							Shader += name;
							Shader += "Padding";
							Shader += ";";

							ADD_NEW_LINE();
						}
					}
				}

				Shader += "};";
				ADD_NEW_LINE();
			}

			void ASTToHLSLCompiler::BuildStructVariable(StructVariableType* Variable, Stages Stage, String& Shader)
			{
				BuildDataTypeStatement(Variable->GetDataType(), Shader);

				Shader += " ";
				Shader += Variable->GetName();

				if (Variable->GetDataType()->GetPostElementCount() != nullptr)
				{
					Shader += "[";
					BuildStatement(Variable->GetDataType()->GetPostElementCount(), FunctionType::Types::None, Stage, Shader);
					Shader += "]";
				}

				if (Variable->GetRegister() != StructVariableType::Registers::None)
				{
					Shader += ":";
					Shader += GetRegisterName(Variable->GetRegister());
					Shader += StringUtility::ToString<char8>(Variable->GetRegisterIndex());
				}

				Shader += ";";

				ADD_NEW_LINE();
			}

			void ASTToHLSLCompiler::BuildGlobalVariable(GlobalVariableType* Variable, Stages Stage, String& Shader)
			{
				DataTypeStatement* dataType = Variable->GetDataType();

				if (dataType->IsBuiltIn())
				{
					for (auto allowedDataType : ALLOWED_CONTEXT_FREE_DATA_TYPES)
					{
						if (allowedDataType != dataType->GetType())
							continue;

						break;
					}
				}
				else
				{
					Shader += "ConstantBuffer<";
				}

				BuildDataTypeStatement(dataType, Shader);

				if (!dataType->IsBuiltIn())
					Shader += ">";

				Shader += " ";
				Shader += Variable->GetName();

				if (dataType->IsBuiltIn())
				{
					if (dataType->GetType() == ProgramDataTypes::Texture2D)
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

				Shader += ";";

				ADD_NEW_LINE();

				if (dataType->GetType() == ProgramDataTypes::Texture2D)
				{
					Shader += "SamplerState ";
					Shader += GetSamplerVariableName(Variable->GetName());
					Shader += ":register(s";
					Shader += StringUtility::ToString<char8>(m_TextureBindingCount);
					Shader += ");";

					ADD_NEW_LINE();

					++m_TextureBindingCount;
				}
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

				ASTCompilerBase::BuildAttributes(Function->GetAttributes(), funcType, Stage, Shader);

				BuildDataTypeStatement(Function->GetReturnDataType(), Shader);

				Shader += " ";

				if (Function->IsEntrypoint())
					Shader += Constants::ENTRY_POINT_NAME;
				else
					Shader += Function->GetName();

				Shader += "(";

				BuildParameters(Function->GetParameters(), funcType, Stage, Shader);

				Shader += ")";

				ADD_NEW_LINE();

				Shader += "{";

				ADD_NEW_LINE();

				BuildStatementHolder(Function, funcType, Stage, Shader);

				Shader += "}";

				ADD_NEW_LINE();
			}

			void ASTToHLSLCompiler::BuildDomainAttributeType(DomainAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				Shader += "[domain(\"";

				Shader += GetDomainType(Attribute->GetType());

				Shader += "\")]";
			}

			void ASTToHLSLCompiler::BuildPartitioningAttributeType(PartitioningAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				Shader += "[partitioning(\"";

				Shader += GetPartitioningType(Attribute->GetType());

				Shader += "\")]";
			}

			void ASTToHLSLCompiler::BuildTopologyAttributeType(TopologyAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				Shader += "[outputtopology(\"";

				Shader += GetTopologyType(Attribute->GetType());

				Shader += "\")]";
			}

			void ASTToHLSLCompiler::BuildControlPointsAttributeType(ControlPointsAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				Shader += "[outputcontrolpoints(";

				Shader += StringUtility::ToString<char8>(Attribute->GetNumber());

				Shader += ")]";
			}

			void ASTToHLSLCompiler::BuildConstantEntrypointAttributeType(ConstantEntrypointAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				Shader += "[patchconstantfunc(\"";

				Shader += Attribute->GetEntrypoint();

				Shader += "\")]";
			}

			void ASTToHLSLCompiler::BuildThreadCountAttributeType(ThreadCountAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)
			{
				Shader += "[numthreads(";

				Shader += StringUtility::ToString<char8>(Attribute->GetXCount());
				Shader += ", ";
				Shader += StringUtility::ToString<char8>(Attribute->GetXCount());
				Shader += ", ";
				Shader += StringUtility::ToString<char8>(Attribute->GetXCount());

				Shader += ")]";
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