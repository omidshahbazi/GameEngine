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
			bool IsRegisterAvailable(StructVariableType::Registers Register, Stages Stage)
			{
				switch (Register)
				{
				case StructVariableType::Registers::Position:
				case StructVariableType::Registers::Normal:
				case StructVariableType::Registers::Color:
				case StructVariableType::Registers::UV:
					return true;

				case StructVariableType::Registers::PrimitiveID:
					return (Stage == Stages::Hull || Stage == Stages::Domain || Stage == Stages::Geometry || Stage == Stages::Fragment);

				case StructVariableType::Registers::TessellationFactor:
				case StructVariableType::Registers::InsideTessellationFactor:
					return (Stage == Stages::Hull || Stage == Stages::Domain);

				case StructVariableType::Registers::DomainLocation:
					return (Stage == Stages::Domain);

				case StructVariableType::Registers::InstanceID:
					return (Stage == Stages::Geometry);

				case StructVariableType::Registers::OutputControlPointID:
					return (Stage == Stages::Hull);

				case StructVariableType::Registers::FragmentPosition:
					return true;

				case StructVariableType::Registers::Target:
					return (Stage == Stages::Fragment);

				case StructVariableType::Registers::DispatchThreadID:
				case StructVariableType::Registers::GroupID:
				case StructVariableType::Registers::GroupIndex:
				case StructVariableType::Registers::GroupThreadID:
					return (Stage == Stages::Compute);

				default:
					THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
				}

				return false;
			}

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

				case StructVariableType::Registers::OutputControlPointID:
					return "SV_OutputControlPointID";

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

			void ASTToHLSLCompiler::ResetPerStageValues(StageData& Data)
			{
				ASTCompilerBase::ResetPerStageValues(Data);

				m_ConstantBufferBindingCount = 0;
				m_TextureBindingCount = 0;
			}

			void ASTToHLSLCompiler::BuildStageShader(StageData& Data)
			{
				ASTCompilerBase::BuildStageShader(Data);

				if (Data.Stage != Stages::Vertex &&
					Data.Stage != Stages::Compute)
					return;

				String rootSignature;
				StageData data = { Data.FunctionType, Data.Stage, Data.Structs, Data.Variables, Data.Functions, rootSignature, 0 };
				BuildRootSignature(data);

				Data.Shader = rootSignature + Data.Shader;
			}

			void ASTToHLSLCompiler::BuildStruct(const StructType* Struct, StageData& Data)
			{
				AddCode("struct ", Data);

				const String& name = Struct->GetName();

				AddCode(name, Data);

				AddNewLine(Data);
				AddCode('{', Data);
				AddNewLine(Data);

				++Data.IndentOffset;

				for (auto variable : Struct->GetItems())
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
							BuildType(ProgramDataTypes::Float, Data);
							AddCode(StringUtility::ToString<char8>((GPUAlignedVector4F::Alignment - overflowByteCount) / GPUAlignedFloat32::Size), Data);
							AddCode(' ', Data);
							AddCode(name, Data);
							AddCode("Padding", Data);
							AddCode(';', Data);

							AddNewLine(Data);
						}
					}
				}

				--Data.IndentOffset;

				AddCode("};", Data);
				AddNewLine(Data);
			}

			void ASTToHLSLCompiler::BuildGlobalVariable(const GlobalVariableType* Variable, StageData& Data)
			{
				DataTypeStatement* dataType = Variable->GetDataType();

				if (!dataType->IsBuiltIn())
					AddCode("ConstantBuffer<", Data);

				BuildDataTypeStatement(dataType, Data);

				if (!dataType->IsBuiltIn())
					AddCode('>', Data);

				AddCode(' ', Data);
				AddCode(Variable->GetName(), Data);

				if (dataType->IsBuiltIn())
				{
					if (dataType->IsTexture())
					{
						AddCode(":register(t", Data);
						AddCode(StringUtility::ToString<char8>(m_TextureBindingCount), Data);
						AddCode(')', Data);
					}
				}
				else
				{
					AddCode(":register(b", Data);
					AddCode(StringUtility::ToString<char8>(m_ConstantBufferBindingCount++), Data);
					AddCode(')', Data);
				}

				AddCode(';', Data);

				AddNewLine(Data);

				if (dataType->IsTexture())
				{
					AddCode("SamplerState ", Data);
					AddCode(GetSamplerVariableName(Variable->GetName()), Data);
					AddCode(":register(s", Data);
					AddCode(StringUtility::ToString<char8>(m_TextureBindingCount), Data);
					AddCode(");", Data);

					AddNewLine(Data);

					++m_TextureBindingCount;
				}
			}

			void ASTToHLSLCompiler::BuildFunction(const FunctionType* Function, StageData& Data)
			{
				--Data.IndentOffset;

				FunctionType::Types funcType = Function->GetType();

				if (funcType == FunctionType::Types::VertexMain ||
					funcType == FunctionType::Types::ComputeMain)
				{
					AddCode("[RootSignature(", Data);
					AddCode(GetRootSignatureDefineName(), Data);
					AddCode(")]", Data);
					AddNewLine(Data);
				}
				else if (funcType == FunctionType::Types::DomainMain)
				{
					const FunctionType* hullEntrypoint = GetEntrypointFunctionType(FunctionType::Types::HullMain, Data);

					BuildDomainAttributeType(hullEntrypoint->GetAttribute<DomainAttributeType>(), Data);
					AddNewLine(Data);
				}

				BuildAttributes(Function->GetAttributes(), Data);

				BuildDataTypeStatement(Function->GetReturnDataType(), Data);

				AddCode(' ', Data);

				if (Function->IsEntrypoint())
					AddCode(Constants::ENTRY_POINT_NAME, Data);
				else
					AddCode(Function->GetName(), Data);

				AddCode('(', Data);

				if (funcType == FunctionType::Types::GeometryMain)
				{
					const auto* outputType = Function->GetAttribute<OutputStreamTypeAttributeType>();

					AddCode("inout ", Data);
					AddCode(GetOutputStreamType(outputType->GetType()), Data);
					AddCode('<', Data);
					AddCode(outputType->GetDataType(), Data);
					AddCode("> ", Data);
					AddCode(GetGeometryOutputStreamParameterName(), Data);

					AddCode(", ", Data);

					const auto* primitiveTypeAttr = Function->GetAttribute<PrimitiveTypeAttributeType>();

					AddCode(GetPrimitiveType(primitiveTypeAttr->GetType()), Data);
					AddCode(' ', Data);
				}

				if (funcType == FunctionType::Types::HullMain || funcType == FunctionType::Types::DomainMain)
				{
					const ParameterType* param = Function->GetParameters()[0];

					if (funcType == FunctionType::Types::HullMain)
						AddCode("InputPatch", Data);
					else if (funcType == FunctionType::Types::DomainMain)
					{
						AddCode(GetHullConstantFunction()->GetReturnDataType()->GetUserDefined(), Data);
						AddCode(" __RequiredValues, const OutputPatch", Data);
					}

					AddCode('<', Data);
					BuildDataTypeStatement(param->GetDataType(), Data);
					AddCode(", ", Data);
					BuildStatement(param->GetDataType()->GetPostElementCount(), Data);
					AddCode("> ", Data);
					AddCode(param->GetName(), Data);
				}
				else
					BuildParameters(Function->GetParameters(), Data);

				AddCode(')', Data);

				AddNewLine(Data);

				AddCode('{', Data);

				AddNewLine(Data);

				BuildStatementHolder(Function, true, Data);

				AddCode('}', Data);

				AddNewLine(Data);

				++Data.IndentOffset;
			}

			void ASTToHLSLCompiler::BuildDomainAttributeType(const DomainAttributeType* Attribute, StageData& Data)
			{
				AddCode("[domain(\"", Data);

				AddCode(GetDomainType(Attribute->GetType()), Data);

				AddCode("\")]", Data);
			}

			void ASTToHLSLCompiler::BuildPartitioningAttributeType(const PartitioningAttributeType* Attribute, StageData& Data)
			{
				AddCode("[partitioning(\"", Data);

				AddCode(GetPartitioningType(Attribute->GetType()), Data);

				AddCode("\")]", Data);
			}

			void ASTToHLSLCompiler::BuildTopologyAttributeType(const TopologyAttributeType* Attribute, StageData& Data)
			{
				AddCode("[outputtopology(\"", Data);

				AddCode(GetTopologyType(Attribute->GetType()), Data);

				AddCode("\")]", Data);
			}

			void ASTToHLSLCompiler::BuildControlPointsAttributeType(const ControlPointsAttributeType* Attribute, StageData& Data)
			{
				AddCode("[outputcontrolpoints(", Data);

				AddCode(StringUtility::ToString<char8>(Attribute->GetNumber()), Data);

				AddCode(")]", Data);
			}

			void ASTToHLSLCompiler::BuildMaxVertexCountAttributeType(const MaxVertexCountAttributeType* Attribute, StageData& Data)
			{
				AddCode("[maxvertexcount(", Data);

				AddCode(StringUtility::ToString<char8>(Attribute->GetCount()), Data);

				AddCode(")]", Data);
			}

			void ASTToHLSLCompiler::BuildConstantEntrypointAttributeType(const ConstantEntrypointAttributeType* Attribute, StageData& Data)
			{
				AddCode("[patchconstantfunc(\"", Data);

				AddCode(Attribute->GetEntrypoint(), Data);

				AddCode("\")]", Data);
			}

			void ASTToHLSLCompiler::BuildThreadCountAttributeType(const ThreadCountAttributeType* Attribute, StageData& Data)
			{
				AddCode("[numthreads(", Data);

				AddCode(StringUtility::ToString<char8>(Attribute->GetXCount()), Data);
				AddCode(", ", Data);
				AddCode(StringUtility::ToString<char8>(Attribute->GetYCount()), Data);
				AddCode(", ", Data);
				AddCode(StringUtility::ToString<char8>(Attribute->GetZCount()), Data);

				AddCode(")]", Data);
			}

			void ASTToHLSLCompiler::BuildReturnStatement(const ReturnStatement* Statement, StageData& Data)
			{
				AddCode("return ", Data);

				BuildStatement(Statement->GetStatement(), Data);

				AddCode(';', Data);

				AddNewLine(Data);
			}

			void ASTToHLSLCompiler::BuildArrayStatement(const ArrayStatement* Statement, StageData& Data)
			{
				AddCode('{', Data);

				BuildArguments(Statement, Data);

				AddCode('}', Data);
			}

			void ASTToHLSLCompiler::BuildPostTemplateDataTypeStatement(const DataTypeStatement* Statement, StageData& Data)
			{
				AddCode('<', Data);

				BuildDataTypeStatement(Statement, Data);

				AddCode('>', Data);
			}

			void ASTToHLSLCompiler::BuildType(ProgramDataTypes Type, StageData& Data)
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
					AddCode("int2", Data);
					break;

				case ProgramDataTypes::UnsignedInteger2:
					AddCode("uint2", Data);
					break;

				case ProgramDataTypes::Float2:
					AddCode("float2", Data);
					break;

				case ProgramDataTypes::Double2:
					AddCode("double2", Data);
					break;

				case ProgramDataTypes::Integer3:
					AddCode("int3", Data);
					break;

				case ProgramDataTypes::UnsignedInteger3:
					AddCode("uint3", Data);
					break;

				case ProgramDataTypes::Float3:
					AddCode("float3", Data);
					break;

				case ProgramDataTypes::Double3:
					AddCode("double3", Data);
					break;

				case ProgramDataTypes::Integer4:
					AddCode("int4", Data);
					break;

				case ProgramDataTypes::UnsignedInteger4:
					AddCode("uint4", Data);
					break;

				case ProgramDataTypes::Float4:
					AddCode("float4", Data);
					break;

				case ProgramDataTypes::Double4:
					AddCode("double4", Data);
					break;

				case ProgramDataTypes::Matrix4F:
					AddCode("float4x4", Data);
					break;

				case ProgramDataTypes::Matrix4D:
					AddCode("double4x4", Data);
					break;

				case ProgramDataTypes::Texture1D:
					AddCode("Texture1D", Data);
					break;

				case ProgramDataTypes::Texture2D:
					AddCode("Texture2D", Data);
					break;

				case ProgramDataTypes::Texture3D:
					AddCode("Texture3D", Data);
					break;

				case ProgramDataTypes::TextureCube:
					AddCode("TextureCube", Data);
					break;

				case ProgramDataTypes::Texture1DRW:
					AddCode("RWTexture1D", Data);
					break;

				case ProgramDataTypes::Texture2DRW:
					AddCode("RWTexture2D", Data);
					break;

				case ProgramDataTypes::Texture3DRW:
					AddCode("RWTexture3D", Data);
					break;
				}
			}

			void ASTToHLSLCompiler::BuildRootSignature(StageData& Data)
			{
				AddCode("#define ", Data);
				AddCode(GetRootSignatureDefineName(), Data);
				AddCode(" \"", Data);

				AddCode("RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)", Data);

				uint8 cbvIndex = 0;
				uint8 textureIndex = 0;
				for (auto variableType : Data.Variables)
				{
					const DataTypeStatement* dataType = variableType->GetDataType();

					AddCode(',', Data);

					if (dataType->GetType() == ProgramDataTypes::Unknown)
					{
						AddCode("CBV(b", Data);

						AddCode(StringUtility::ToString<char8>(cbvIndex++), Data);
					}
					else if (dataType->IsTexture())
					{
						AddCode("DescriptorTable(SRV(t", Data);
						AddCode(StringUtility::ToString<char8>(textureIndex), Data);
						AddCode(")),DescriptorTable(Sampler(s", Data);
						AddCode(StringUtility::ToString<char8>(textureIndex), Data);
						AddCode(")", Data);

						++textureIndex;
					}

					AddCode(')', Data);
				}

				AddCode("\"\n", Data);
			}

			void ASTToHLSLCompiler::BuildStructVariable(const StructVariableType* Variable, StageData& Data)
			{
				BuildDataTypeStatement(Variable->GetDataType(), Data);

				AddCode(' ', Data);
				AddCode(Variable->GetName(), Data);

				if (Variable->GetDataType()->GetPostElementCount() != nullptr)
				{
					AddCode('[', Data);
					BuildStatement(Variable->GetDataType()->GetPostElementCount(), Data);
					AddCode(']', Data);
				}

				if (Variable->GetRegister() != StructVariableType::Registers::None && IsRegisterAvailable(Variable->GetRegister(), Data.Stage))
				{
					AddCode(" : ", Data);
					AddCode(GetRegisterName(Variable->GetRegister()), Data);

					//if (!IsSystemValue(Variable->GetRegister()))
					AddCode(StringUtility::ToString<char8>(Variable->GetRegisterIndex()), Data);
				}

				AddCode(';', Data);

				AddNewLine(Data);
			}

			cstr ASTToHLSLCompiler::GetRootSignatureDefineName(void)
			{
				static cstr name = "__RS__";

				return name;
			}

			String ASTToHLSLCompiler::GetGeometryOutputStreamParameterName(void)
			{
				return "outputStream";
			}
		};
	}
}