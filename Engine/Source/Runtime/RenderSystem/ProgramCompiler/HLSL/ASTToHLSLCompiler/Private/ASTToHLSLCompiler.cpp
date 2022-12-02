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
#define ADD_NEW_LINE() Data.Shader += "\n"
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

			String GetPrimitiveType(PrimitiveTypeAttributeType::Types Type)
			{
				switch (Type)
				{
				case PrimitiveTypeAttributeType::Types::Point:
					return "point";

				case PrimitiveTypeAttributeType::Types::Line:
					return "line";

				case PrimitiveTypeAttributeType::Types::Triangle:
					return "triangle";

				case PrimitiveTypeAttributeType::Types::LineAdjacency:
					return "lineadj";

				case PrimitiveTypeAttributeType::Types::TriangleAdjacency:
					return "triangleadj";
				}

				THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
			}

			String GetOutputStreamType(OutputStreamTypeAttributeType::Types Type)
			{
				switch (Type)
				{
				case OutputStreamTypeAttributeType::Types::Point:
					return "PointStream";

				case OutputStreamTypeAttributeType::Types::Line:
					return "LineStream";

				case OutputStreamTypeAttributeType::Types::Triangle:
					return "TriangleStream";
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
			}

			void ASTToHLSLCompiler::ResetPerStageValues(const StageData& Data)
			{
				ASTCompilerBase::ResetPerStageValues(Data);

				m_ConstantBufferBindingCount = 0;
				m_TextureBindingCount = 0;
			}

			void ASTToHLSLCompiler::BuildStageShader(const StageData& Data)
			{
				ASTCompilerBase::BuildStageShader(Data);

				if (Data.Stage != Stages::Vertex &&
					Data.Stage != Stages::Compute)
					return;

				String rootSignature;
				BuildRootSignature({ Data.FunctionType, Data.Stage, Data.Structs, Data.Variables, Data.Functions, rootSignature });

				Data.Shader = rootSignature + Data.Shader;
			}

			void ASTToHLSLCompiler::BuildStruct(StructType* Struct, const StageData& Data)
			{
				Data.Shader += "struct ";

				const String& name = Struct->GetName();

				Data.Shader += name;

				ADD_NEW_LINE();
				Data.Shader += "{";
				ADD_NEW_LINE();

				auto variables = Struct->GetItems();
				for (auto variable : variables)
				{
					DataTypeStatement* dataType = variable->GetDataType();

					BuildStructVariable(variable, Data);

					if (variable->GetRegister() == StructVariableType::Registers::None)
					{
						uint8 size = 0;
						uint16 offset = 0;
						StructType::GetAlignedOffset(dataType->GetType(), offset, size);

						uint8 overflowByteCount = size % GPUAlignedVector4F::Alignment;
						if (overflowByteCount != 0)
						{
							Data.Shader += "float";
							Data.Shader += StringUtility::ToString<char8>((GPUAlignedVector4F::Alignment - overflowByteCount) / GPUAlignedFloat32::Size);
							Data.Shader += " ";
							Data.Shader += name;
							Data.Shader += "Padding";
							Data.Shader += ";";

							ADD_NEW_LINE();
						}
					}
				}

				Data.Shader += "};";
				ADD_NEW_LINE();
			}

			void ASTToHLSLCompiler::BuildGlobalVariable(GlobalVariableType* Variable, const StageData& Data)
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
					Data.Shader += "ConstantBuffer<";
				}

				BuildDataTypeStatement(dataType, Data);

				if (!dataType->IsBuiltIn())
					Data.Shader += ">";

				Data.Shader += " ";
				Data.Shader += Variable->GetName();

				if (dataType->IsBuiltIn())
				{
					if (dataType->GetType() == ProgramDataTypes::Texture2D)
					{
						Data.Shader += ":register(t";
						Data.Shader += StringUtility::ToString<char8>(m_TextureBindingCount);
						Data.Shader += ")";
					}
				}
				else
				{
					Data.Shader += ":register(b";
					Data.Shader += StringUtility::ToString<char8>(m_ConstantBufferBindingCount++);
					Data.Shader += ")";
				}

				Data.Shader += ";";

				ADD_NEW_LINE();

				if (dataType->GetType() == ProgramDataTypes::Texture2D)
				{
					Data.Shader += "SamplerState ";
					Data.Shader += GetSamplerVariableName(Variable->GetName());
					Data.Shader += ":register(s";
					Data.Shader += StringUtility::ToString<char8>(m_TextureBindingCount);
					Data.Shader += ");";

					ADD_NEW_LINE();

					++m_TextureBindingCount;
				}
			}

			void ASTToHLSLCompiler::BuildFunction(FunctionType* Function, const StageData& Data)
			{
				FunctionType::Types funcType = Function->GetType();

				if (funcType == FunctionType::Types::VertexMain ||
					funcType == FunctionType::Types::ComputeMain)
				{
					Data.Shader += "[RootSignature(";
					Data.Shader += GetRootSignatureDefineName();
					Data.Shader += ")]";
					ADD_NEW_LINE();
				}

				BuildAttributes(Function->GetAttributes(), Data);

				BuildDataTypeStatement(Function->GetReturnDataType(), Data);

				Data.Shader += " ";

				if (Function->IsEntrypoint())
					Data.Shader += Constants::ENTRY_POINT_NAME;
				else
					Data.Shader += Function->GetName();

				Data.Shader += "(";

				if (funcType == FunctionType::Types::GeometryMain)
				{
					const auto* outputType = Function->GetAttribute<OutputStreamTypeAttributeType>();

					Data.Shader += "inout ";
					Data.Shader += GetOutputStreamType(outputType->GetType());
					Data.Shader += "<";
					Data.Shader += outputType->GetDataType();
					Data.Shader += "> ";
					Data.Shader += GetGeometryOutputStreamParameterName();

					Data.Shader += ", ";

					const auto* primitiveTypeAttr = Function->GetAttribute<PrimitiveTypeAttributeType>();

					Data.Shader += GetPrimitiveType(primitiveTypeAttr->GetType());
					Data.Shader += " ";
				}

				BuildParameters(Function->GetParameters(), Data);

				Data.Shader += ")";

				ADD_NEW_LINE();

				Data.Shader += "{";

				ADD_NEW_LINE();

				BuildStatementHolder(Function, Data);

				Data.Shader += "}";

				ADD_NEW_LINE();
			}

			void ASTToHLSLCompiler::BuildDomainAttributeType(DomainAttributeType* Attribute, const StageData& Data)
			{
				Data.Shader += "[domain(\"";

				Data.Shader += GetDomainType(Attribute->GetType());

				Data.Shader += "\")]";
			}

			void ASTToHLSLCompiler::BuildPartitioningAttributeType(PartitioningAttributeType* Attribute, const StageData& Data)
			{
				Data.Shader += "[partitioning(\"";

				Data.Shader += GetPartitioningType(Attribute->GetType());

				Data.Shader += "\")]";
			}

			void ASTToHLSLCompiler::BuildTopologyAttributeType(TopologyAttributeType* Attribute, const StageData& Data)
			{
				Data.Shader += "[outputtopology(\"";

				Data.Shader += GetTopologyType(Attribute->GetType());

				Data.Shader += "\")]";
			}

			void ASTToHLSLCompiler::BuildControlPointsAttributeType(ControlPointsAttributeType* Attribute, const StageData& Data)
			{
				Data.Shader += "[outputcontrolpoints(";

				Data.Shader += StringUtility::ToString<char8>(Attribute->GetNumber());

				Data.Shader += ")]";
			}

			void ASTToHLSLCompiler::BuildMaxVertexCountAttributeType(MaxVertexCountAttributeType* Attribute, const StageData& Data)
			{
				Data.Shader += "[maxvertexcount(";

				Data.Shader += StringUtility::ToString<char8>(Attribute->GetCount());

				Data.Shader += ")]";
			}

			void ASTToHLSLCompiler::BuildConstantEntrypointAttributeType(ConstantEntrypointAttributeType* Attribute, const StageData& Data)
			{
				Data.Shader += "[patchconstantfunc(\"";

				Data.Shader += Attribute->GetEntrypoint();

				Data.Shader += "\")]";
			}

			void ASTToHLSLCompiler::BuildThreadCountAttributeType(ThreadCountAttributeType* Attribute, const StageData& Data)
			{
				Data.Shader += "[numthreads(";

				Data.Shader += StringUtility::ToString<char8>(Attribute->GetXCount());
				Data.Shader += ", ";
				Data.Shader += StringUtility::ToString<char8>(Attribute->GetYCount());
				Data.Shader += ", ";
				Data.Shader += StringUtility::ToString<char8>(Attribute->GetZCount());

				Data.Shader += ")]";
			}

			void ASTToHLSLCompiler::BuildVariableAccessStatement(VariableAccessStatement* Statement, const StageData& Data)
			{
				Data.Shader += Statement->GetName();
			}

			void ASTToHLSLCompiler::BuildReturnStatement(ReturnStatement* Statement, const StageData& Data)
			{
				Data.Shader += "return ";

				BuildStatement(Statement->GetStatement(), Data);

				Data.Shader += ";";

				ADD_NEW_LINE();
			}

			void ASTToHLSLCompiler::BuildArrayStatement(ArrayStatement* Statement, const StageData& Data)
			{
				Data.Shader += '{';

				BuildArguments(Statement, Data);

				Data.Shader += '}';
			}

			void ASTToHLSLCompiler::BuildType(ProgramDataTypes Type, const StageData& Data)
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
					Data.Shader += "int2";
					break;

				case ProgramDataTypes::UnsignedInteger2:
					Data.Shader += "uint2";
					break;

				case ProgramDataTypes::Float2:
					Data.Shader += "float2";
					break;

				case ProgramDataTypes::Double2:
					Data.Shader += "double2";
					break;

				case ProgramDataTypes::Integer3:
					Data.Shader += "int3";
					break;

				case ProgramDataTypes::UnsignedInteger3:
					Data.Shader += "uint3";
					break;

				case ProgramDataTypes::Float3:
					Data.Shader += "float3";
					break;

				case ProgramDataTypes::Double3:
					Data.Shader += "double3";
					break;

				case ProgramDataTypes::Integer4:
					Data.Shader += "int4";
					break;

				case ProgramDataTypes::UnsignedInteger4:
					Data.Shader += "uint4";
					break;

				case ProgramDataTypes::Float4:
					Data.Shader += "float4";
					break;

				case ProgramDataTypes::Double4:
					Data.Shader += "double4";
					break;

				case ProgramDataTypes::Matrix4F:
					Data.Shader += "float4x4";
					break;

				case ProgramDataTypes::Matrix4D:
					Data.Shader += "double4x4";
					break;

				case ProgramDataTypes::Texture2D:
					Data.Shader += "Texture2D";
					break;
				}
			}

			void ASTToHLSLCompiler::BuildRootSignature(const StageData& Data)
			{
				Data.Shader += "#define ";
				Data.Shader += GetRootSignatureDefineName();
				Data.Shader += " \"";

				Data.Shader += "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)";

				uint8 cbvIndex = 0;
				uint8 textureIndex = 0;
				for (auto variableType : Data.Variables)
				{
					const DataTypeStatement* dataType = variableType->GetDataType();

					Data.Shader += ",";

					if (dataType->GetType() == ProgramDataTypes::Unknown)
					{
						Data.Shader += "CBV(b";

						Data.Shader += StringUtility::ToString<char8>(cbvIndex++);
					}
					else if (dataType->GetType() == ProgramDataTypes::Texture2D)
					{
						Data.Shader += "DescriptorTable(SRV(t";
						Data.Shader += StringUtility::ToString<char8>(textureIndex);
						Data.Shader += ")),DescriptorTable(Sampler(s";
						Data.Shader += StringUtility::ToString<char8>(textureIndex);
						Data.Shader += ")";

						++textureIndex;
					}

					Data.Shader += ")";
				}

				Data.Shader += "\"\n";
			}

			void ASTToHLSLCompiler::BuildStructVariable(StructVariableType* Variable, const StageData& Data)
			{
				BuildDataTypeStatement(Variable->GetDataType(), Data);

				Data.Shader += " ";
				Data.Shader += Variable->GetName();

				if (Variable->GetDataType()->GetPostElementCount() != nullptr)
				{
					Data.Shader += "[";
					BuildStatement(Variable->GetDataType()->GetPostElementCount(), Data);
					Data.Shader += "]";
				}

				if (Variable->GetRegister() != StructVariableType::Registers::None)
				{
					Data.Shader += ":";
					Data.Shader += GetRegisterName(Variable->GetRegister());
					Data.Shader += StringUtility::ToString<char8>(Variable->GetRegisterIndex());
				}

				Data.Shader += ";";

				ADD_NEW_LINE();
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
			String ASTToHLSLCompiler::GetGeometryOutputStreamParameterName(void)
			{
				return "outputStream";
			}
		};

#undef ADD_NEW_LINE
	}
}