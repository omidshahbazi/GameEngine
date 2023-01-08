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
			bool IsInternalVertexLayout(StructVariableType::Registers Register)
			{
				switch (Register)
				{
				case StructVariableType::Registers::Position:
				case StructVariableType::Registers::Normal:
				case StructVariableType::Registers::UV:
				case StructVariableType::Registers::Target:
					return true;

				default:
					return false;
				}
			}

			bool IsAdditionalVertexLayout(StructVariableType::Registers Register)
			{
				switch (Register)
				{
				case StructVariableType::Registers::Color:
					return true;

				default:
					return false;
				}
			}

			bool IsVertexLayout(StructVariableType::Registers Register)
			{
				if (IsInternalVertexLayout(Register))
					return true;

				if (IsAdditionalVertexLayout(Register))
					return true;

				return false;
			}

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

				default:
					THROW_PROGRAM_COMPILER_EXCEPTION("Register not defined", String::Empty);
				}
			}

			String GetSystemValue(StructVariableType::Registers Register, Stages Stage)
			{
				switch (Register)
				{
				case StructVariableType::Registers::Position:
				{
					if (Stage == Stages::Fragment)
						return "gl_FragCoord";

					return "gl_Position";
				}

				case StructVariableType::Registers::TessellationFactor:
					return "gl_TessLevelOuter";

				case StructVariableType::Registers::InsideTessellationFactor:
					return "gl_TessLevelInner";

				case StructVariableType::Registers::PrimitiveID:
					return "gl_PrimitiveID";

				case StructVariableType::Registers::DomainLocation:
					return "gl_TessCord";

				case StructVariableType::Registers::InstanceID:
					return "gl_InstanceID";

				case StructVariableType::Registers::OutputControlPointID:
					return "gl_InvocationID";

				case StructVariableType::Registers::FragmentPosition:
					return "gl_FragCoord";

				case StructVariableType::Registers::DispatchThreadID:
					return "gl_LocalInvocationID";

				case StructVariableType::Registers::GroupID:
					return "gl_WorkGroupID";

				case StructVariableType::Registers::GroupIndex:
					return "gl_LocalInvocationIndex";

				case StructVariableType::Registers::GroupThreadID:
					return "gl_LocalInvocationID";
				}

				THROW_PROGRAM_COMPILER_EXCEPTION("Register not defined", String::Empty);
			}

			String GetDomainType(DomainAttributeType::Types Type)
			{
				switch (Type)
				{
				case DomainAttributeType::Types::Triangle:
					return "triangles";

				case DomainAttributeType::Types::Quad:
					return "quads";

				case DomainAttributeType::Types::Isoline:
					return "isolines";
				}

				THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
			}

			String GetPartitioningType(PartitioningAttributeType::Types Type)
			{
				switch (Type)
				{
				case PartitioningAttributeType::Types::Integer:
					return "equal_spacing";

				case PartitioningAttributeType::Types::FractionalEven:
					return "fractional_even_spacing";

				case PartitioningAttributeType::Types::FractionalOdd:
					return "fractional_odd_spacing";

				case PartitioningAttributeType::Types::PowerOfTwo:
					return "equal_spacing";
				}

				THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
			}

			String GetTopologyType(TopologyAttributeType::Types Type)
			{
				switch (Type)
				{
				case TopologyAttributeType::Types::Point:
					return "points";

				case TopologyAttributeType::Types::Line:
					return "lines";

				case TopologyAttributeType::Types::TriangleClockwise:
					return "cw";

				case TopologyAttributeType::Types::TriangleCounterClockwise:
					return "ccw";
				}

				THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
			}

			String GetPrimitiveType(PrimitiveTypeAttributeType::Types Type)
			{
				switch (Type)
				{
				case PrimitiveTypeAttributeType::Types::Point:
					return "points";

				case PrimitiveTypeAttributeType::Types::Line:
					return "lines";

				case PrimitiveTypeAttributeType::Types::Triangle:
					return "triangles";

				case PrimitiveTypeAttributeType::Types::LineAdjacency:
					return "lines_adjacency";

				case PrimitiveTypeAttributeType::Types::TriangleAdjacency:
					return "triangles_adjacency";
				}

				THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
			}

			String GetOutputStreamType(OutputStreamTypeAttributeType::Types Type)
			{
				switch (Type)
				{
				case OutputStreamTypeAttributeType::Types::Point:
					return "points";

				case OutputStreamTypeAttributeType::Types::Line:
					return "line_strip";

				case OutputStreamTypeAttributeType::Types::Triangle:
					return "triangle_strip";
				}

				THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
			}

			String GetFragmentVariableName(uint8 Index)
			{
				return String(Constants::FRAGMENT_ENTRY_POINT_NAME) + "_FragColor" + StringUtility::ToString<char8>(Index);
			}

			ASTToGLSLCompiler::ASTToGLSLCompiler() :
				m_AdditionalLayoutCount(0),
				m_BindingCount(0),
				m_SequentialVariableNumber(0)
			{
			}

			void ASTToGLSLCompiler::ResetPerStageValues(StageData& Data)
			{
				ASTCompilerBase::ResetPerStageValues(Data);

				m_AdditionalLayoutCount = SubMeshInfo::GetLayoutCount();
				m_BindingCount = 0;
				m_SequentialVariableNumber = 0;
			}

			void ASTToGLSLCompiler::BuildStageShader(StageData& Data)
			{
				ASTCompilerBase::BuildStageShader(Data);

				String header = "#version 460 core\n";

				if (Data.Stage == Stages::Hull)
				{
					const FunctionType* hullEntrypoint = GetEntrypointFunctionType(FunctionType::Types::HullMain, Data);

					header += "layout (vertices = ";

					const ControlPointsAttributeType* attrib = hullEntrypoint->GetAttribute<ControlPointsAttributeType>();
					header += StringUtility::ToString<char8>(attrib->GetNumber());

					header += ") out;\n";
				}
				else if (Data.Stage == Stages::Domain)
				{
					const FunctionType* hullEntrypoint = GetEntrypointFunctionType(FunctionType::Types::HullMain, Data);

					header += "layout (";

					const DomainAttributeType* domainAttrib = hullEntrypoint->GetAttribute<DomainAttributeType>();
					header += GetDomainType(domainAttrib->GetType());
					header += ", ";

					const PartitioningAttributeType* partitioningAttrib = hullEntrypoint->GetAttribute<PartitioningAttributeType>();
					header += GetPartitioningType(partitioningAttrib->GetType());
					header += ", ";

					const TopologyAttributeType* topologyAttrib = hullEntrypoint->GetAttribute<TopologyAttributeType>();
					header += GetTopologyType(topologyAttrib->GetType());

					header += ") in;\n";
				}
				else if (Data.Stage == Stages::Geometry)
				{
					const FunctionType* geometryEntrypoint = GetEntrypointFunctionType(FunctionType::Types::GeometryMain, Data);

					header += "layout (";

					const PrimitiveTypeAttributeType* primitiveTypeAttrib = geometryEntrypoint->GetAttribute<PrimitiveTypeAttributeType>();
					header += GetPrimitiveType(primitiveTypeAttrib->GetType());
					header += ") in;\n";

					header += "layout (";

					const OutputStreamTypeAttributeType* outputStreamTypeAttrib = geometryEntrypoint->GetAttribute<OutputStreamTypeAttributeType>();
					header += GetOutputStreamType(outputStreamTypeAttrib->GetType());
					header += ", max_vertices = ";

					const MaxVertexCountAttributeType* maxVertexCountAttrib = geometryEntrypoint->GetAttribute<MaxVertexCountAttributeType>();
					header += StringUtility::ToString<char8>(maxVertexCountAttrib->GetCount());

					header += ") out;\n";
				}

				Data.Shader = header + Data.Shader;
			}

			void ASTToGLSLCompiler::BuildStruct(const StructType* Struct, StageData& Data)
			{
				AddCode("struct ", Data);
				AddCode(Struct->GetName(), Data);
				AddNewLine(Data);

				AddCode('{', Data);
				AddNewLine(Data);

				++Data.IndentOffset;

				for (auto& variable : Struct->GetItems())
				{
					BuildDataTypeStatement(variable->GetDataType(), Data);

					AddCode(' ', Data);

					AddCode(variable->GetName(), Data);

					BuildPostDataType(variable->GetDataType(), Data);

					AddCode(';', Data);

					AddNewLine(Data);
				}

				--Data.IndentOffset;

				AddCode("};", Data);
				AddNewLine(Data);
			}

			void ASTToGLSLCompiler::BuildGlobalVariable(const GlobalVariableType* Variable, StageData& Data)
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

			void ASTToGLSLCompiler::BuildFunction(const FunctionType* Function, StageData& Data)
			{
				FunctionType::Types funcType = Function->GetType();

				if (IsEntrypointOrHullConstant(Function, Data))
				{
					const bool inputHaveToConvertToArray = (funcType == FunctionType::Types::HullMain || funcType == FunctionType::Types::DomainMain || funcType == FunctionType::Types::GeometryMain);
					const auto* parameter = Function->GetParameters()[0];
					BuildInputOutputStruct(parameter->GetDataType(), parameter->GetName(), true, inputHaveToConvertToArray, (funcType == FunctionType::Types::GeometryMain), Data);

					if (funcType != FunctionType::Types::GeometryMain &&
						funcType != FunctionType::Types::ComputeMain)
					{
						const bool outputHaveToConvertToArray = (funcType == FunctionType::Types::HullMain);
						BuildInputOutputStruct(Function->GetReturnDataType(), String::Empty, false, outputHaveToConvertToArray, false, Data);
					}
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

				--Data.IndentOffset;

				if (IsEntrypointOrHullConstant(Function, Data))
					BuildType(ProgramDataTypes::Void, Data);
				else
					BuildDataTypeStatement(Function->GetReturnDataType(), Data);

				AddCode(' ', Data);

				if (Function->IsEntrypoint())
					AddCode(Constants::ENTRY_POINT_NAME, Data);
				else
					AddCode(Function->GetName(), Data);

				AddCode('(', Data);

				if (IsEntrypointOrHullConstant(Function, Data))
				{
					for (auto& parameter : Function->GetParameters())
						PushVariable(parameter);
				}
				else
					BuildParameters(Function->GetParameters(), Data);

				AddCode(')', Data);
				AddNewLine(Data);

				AddCode('{', Data);

				AddNewLine(Data);

				if (funcType == FunctionType::Types::HullMain)
				{
					AddCode("if (", Data);
					AddCode(GetSystemValue(StructVariableType::Registers::OutputControlPointID, Data.Stage), Data);
					AddCode(" == 0)", Data);
					AddNewLine(Data);

					++Data.IndentOffset;
					AddCode(GetHullConstantFunction()->GetName(), Data);
					AddCode("();", Data);
					--Data.IndentOffset;

					AddNewLine(Data);
				}

				BuildStatementHolder(Function, true, Data);

				AddCode('}', Data);

				++Data.IndentOffset;

				AddNewLine(Data);
			}

			void ASTToGLSLCompiler::BuildFunctionCallStatement(const FunctionCallStatement* Statement, StageData& Data)
			{
				if (Data.FunctionType == FunctionType::Types::GeometryMain)
				{
					String temp;
					if (IntrinsicsBuilder::BuildFunctionCallStatement(Statement, Data.FunctionType, Data.Stage, temp))
					{
						if (temp.StartsWith("EmitVertex"))
						{
							const String& returnDataType = GetEntrypointFunctionType(FunctionType::Types::GeometryMain, Data)->GetAttribute<OutputStreamTypeAttributeType>()->GetDataType();

							auto* argument = Statement->GetArguments()->GetItems()[0];
							DataTypeStatement dataType = EvaluateDataType(argument);

							if (dataType.GetUserDefined() != returnDataType)
								THROW_PROGRAM_COMPILER_EXCEPTION("Invalid data-type to append to the output stream", dataType.GetUserDefined());

							const String varName = BuildSequentialVariable(&dataType, argument, Data);

							auto* structType = GetStructType(dataType.GetUserDefined());

							for (auto& variable : structType->GetItems())
							{
								if (variable->GetRegister() == StructVariableType::Registers::None || variable->GetRegisterIndex() != 0)
									continue;

								if (IsVertexLayout(variable->GetRegister()) && variable->GetRegister() != StructVariableType::Registers::Position)
									continue;

								AddCode(GetSystemValue(variable->GetRegister(), Data.Stage), Data);
								AddCode(" = ", Data);
								AddCode(varName, Data);
								AddCode('.', Data);
								AddCode(variable->GetName(), Data);
								AddCode(';', Data);
								AddNewLine(Data);
							}
						}
					}
				}

				ASTCompilerBase::BuildFunctionCallStatement(Statement, Data);
			}

			void ASTToGLSLCompiler::BuildMemberAccessStatement(const MemberAccessStatement* Statement, StageData& Data)
			{
				bool handled = false;

				if (IsEntrypointOrHullConstant(GetLastFunction(), Data))
				{
					String leftStm = String::Empty;
					ArrayElementAccessStatement* leftAsArrayElementAccess = nullptr;
					if (IsAssignableFrom(Statement->GetLeft(), ArrayElementAccessStatement))
					{
						leftAsArrayElementAccess = ReinterpretCast(ArrayElementAccessStatement*, Statement->GetLeft());
						leftStm = StatementToString(leftAsArrayElementAccess->GetArrayStatement(), Data);
					}
					else
						leftStm = StatementToString(Statement->GetLeft(), Data);

					//CheckMemberAccess(Statement->GetLeft());

					const VariableType* leftVariable = FindVariableType(leftStm);
					if (leftVariable != nullptr)
					{
						PushDataAccessStatement(Statement->GetLeft());

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

							IncreaseBlockIndexAndPushStructVariables(structType);

							String rightStm = StatementToString(rightStatement, Data);
							rightStm = rightStm.Split('.')[0];

							const StructVariableType* rightVariable = GetVariableType(structType, rightStm);

							BuildFlattenStructMemberVariableName(structType, rightVariable, name, isParameter, Data);

							if (Data.FunctionType == FunctionType::Types::HullMain ||
								Data.FunctionType == FunctionType::Types::DomainMain ||
								Data.FunctionType == FunctionType::Types::GeometryMain)
							{
								if (isOutputVariable && Data.FunctionType == FunctionType::Types::HullMain)
								{
									AddCode('[', Data);
									AddCode(GetSystemValue(StructVariableType::Registers::OutputControlPointID, Data.Stage), Data);
									AddCode(']', Data);
								}
								else if (leftAsArrayElementAccess != nullptr &&
									(rightVariable->GetRegister() == StructVariableType::Registers::None || IsVertexLayout(rightVariable->GetRegister())))
								{
									AddCode('[', Data);
									BuildStatement(leftAsArrayElementAccess->GetElementStatement(), Data);
									AddCode(']', Data);
								}
							}

							PushDataAccessStatement(rightStatement);

							if (isMemberAccess)
							{
								AddCode('.', Data);

								BuildStatement(ReinterpretCast(MemberAccessStatement*, Statement->GetRight())->GetRight(), Data);
							}

							DecreaseBlockIndex();

							PopDataAceessStatement();

							handled = true;
						}

						PopDataAceessStatement();
					}
				}

				if (!handled)
					ASTCompilerBase::BuildMemberAccessStatement(Statement, Data);
			}

			void ASTToGLSLCompiler::BuildReturnStatement(const ReturnStatement* Statement, StageData& Data)
			{
				if (!IsEntrypointOrHullConstant(GetLastFunction(), Data))
				{
					AddCode("return ", Data);

					BuildStatement(Statement->GetStatement(), Data);

					AddCode(';', Data);
					AddNewLine(Data);

					return;
				}

				const String returnDataType = GetLastFunction()->GetReturnDataType()->GetUserDefined();
				const StructType* structType = GetStructType(returnDataType);
				const StructVariableType* variableType = nullptr;

				if (Data.FunctionType == FunctionType::Types::VertexMain || Data.FunctionType == FunctionType::Types::DomainMain)
				{
					AddCode(GetSystemValue(StructVariableType::Registers::Position, Data.Stage), Data);
					AddCode(" = ", Data);

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

				if (variableType != nullptr)
				{
					const uint8 requiredComponentCount = 4;
					const uint8 variableComponentCount = variableType->GetDataType()->GetComponentCount();
					if (variableComponentCount != requiredComponentCount)
						AddCode("vec4(", Data);

					BuildFlattenStructMemberVariableName(structType, variableType, String::Empty, false, Data);

					if (variableComponentCount != requiredComponentCount)
					{
						for (uint8 i = 0; i < requiredComponentCount - variableComponentCount; ++i)
							AddCode(", 1", Data);

						AddCode(')', Data);
					}

					AddCode(';', Data);
					AddNewLine(Data);
				}

				AddCode("return;", Data);
				AddNewLine(Data);
			}

			void ASTToGLSLCompiler::BuildArrayStatement(const ArrayStatement* Statement, StageData& Data)
			{
				DataTypeStatement dataType = EvaluateDataType(Statement);

				BuildType(dataType.GetType(), Data);

				AddCode("[](", Data);

				BuildArguments(Statement, Data);

				AddCode(')', Data);
			}

			void ASTToGLSLCompiler::BuildExplicitCast(const Statement* Statement, const DataTypeStatement* DataType, StageData& Data)
			{
				DataTypeStatement sourceDataType = EvaluateDataType(Statement);
				bool needsCasting = !CompareDataTypes(sourceDataType, *DataType);

				if (needsCasting)
				{
					CheckForImplicitCast(sourceDataType, *DataType);

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

			void ASTToGLSLCompiler::BuildInputOutputStruct(const DataTypeStatement* DataType, const String& Name, bool IsInput, bool ConvertToArray, bool ExplicitArrayLength, StageData& Data)
			{
				CoreDebugAssert(Categories::ProgramCompiler, !DataType->IsBuiltIn(), "DataType must be user-defined");

				const StructType* structType = GetStructType(DataType->GetUserDefined());
				const auto& variables = structType->GetItems();

				if (variables.ContainsIf([](auto item) { return item->GetRegister() == StructVariableType::Registers::None; }))
					THROW_PROGRAM_COMPILER_EXCEPTION("Struct cannot have variables without register specified", structType->GetName());

				for (auto& variable : variables)
				{
					if (!IsVertexLayout(variable->GetRegister()))
						continue;

					int8 location = 0;
					if (IsInput && variable->GetRegisterIndex() == 0)
						location = SubMeshInfo::GetLayoutIndex(GetLayout(variable->GetRegister()));
					else
						location = m_AdditionalLayoutCount++;

					AddCode("layout(location = ", Data);
					AddCode(StringUtility::ToString<char8>(location), Data);
					AddCode(") ", Data);
					AddCode((IsInput ? "in " : "out "), Data);

					BuildDataTypeStatement(variable->GetDataType(), Data);
					AddCode(' ', Data);

					BuildFlattenStructMemberVariableName(structType, variable, Name, IsInput, Data);

					if (ConvertToArray)
					{
						AddCode('[', Data);

						if (ExplicitArrayLength && DataType->GetPostElementCount() != nullptr)
							BuildStatement(DataType->GetPostElementCount(), Data);

						AddCode(']', Data);
					}

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

				++Data.IndentOffset;

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

				--Data.IndentOffset;

				AddCode("} ", Data);

				AddCode(Name, Data);

				AddCode(';', Data);

				AddNewLine(Data);
			}

			void ASTToGLSLCompiler::BuildFlattenStructMemberVariableName(const StructType* Parent, const StructVariableType* Variable, const String& Name, bool IsInput, StageData& Data)
			{
				if (!IsVertexLayout(Variable->GetRegister()))
				{
					AddCode(GetSystemValue(Variable->GetRegister(), Data.Stage), Data);
					return;
				}

				AddCode(Name, Data);
				AddCode('_', Data);
				AddCode((IsInput ? "In" : "Out"), Data);
				AddCode('_', Data);
				AddCode(Parent->GetName(), Data);
				AddCode('_', Data);
				AddCode(Variable->GetName(), Data);
			}

			String ASTToGLSLCompiler::BuildSequentialVariable(const Statement* IntializerStatement, StageData& Data)
			{
				DataTypeStatement dataType = EvaluateDataType(IntializerStatement);
				return BuildSequentialVariable(&dataType, IntializerStatement, Data);
			}

			String ASTToGLSLCompiler::BuildSequentialVariable(const DataTypeStatement* DataType, const Statement* IntializerStatement, StageData& Data)
			{
				const String varName = '_' + StringUtility::ToString<char8>(++m_SequentialVariableNumber);

				BuildDataTypeStatement(DataType, Data);
				AddCode(' ', Data);
				AddCode(varName, Data);
				AddCode(" = ", Data);
				BuildStatement(IntializerStatement, Data);
				AddCode(';', Data);
				AddNewLine(Data);

				return varName;
			}
		}
	}
}