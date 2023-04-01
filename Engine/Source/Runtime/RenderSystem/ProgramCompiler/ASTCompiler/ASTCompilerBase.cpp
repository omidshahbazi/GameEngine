// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ASTCompiler\ASTCompilerBase.h>
#include <ProgramCompilerCommon\ProgramCompilerException.h>

namespace Engine
{
	using namespace ProgramCompilerCommon;

	namespace ASTCompiler
	{
		ASTCompilerBase::ASTCompilerBase(void) :
			m_Allocator(nullptr),
			m_BlockIndex(-1),
			m_HullConstantFunction(nullptr),
			m_LastFunction(nullptr),
			m_SequentialVariableNumber(0)
		{
		}

		void ASTCompilerBase::Initialize(DeviceTypes DeviceType)
		{
			IntrinsicsBuilder::Initialize(DeviceType);
		}

		void ASTCompilerBase::Compile(AllocatorBase* Allocator, const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, OutputInfo& Output)
		{
			m_BlockIndex = -1;
			m_LastFunction = nullptr;
			m_Allocator = Allocator;
			m_BlockVariables = BlockVariablesList(Allocator);

			auto containsEntrypoint = [&Functions](FunctionType::Types Type)
			{
				return Functions.ContainsIf([Type](auto functionType) {return functionType->GetType() == Type; });
			};

			if (containsEntrypoint(FunctionType::Types::VertexMain))
			{
				StageData data = { FunctionType::Types::None, Stages::Vertex, Structs, Variables, Functions, Output.VertexShader, 0 };
				BuildStageShader(data);
			}

			if (containsEntrypoint(FunctionType::Types::HullMain))
			{
				StageData data = { FunctionType::Types::None, Stages::Hull, Structs, Variables, Functions, Output.HullShader, 0 };
				BuildStageShader(data);
			}

			if (containsEntrypoint(FunctionType::Types::DomainMain))
			{
				StageData data = { FunctionType::Types::None, Stages::Domain, Structs, Variables, Functions, Output.DomainShader, 0 };
				BuildStageShader(data);
			}

			if (containsEntrypoint(FunctionType::Types::GeometryMain))
			{
				StageData data = { FunctionType::Types::None, Stages::Geometry, Structs, Variables, Functions, Output.GeometryShader, 0 };
				BuildStageShader(data);
			}

			if (containsEntrypoint(FunctionType::Types::FragmentMain))
			{
				StageData data = { FunctionType::Types::None, Stages::Fragment, Structs, Variables, Functions, Output.FragmentShader, 0 };
				BuildStageShader(data);
			}

			if (containsEntrypoint(FunctionType::Types::ComputeMain))
			{
				StageData data = { FunctionType::Types::None, Stages::Compute, Structs, Variables, Functions, Output.ComputeShader, 0 };
				BuildStageShader(data);
			}

			m_Structs.Clear();
			m_Functions.Clear();
			m_BlockVariables.Clear();
		}

		void ASTCompilerBase::BuildStageShader(StageData& Data)
		{
			ResetPerStageValues(Data);

			BuildStructs(Data);

			BuildGlobalVariables(Data);

			IntrinsicsBuilder::BuildBuiltIns(Data.Stage, Data.Shader);

			BuildFunctions(Data);
		}

		void ASTCompilerBase::ResetPerStageValues(StageData& Data)
		{
			m_Structs.Clear();
			m_Functions.Clear();
			m_BlockVariables.Clear();
			m_BlockIndex = -1;
			m_LastFunction = nullptr;
			m_SequentialVariableNumber = 0;

			IncreaseBlockIndex();

			if (Data.Stage == Stages::Hull || Data.Stage == Stages::Domain)
			{
				auto getHullConstantFunction = [this, &Data]() -> const FunctionType*
				{
					const FunctionType* hullEntrypoint = GetEntrypointFunctionType(FunctionType::Types::HullMain, Data);

					const ConstantEntrypointAttributeType* attrib = hullEntrypoint->GetAttribute<ConstantEntrypointAttributeType>();

					int32 index = Data.Functions.FindIf([attrib](auto& item) { return item->GetName() == attrib->GetEntrypoint(); });
					if (index == -1)
						return nullptr;

					return Data.Functions[index];
				};

				m_HullConstantFunction = getHullConstantFunction();
			}
		}

		void ASTCompilerBase::BuildStructs(StageData& Data)
		{
			for (auto structType : Data.Structs)
			{
				m_Structs.Add(structType);

				BuildStruct(structType, Data);
			}
		}

		void ASTCompilerBase::BuildGlobalVariables(StageData& Data)
		{
			for (auto variable : Data.Variables)
			{
				PushVariable(variable);

				BuildGlobalVariable(variable, Data);
			}
		}

		void ASTCompilerBase::BuildFunctions(StageData& Data)
		{
			for (auto function : Data.Functions)
			{
				FunctionType::Types funcType = function->GetType();

				if (function->IsEntrypoint())
				{
					if (funcType == FunctionType::Types::VertexMain && Data.Stage != Stages::Vertex)
						continue;

					if (funcType == FunctionType::Types::HullMain && Data.Stage != Stages::Hull)
						continue;

					if (funcType == FunctionType::Types::DomainMain && Data.Stage != Stages::Domain)
						continue;

					if (funcType == FunctionType::Types::GeometryMain && Data.Stage != Stages::Geometry)
						continue;

					if (funcType == FunctionType::Types::FragmentMain && Data.Stage != Stages::Fragment)
						continue;

					if (funcType == FunctionType::Types::ComputeMain && Data.Stage != Stages::Compute)
						continue;
				}

				m_Functions.Add(function);
				m_LastFunction = function;

				IncreaseBlockIndex();

				for (auto parameter : function->GetParameters())
					PushVariable(parameter);

				StageData data = { funcType, Data.Stage, Data.Structs, Data.Variables, Data.Functions, Data.Shader };

				if (function->IsEntrypoint())
					ValidateEntrypointFunction(function, data);

				BuildFunction(function, data);

				DecreaseBlockIndex();
			}
		}

		void ASTCompilerBase::ValidateEntrypointFunction(const FunctionType* Function, StageData& Data)
		{
			auto checkExistenceOfEntrypoint = [&](FunctionType::Types Type)
			{
				if (Data.Functions.ContainsIf([&](auto item) { return item->GetType() == Type; }))
					return;

				THROW_PROGRAM_COMPILER_EXCEPTION("Complementary entrypoint is missing", Function->GetName());
			};

			auto checkRequiredRegisters = [&](const String& StructName, const StructVariableType::Registers* RequiredRegisters, uint8 RequiredRegisterCount)
			{
				const StructType* structType = GetStructType(StructName);

				for (uint8 i = 0; i < RequiredRegisterCount; ++i)
				{
					StructVariableType::Registers reg = RequiredRegisters[i];

					if (structType->GetItems().ContainsIf([reg](auto item) { return item->GetRegister() == reg; }))
						continue;

					THROW_PROGRAM_COMPILER_EXCEPTION(StringUtility::Format<char8>("A variable with %S is required inside %S", StructVariableType::GetRegisterName(reg), structType->GetName()), Function->GetName());
				}
			};

			auto checkRequiredInputs = [&](const FunctionType* Function, const StructVariableType::Registers* RequiredRegisters, uint8 RequiredRegisterCount, bool MustBeArray)
			{
				for (auto& param : Function->GetParameters())
				{
					if (param->GetDataType()->IsArray() != MustBeArray)
						THROW_PROGRAM_COMPILER_EXCEPTION(StringUtility::Format<char8>("Parameter %S array type mismatched with the desired type", param->GetName()), Function->GetName());

					auto& structName = param->GetDataType()->GetUserDefined();
					if (structName == String::Empty)
						continue;

					checkRequiredRegisters(structName, RequiredRegisters, RequiredRegisterCount);
				}
			};

			auto checkRequiredOutputs = [&](const FunctionType* Function, const StructVariableType::Registers* RequiredRegisters, uint8 RequiredRegisterCount)
			{
				checkRequiredRegisters(Function->GetReturnDataType()->GetUserDefined(), RequiredRegisters, RequiredRegisterCount);
			};

			const StructVariableType::Registers RequiredTessFactorsRegisters[]{ StructVariableType::Registers::TessellationFactor, StructVariableType::Registers::InsideTessellationFactor };

			if (Function->GetParameters().GetSize() > 1)
				THROW_PROGRAM_COMPILER_EXCEPTION("Entrypoints cannot have more than one parameter", Function->GetName());

			FunctionType::Types funcType = Function->GetType();
			checkRequiredInputs(Function, nullptr, 0, (funcType == FunctionType::Types::HullMain || funcType == FunctionType::Types::DomainMain || funcType == FunctionType::Types::GeometryMain));

			if (Data.Stage == Stages::Hull)
			{
				if (Function->GetAttribute<DomainAttributeType>() == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find Domain attribute", Function->GetName());
				if (Function->GetAttribute<PartitioningAttributeType>() == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find Partitioning attribute", Function->GetName());
				if (Function->GetAttribute<TopologyAttributeType>() == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find Topology attribute", Function->GetName());
				if (Function->GetAttribute<ControlPointsAttributeType>() == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find ControlPoints attribute", Function->GetName());
				if (Function->GetAttribute<ConstantEntrypointAttributeType>() == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find ConstantEntrypoint attribute", Function->GetName());

				if (m_HullConstantFunction == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find ConstantEntrypoint function", Function->GetName());

				checkRequiredOutputs(m_HullConstantFunction, RequiredTessFactorsRegisters, _countof(RequiredTessFactorsRegisters));

				checkExistenceOfEntrypoint(FunctionType::Types::DomainMain);
			}
			else if (Data.Stage == Stages::Domain)
			{
				checkExistenceOfEntrypoint(FunctionType::Types::HullMain);
			}
			else if (Data.Stage == Stages::Geometry)
			{
				if (Function->GetAttribute<MaxVertexCountAttributeType>() == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find MaxVertexCount attribute", Function->GetName());
				const PrimitiveTypeAttributeType* primitiveType = Function->GetAttribute<PrimitiveTypeAttributeType>();
				if (primitiveType == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find PrimitiveType attribute", Function->GetName());
				const OutputStreamTypeAttributeType* outputStreamType = Function->GetAttribute<OutputStreamTypeAttributeType>();
				if (outputStreamType == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find OutputStreamType attribute", Function->GetName());

				if (!Function->GetParameters()[0]->GetDataType()->IsArray())
					THROW_PROGRAM_COMPILER_EXCEPTION("Parameter of a Geometry entry point must be an array", Function->GetName());

				const uint8 ELEMENT_COUNT[]{ 1, 2, 3, 4, 6 };
				if (ELEMENT_COUNT[(uint32)primitiveType->GetType()] != StringUtility::ToUInt8(Function->GetParameters()[0]->GetDataType()->GetPostElementCount()->ToString()))
					THROW_PROGRAM_COMPILER_EXCEPTION("Element count of the Geometry entrypoint is not compatible with its primitive type", Function->GetName());

				if (GetStructType(outputStreamType->GetDataType()) == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find the DataType specified in the OutputStream attribute", Function->GetName());

				if (Function->GetReturnDataType()->GetType() != ProgramDataTypes::Void)
					THROW_PROGRAM_COMPILER_EXCEPTION("Geometry program must not return any value", Function->GetName());
			}
			else if (Data.Stage == Stages::Compute)
			{
				if (Function->GetAttribute<ThreadCountAttributeType>() == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find ThreadCount attribute", Function->GetName());

				if (Function->GetReturnDataType()->GetType() != ProgramDataTypes::Void)
					THROW_PROGRAM_COMPILER_EXCEPTION("Compute program must not return any value", Function->GetName());
			}
		}

		void ASTCompilerBase::BuildAttributes(const AttributeList& Attributes, StageData& Data)
		{
			for (auto attribute : Attributes)
			{
				BuildAttribute(attribute, Data);

				AddNewLine(Data);
			}
		}

		void ASTCompilerBase::BuildAttribute(const BaseAttributeType* Attribute, StageData& Data)
		{
			if (IsAssignableFrom(Attribute, const DomainAttributeType))
			{
				const DomainAttributeType* stm = ReinterpretCast(const DomainAttributeType*, Attribute);

				BuildDomainAttributeType(stm, Data);
			}
			else if (IsAssignableFrom(Attribute, const PartitioningAttributeType))
			{
				const PartitioningAttributeType* stm = ReinterpretCast(const PartitioningAttributeType*, Attribute);

				BuildPartitioningAttributeType(stm, Data);
			}
			else if (IsAssignableFrom(Attribute, const TopologyAttributeType))
			{
				const TopologyAttributeType* stm = ReinterpretCast(const TopologyAttributeType*, Attribute);

				BuildTopologyAttributeType(stm, Data);
			}
			else if (IsAssignableFrom(Attribute, const ControlPointsAttributeType))
			{
				const ControlPointsAttributeType* stm = ReinterpretCast(const ControlPointsAttributeType*, Attribute);

				BuildControlPointsAttributeType(stm, Data);
			}
			else if (IsAssignableFrom(Attribute, const ConstantEntrypointAttributeType))
			{
				const ConstantEntrypointAttributeType* stm = ReinterpretCast(const ConstantEntrypointAttributeType*, Attribute);

				BuildConstantEntrypointAttributeType(stm, Data);
			}
			else if (IsAssignableFrom(Attribute, const MaxVertexCountAttributeType))
			{
				const MaxVertexCountAttributeType* stm = ReinterpretCast(const MaxVertexCountAttributeType*, Attribute);

				BuildMaxVertexCountAttributeType(stm, Data);
			}
			else if (IsAssignableFrom(Attribute, const PrimitiveTypeAttributeType))
			{
				const PrimitiveTypeAttributeType* stm = ReinterpretCast(const PrimitiveTypeAttributeType*, Attribute);

				BuildPrimitiveTypeAttributeType(stm, Data);
			}
			else if (IsAssignableFrom(Attribute, const OutputStreamTypeAttributeType))
			{
				const OutputStreamTypeAttributeType* stm = ReinterpretCast(const OutputStreamTypeAttributeType*, Attribute);

				BuildOutputStreamTypeAttributeType(stm, Data);
			}
			else if (IsAssignableFrom(Attribute, const ThreadCountAttributeType))
			{
				const ThreadCountAttributeType* stm = ReinterpretCast(const ThreadCountAttributeType*, Attribute);

				BuildThreadCountAttributeType(stm, Data);
			}
		}

		void ASTCompilerBase::BuildParameters(const ParameterList& Parameters, StageData& Data)
		{
			bool isFirst = true;
			for (auto parameter : Parameters)
			{
				PushVariable(parameter);

				if (!isFirst)
					AddCode(", ", Data);
				isFirst = false;

				BuildParameter(parameter, Data);
			}
		}

		void ASTCompilerBase::BuildParameter(const ParameterType* Parameter, StageData& Data)
		{
			BuildDataTypeStatement(Parameter->GetDataType(), Data);
			AddCode(' ', Data);
			AddCode(Parameter->GetName(), Data);

			if (Parameter->GetDataType()->GetPostElementCount() != nullptr)
			{
				AddCode('[', Data);
				BuildStatement(Parameter->GetDataType()->GetPostElementCount(), Data);
				AddCode(']', Data);
			}
		}

		void ASTCompilerBase::BuildStatementHolder(const StatementItemHolder* Holder, bool IncreamentBlock, StageData& Data)
		{
			if (IncreamentBlock)
				IncreaseBlockIndex();

			for (auto statement : Holder->GetItems())
			{
				BuildStatement(statement, Data);

				if (IsAssignableFrom(statement, VariableStatement) ||
					IsAssignableFrom(statement, OperatorStatement) ||
					IsAssignableFrom(statement, UnaryOperatorStatement))
				{
					AddCode(';', Data);

					AddNewLine(Data);
				}
			}

			if (IncreamentBlock)
				DecreaseBlockIndex();
		}

		void ASTCompilerBase::BuildStatement(const Statement* Statement, StageData& Data)
		{
			if (IsAssignableFrom(Statement, const OperatorStatement))
			{
				const OperatorStatement* stm = ReinterpretCast(const OperatorStatement*, Statement);

				BuildOperatorStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const UnaryOperatorStatement))
			{
				const UnaryOperatorStatement* stm = ReinterpretCast(const UnaryOperatorStatement*, Statement);

				BuildUnaryOperatorStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const ConstantStatement))
			{
				const ConstantStatement* stm = ReinterpretCast(const ConstantStatement*, Statement);

				BuildConstantStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const FunctionCallStatement))
			{
				const FunctionCallStatement* stm = ReinterpretCast(const FunctionCallStatement*, Statement);

				BuildFunctionCallStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const VariableStatement))
			{
				const VariableStatement* stm = ReinterpretCast(const VariableStatement*, Statement);

				BuildVariableStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const VariableAccessStatement))
			{
				const VariableAccessStatement* stm = ReinterpretCast(const VariableAccessStatement*, Statement);

				BuildVariableAccessStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const ArrayElementAccessStatement))
			{
				const ArrayElementAccessStatement* stm = ReinterpretCast(const ArrayElementAccessStatement*, Statement);

				BuildArrayElementAccessStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const MemberAccessStatement))
			{
				const MemberAccessStatement* stm = ReinterpretCast(const MemberAccessStatement*, Statement);

				BuildMemberAccessStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const IfStatement))
			{
				const IfStatement* stm = ReinterpretCast(const IfStatement*, Statement);

				BuildIfStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const ElseStatement))
			{
				const ElseStatement* stm = ReinterpretCast(const ElseStatement*, Statement);

				BuildElseStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const SwitchStatement))
			{
				const SwitchStatement* stm = ReinterpretCast(const SwitchStatement*, Statement);

				BuildSwitchStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const CaseStatement))
			{
				const CaseStatement* stm = ReinterpretCast(const CaseStatement*, Statement);

				BuildCaseStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const DefaultStatement))
			{
				const DefaultStatement* stm = ReinterpretCast(const DefaultStatement*, Statement);

				BuildDefaultStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const ForStatement))
			{
				const ForStatement* stm = ReinterpretCast(const ForStatement*, Statement);

				BuildForStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const DoStatement))
			{
				const DoStatement* stm = ReinterpretCast(const DoStatement*, Statement);

				BuildDoStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const WhileStatement))
			{
				const WhileStatement* stm = ReinterpretCast(const WhileStatement*, Statement);

				BuildWhileStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const BreakStatement))
			{
				const BreakStatement* stm = ReinterpretCast(const BreakStatement*, Statement);

				BuildBreakStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const ReturnStatement))
			{
				const ReturnStatement* stm = ReinterpretCast(const ReturnStatement*, Statement);

				BuildReturnStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const ArrayStatement))
			{
				const ArrayStatement* stm = ReinterpretCast(const ArrayStatement*, Statement);

				BuildArrayStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, const DiscardStatement))
			{
				const DiscardStatement* stm = ReinterpretCast(const DiscardStatement*, Statement);

				BuildDiscardStatement(stm, Data);
			}
			else
				THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
		}

		void ASTCompilerBase::BuildOperatorStatement(const OperatorStatement* Statement, StageData& Data)
		{
			OperatorStatement::Operators op = Statement->GetOperator();

			if (op == OperatorStatement::Operators::Multiplication)
			{
				ProgramDataTypes dataType = EvaluateDataType(Statement->GetLeft()).GetType();

				if (dataType == ProgramDataTypes::Matrix4F ||
					dataType == ProgramDataTypes::Matrix4D)
				{
					IntrinsicsBuilder::BuildFunctionCallStatement("Multiply", { Statement->GetLeft(), Statement->GetRight() }, Data.FunctionType, Data.Stage, Data.Shader);

					return;
				}
			}
			else if (op == OperatorStatement::Operators::MultiplicationAssignment)
			{
				ProgramDataTypes dataType = EvaluateDataType(Statement->GetLeft()).GetType();

				if (dataType == ProgramDataTypes::Matrix4F ||
					dataType == ProgramDataTypes::Matrix4D)
				{
					BuildStatement(Statement->GetLeft(), Data);

					AddCode(" = ", Data);

					IntrinsicsBuilder::BuildFunctionCallStatement("Multiply", { Statement->GetLeft(), Statement->GetRight() }, Data.FunctionType, Data.Stage, Data.Shader);

					return;
				}
			}
			else if (op == OperatorStatement::Operators::Remainder)
			{
				IntrinsicsBuilder::BuildFunctionCallStatement("Reminder", { Statement->GetLeft(), Statement->GetRight() }, Data.FunctionType, Data.Stage, Data.Shader);

				return;
			}

			bool isAssignment =
				op == OperatorStatement::Operators::Assignment ||
				op == OperatorStatement::Operators::AdditionAssignment ||
				op == OperatorStatement::Operators::DivisionAssignment ||
				op == OperatorStatement::Operators::MultiplicationAssignment ||
				op == OperatorStatement::Operators::SubtractionAssignment;

			if (!isAssignment)
				AddCode('(', Data);

			BuildStatement(Statement->GetLeft(), Data);

			AddCode(' ', Data);

			AddCode(OperatorStatement::GetOperatorSymbol(op), Data);

			AddCode(' ', Data);

			if (isAssignment)
			{
				DataTypeStatement leftDataType = EvaluateDataType(Statement->GetLeft());
				BuildExplicitCast(Statement->GetRight(), &leftDataType, Data);
			}
			else
				BuildStatement(Statement->GetRight(), Data);

			if (!isAssignment)
				AddCode(')', Data);
		}

		void ASTCompilerBase::BuildUnaryOperatorStatement(const UnaryOperatorStatement* Statement, StageData& Data)
		{
			AddCode('(', Data);

			switch (Statement->GetOperator())
			{
			case UnaryOperatorStatement::Operators::Exlamation:
			case UnaryOperatorStatement::Operators::Minus:
			case UnaryOperatorStatement::Operators::PrefixIncrement:
			case UnaryOperatorStatement::Operators::PrefixDecrement:
				AddCode(UnaryOperatorStatement::GetOperatorSymbol(Statement->GetOperator()), Data);
				break;
			}

			BuildStatement(Statement->GetStatement(), Data);

			switch (Statement->GetOperator())
			{
			case UnaryOperatorStatement::Operators::PostfixIncrement:
			case UnaryOperatorStatement::Operators::PostfixDecrement:
				AddCode(UnaryOperatorStatement::GetOperatorSymbol(Statement->GetOperator()), Data);
				break;
			}

			AddCode(')', Data);
		}

		void ASTCompilerBase::BuildConstantStatement(const ConstantStatement* Statement, StageData& Data)
		{
			if (Statement->GetType() == ProgramDataTypes::Bool)
				AddCode(StringUtility::ToString<char8>(Statement->GetBool()), Data);
			else if (Statement->GetFloat32() == 0 || Statement->GetFloat32() / (int32)Statement->GetFloat32() == 1)
				AddCode(StringUtility::ToString<char8>((int32)Statement->GetFloat32()), Data);
			else
				AddCode(StringUtility::ToString<char8>(Statement->GetFloat32()), Data);
		}

		void ASTCompilerBase::BuildFunctionCallStatement(const FunctionCallStatement* Statement, StageData& Data)
		{
			auto& funcName = Statement->GetFunctionName();

			const FunctionType* functionType = FindMatchingFunction(funcName, Statement->GetArguments());
			if (functionType != nullptr)
			{
				AddCode(funcName, Data);

				AddCode('(', Data);

				uint8 i = 0;
				for (auto argument : Statement->GetArguments()->GetItems())
				{
					if (i != 0)
						AddCode(", ", Data);

					BuildExplicitCast(argument, functionType->GetParameters()[i++]->GetDataType(), Data);
				}

				AddCode(')', Data);

				return;
			}

			if (IntrinsicsBuilder::BuildFunctionCallStatement(Statement, Data.FunctionType, Data.Stage, Data.Shader))
				return;

			THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find variable or function", funcName);
		}

		void ASTCompilerBase::BuildArguments(const Vector<Statement*>& Statements, StageData& Data)
		{
			bool isFirst = true;
			for (auto argument : Statements)
			{
				if (!isFirst)
					AddCode(", ", Data);
				isFirst = false;

				BuildStatement(argument, Data);
			}
		}

		void ASTCompilerBase::BuildArguments(const StatementItemHolder* Statements, StageData& Data)
		{
			BuildArguments(Statements->GetItems(), Data);
		}

		void ASTCompilerBase::BuildVariableStatement(const VariableStatement* Statement, StageData& Data)
		{
			if (FindVariableType(Statement->GetName(), true))
				THROW_PROGRAM_COMPILER_EXCEPTION("Variable redifinition", Statement->GetName());

			PushVariable(Statement);

			BuildDataTypeStatement(Statement->GetDataType(), Data);

			AddCode(' ', Data);
			AddCode(Statement->GetName(), Data);

			AddCode(" = ", Data);

			if (Statement->GetInitialStatement() == nullptr)
				BuildDataTypeInitializer(Statement->GetDataType(), Data);
			else
				BuildStatement(Statement->GetInitialStatement(), Data);
		}

		void ASTCompilerBase::BuildVariableAccessStatement(const VariableAccessStatement* Statement, StageData& Data)
		{
			CheckVariableExsitence(Statement->GetName());

			AddCode(Statement->GetName(), Data);
		}

		void ASTCompilerBase::BuildArrayElementAccessStatement(const ArrayElementAccessStatement* Statement, StageData& Data)
		{
			BuildStatement(Statement->GetArrayStatement(), Data);

			AddCode('[', Data);

			DataTypeStatement elementIndexDataType = EvaluateDataType(Statement->GetElementStatement());
			if (elementIndexDataType.GetType() != ProgramDataTypes::Integer && elementIndexDataType.GetType() != ProgramDataTypes::UnsignedInteger)
				THROW_PROGRAM_COMPILER_EXCEPTION("Invalid type of index", elementIndexDataType.ToString());

			BuildStatement(Statement->GetElementStatement(), Data);

			AddCode(']', Data);
		}

		void ASTCompilerBase::BuildMemberAccessStatement(const MemberAccessStatement* Statement, StageData& Data)
		{
			BuildStatement(Statement->GetLeft(), Data);

			AddCode('.', Data);

			PushDataAccessStatement(Statement->GetLeft());

			DataTypeStatement dataType = EvaluateDataType(Statement->GetLeft());
			if (!dataType.IsBuiltIn())
				IncreaseBlockIndexAndPushStructVariables(FindStructType(dataType.GetUserDefined()));

			BuildStatement(Statement->GetRight(), Data);

			if (!dataType.IsBuiltIn())
				DecreaseBlockIndex();

			PopDataAceessStatement();
		}

		void ASTCompilerBase::BuildIfStatement(const IfStatement* Statement, StageData& Data)
		{
			AddCode("if (", Data);

			BuildExplicitCast(Statement->GetCondition(), ProgramDataTypes::Bool, Data);

			AddCode(')', Data);

			AddNewLine(Data);

			AddCode('{', Data);

			AddNewLine(Data);

			BuildStatementHolder(Statement, true, Data);

			AddCode('}', Data);

			AddNewLine(Data);

			if (Statement->GetElse() != nullptr)
				BuildStatement(Statement->GetElse(), Data);
		}

		void ASTCompilerBase::BuildElseStatement(const ElseStatement* Statement, StageData& Data)
		{
			AddCode("else", Data);

			AddNewLine(Data);

			AddCode('{', Data);

			AddNewLine(Data);

			BuildStatementHolder(Statement, true, Data);

			AddCode('}', Data);

			AddNewLine(Data);
		}

		void ASTCompilerBase::BuildSwitchStatement(const SwitchStatement* Statement, StageData& Data)
		{
			AddCode("switch (", Data);

			BuildStatement(Statement->GetSelector(), Data);

			AddCode(')', Data);
			AddNewLine(Data);
			AddCode('{', Data);
			AddNewLine(Data);

			BuildStatementHolder(Statement, true, Data);

			AddCode('}', Data);
			AddNewLine(Data);
		}

		void ASTCompilerBase::BuildCaseStatement(const CaseStatement* Statement, StageData& Data)
		{
			AddCode("case ", Data);

			BuildStatement(Statement->GetCondition(), Data);

			AddCode(':', Data);
			AddNewLine(Data);

			if (Statement->GetItems().GetSize() != 0)
			{
				AddCode('{', Data);
				AddNewLine(Data);

				Data.IndentOffset++;
				BuildStatementHolder(Statement, false, Data);
				--Data.IndentOffset;

				AddCode('}', Data);
				AddNewLine(Data);
			}
		}

		void ASTCompilerBase::BuildDefaultStatement(const DefaultStatement* Statement, StageData& Data)
		{
			AddCode("default:", Data);
			AddNewLine(Data);

			if (Statement->GetItems().GetSize() != 0)
			{
				AddCode('{', Data);
				AddNewLine(Data);

				Data.IndentOffset++;
				BuildStatementHolder(Statement, false, Data);
				--Data.IndentOffset;

				AddCode('}', Data);
				AddNewLine(Data);
			}
		}

		void ASTCompilerBase::BuildForStatement(const ForStatement* Statement, StageData& Data)
		{
			AddCode("for (", Data);

			if (Statement->GetInitializer() != nullptr)
				BuildStatement(Statement->GetInitializer(), Data);

			AddCode(';', Data);

			if (Statement->GetCondition() != nullptr)
				BuildExplicitCast(Statement->GetCondition(), ProgramDataTypes::Bool, Data);

			AddCode(';', Data);

			if (Statement->GetStep() != nullptr)
				BuildStatement(Statement->GetStep(), Data);

			AddCode(')', Data);

			if (Statement->GetItems().GetSize() == 0)
			{
				AddCode(';', Data);
				AddNewLine(Data);

				return;
			}

			AddNewLine(Data);

			AddCode('{', Data);
			AddNewLine(Data);

			BuildStatementHolder(Statement, true, Data);

			AddCode('}', Data);
			AddNewLine(Data);
		}

		void ASTCompilerBase::BuildDoStatement(const DoStatement* Statement, StageData& Data)
		{
			AddCode("do", Data);
			AddNewLine(Data);

			AddCode('{', Data);
			AddNewLine(Data);

			BuildStatementHolder(Statement, true, Data);

			AddCode("} ", Data);

			BuildStatement(Statement->GetWhile(), Data);
		}

		void ASTCompilerBase::BuildWhileStatement(const WhileStatement* Statement, StageData& Data)
		{
			AddCode("while (", Data);

			BuildExplicitCast(Statement->GetCondition(), ProgramDataTypes::Bool, Data);

			AddCode(')', Data);

			if (Statement->GetItems().GetSize() == 0)
			{
				AddCode(';', Data);
				AddNewLine(Data);

				return;
			}

			AddNewLine(Data);

			AddCode('{', Data);
			AddNewLine(Data);

			BuildStatementHolder(Statement, true, Data);

			AddCode('}', Data);
			AddNewLine(Data);
		}

		void ASTCompilerBase::BuildContinueStatement(const ContinueStatement* Statement, StageData& Data)
		{
			AddCode("continue;", Data);
			AddNewLine(Data);
		}

		void ASTCompilerBase::BuildBreakStatement(const BreakStatement* Statement, StageData& Data)
		{
			AddCode("break;", Data);
			AddNewLine(Data);
		}

		void ASTCompilerBase::BuildDiscardStatement(const DiscardStatement* Statement, StageData& Data)
		{
			if (Data.Stage != Stages::Fragment)
				THROW_PROGRAM_COMPILER_EXCEPTION("Not a valid statement in this stage", Statement->ToString());

			AddCode("discard;", Data);
			AddNewLine(Data);
		}

		void ASTCompilerBase::BuildDataTypeStatement(const DataTypeStatement* Statement, StageData& Data)
		{
			if (Statement->GetTemplateElementDataType() != nullptr)
				BuildPreTemplateDataTypeStatement(Statement->GetTemplateElementDataType(), Data);

			if (Statement->IsBuiltIn())
				BuildType(Statement->GetType(), Data);
			else
				AddCode(Statement->GetUserDefined(), Data);

			if (Statement->GetTemplateElementDataType() != nullptr)
				BuildPostTemplateDataTypeStatement(Statement->GetTemplateElementDataType(), Data);

			if (Statement->GetElementCount() != nullptr)
			{
				AddCode('[', Data);
				BuildStatement(Statement->GetElementCount(), Data);
				AddCode(']', Data);
			}
		}

		void ASTCompilerBase::BuildPreTemplateDataTypeStatement(const DataTypeStatement* Statement, StageData& Data)
		{
		}

		void ASTCompilerBase::BuildPostTemplateDataTypeStatement(const DataTypeStatement* Statement, StageData& Data)
		{
		}

		void ASTCompilerBase::BuildExplicitCast(const Statement* Statement, const DataTypeStatement* DataType, StageData& Data)
		{
			DataTypeStatement sourceDataType = EvaluateDataType(Statement);
			bool needsCasting = !CompareDataTypes(sourceDataType, *DataType);

			if (needsCasting)
			{
				CheckForImplicitCast(sourceDataType, *DataType);

				AddCode('(', Data);

				BuildDataTypeStatement(DataType, Data);

				AddCode(")(", Data);
			}

			BuildStatement(Statement, Data);

			if (needsCasting)
				AddCode(')', Data);
		}

		void ASTCompilerBase::BuildExplicitCast(const Statement* Statement, ProgramDataTypes DataType, StageData& Data)
		{
			DataTypeStatement dataType = DataType;

			BuildExplicitCast(Statement, &dataType, Data);
		}

		void ASTCompilerBase::BuildDataTypeInitializer(const DataTypeStatement* DataType, StageData& Data)
		{
			if (DataType->IsArray())
			{
				AddCode("{ ", Data);

				uint16 elementCount = StringUtility::ToUInt16(StatementToString(DataType->GetPostElementCount(), Data));
				for (uint16 i = 0; i < elementCount; ++i)
				{
					if (i != 0)
						AddCode(", ", Data);

					DataTypeStatement elementDataType = *DataType;
					elementDataType.SetPostElementCount(nullptr);

					BuildDataTypeInitializer(&elementDataType, Data);
				}

				AddCode(" }", Data);

				return;
			}

			if (DataType->IsBuiltIn())
			{
				switch (DataType->GetType())
				{
				case ProgramDataTypes::Bool:
				{
					AddCode("false", Data);
				}
				break;

				case ProgramDataTypes::Integer:
				case ProgramDataTypes::UnsignedInteger:
				case ProgramDataTypes::Float:
				case ProgramDataTypes::Double:
				case ProgramDataTypes::Integer2:
				case ProgramDataTypes::UnsignedInteger2:
				case ProgramDataTypes::Float2:
				case ProgramDataTypes::Double2:
				case ProgramDataTypes::Integer3:
				case ProgramDataTypes::UnsignedInteger3:
				case ProgramDataTypes::Float3:
				case ProgramDataTypes::Double3:
				case ProgramDataTypes::Integer4:
				case ProgramDataTypes::UnsignedInteger4:
				case ProgramDataTypes::Float4:
				case ProgramDataTypes::Double4:
				case ProgramDataTypes::Matrix4F:
				case ProgramDataTypes::Matrix4D:
				{
					BuildType(DataType->GetType(), Data);

					AddCode('(', Data);

					uint8 componentCount = DataType->GetComponentCount();
					for (uint8 i = 0; i < componentCount; ++i)
					{
						if (i != 0)
							AddCode(", ", Data);

						AddCode('0', Data);
					}

					AddCode(')', Data);
				}
				break;

				default:
					THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
				}

				return;
			}

			AddCode("{ ", Data);

			const StructType* structType = GetStructType(DataType->GetUserDefined());

			bool isFirst = true;
			for (const auto& variable : structType->GetItems())
			{
				if (!isFirst)
					AddCode(", ", Data);
				isFirst = false;

				BuildDataTypeInitializer(variable->GetDataType(), Data);
			}

			AddCode(" }", Data);
		}

		DataTypeStatement ASTCompilerBase::EvaluateDataType(const Statement* CurrentStatement, const Statement* TopStatement) const
		{
			static ProgramDataTypes MULTIPLY_RESULT[(uint8)ProgramDataTypes::Unknown][(uint8)ProgramDataTypes::Unknown] =
			{
				//						Void							Bool							Integer							UnsignedInteger						Float							Double							Integer2						UnsignedInteger2					Float2							Double2							Integer3						UnsignedInteger3					Float3							Double3							Integer4						UnsignedInteger4					Float4							Double4							Matrix4F						Matrix4D
				/*Void*/				{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown	},
				/*Bool*/				{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown	},
				/*Integer*/				{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Integer,		ProgramDataTypes::Integer,			ProgramDataTypes::Float,		ProgramDataTypes::Double,		ProgramDataTypes::Integer2,		ProgramDataTypes::Integer2,			ProgramDataTypes::Float2,		ProgramDataTypes::Double2,		ProgramDataTypes::Integer3,		ProgramDataTypes::Integer3,			ProgramDataTypes::Float3,		ProgramDataTypes::Double3,		ProgramDataTypes::Integer4,		ProgramDataTypes::Integer4,			ProgramDataTypes::Float4,		ProgramDataTypes::Double4,		ProgramDataTypes::Matrix4F,		ProgramDataTypes::Matrix4D	},
				/*UnsignedInteger*/		{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Integer,		ProgramDataTypes::UnsignedInteger,	ProgramDataTypes::Float,		ProgramDataTypes::Double,		ProgramDataTypes::Integer2,		ProgramDataTypes::UnsignedInteger2,	ProgramDataTypes::Float2,		ProgramDataTypes::Double2,		ProgramDataTypes::Integer3,		ProgramDataTypes::UnsignedInteger3,	ProgramDataTypes::Float3,		ProgramDataTypes::Double3,		ProgramDataTypes::Integer4,		ProgramDataTypes::UnsignedInteger4,	ProgramDataTypes::Float4,		ProgramDataTypes::Double4,		ProgramDataTypes::Matrix4F,		ProgramDataTypes::Matrix4D	},
				/*Float*/				{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Float,		ProgramDataTypes::Float,			ProgramDataTypes::Float,		ProgramDataTypes::Double,		ProgramDataTypes::Float2,		ProgramDataTypes::Float2,			ProgramDataTypes::Float2,		ProgramDataTypes::Double2,		ProgramDataTypes::Float3,		ProgramDataTypes::Float3,			ProgramDataTypes::Float3,		ProgramDataTypes::Double3,		ProgramDataTypes::Float4,		ProgramDataTypes::Float4,			ProgramDataTypes::Float4,		ProgramDataTypes::Double4,		ProgramDataTypes::Matrix4F,		ProgramDataTypes::Matrix4D	},
				/*Double*/				{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Double,		ProgramDataTypes::Double,			ProgramDataTypes::Double,		ProgramDataTypes::Double,		ProgramDataTypes::Double2,		ProgramDataTypes::Double2,			ProgramDataTypes::Double2,		ProgramDataTypes::Double2,		ProgramDataTypes::Double3,		ProgramDataTypes::Double3,			ProgramDataTypes::Double3,		ProgramDataTypes::Double3,		ProgramDataTypes::Double4,		ProgramDataTypes::Double4,			ProgramDataTypes::Double4,		ProgramDataTypes::Double4,		ProgramDataTypes::Matrix4F,		ProgramDataTypes::Matrix4D	},
				/*Integer2*/			{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Integer,		ProgramDataTypes::Integer,			ProgramDataTypes::Float,		ProgramDataTypes::Double,		ProgramDataTypes::Integer2,		ProgramDataTypes::Integer2,			ProgramDataTypes::Float2,		ProgramDataTypes::Double2,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Float3,		ProgramDataTypes::Double3,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Float4,		ProgramDataTypes::Double4,		ProgramDataTypes::Matrix4F,		ProgramDataTypes::Unknown	},
				/*UnsignedInteger2*/	{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Integer,		ProgramDataTypes::UnsignedInteger,	ProgramDataTypes::Float,		ProgramDataTypes::Double,		ProgramDataTypes::Integer2,		ProgramDataTypes::UnsignedInteger2,	ProgramDataTypes::Float2,		ProgramDataTypes::Double2,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Float3,		ProgramDataTypes::Double3,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Float4,		ProgramDataTypes::Double4,		ProgramDataTypes::Matrix4F,		ProgramDataTypes::Unknown	},
				/*Float2*/				{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Float2,		ProgramDataTypes::Float2,			ProgramDataTypes::Float2,		ProgramDataTypes::Double2,		ProgramDataTypes::Float2,		ProgramDataTypes::Float2,			ProgramDataTypes::Float2,		ProgramDataTypes::Double2,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown	},
				/*Double2*/				{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Double2,		ProgramDataTypes::Double2,			ProgramDataTypes::Double2,		ProgramDataTypes::Double2,		ProgramDataTypes::Double2,		ProgramDataTypes::Double2,			ProgramDataTypes::Double2,		ProgramDataTypes::Double2,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown	},
				/*Integer3*/			{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Integer,		ProgramDataTypes::Integer,			ProgramDataTypes::Float,		ProgramDataTypes::Double,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Float2,		ProgramDataTypes::Double2,		ProgramDataTypes::Integer3,		ProgramDataTypes::Integer3,			ProgramDataTypes::Float3,		ProgramDataTypes::Double3,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Float4,		ProgramDataTypes::Double4,		ProgramDataTypes::Matrix4F,		ProgramDataTypes::Unknown	},
				/*UnsignedInteger3*/	{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Integer,		ProgramDataTypes::UnsignedInteger,	ProgramDataTypes::Float,		ProgramDataTypes::Double,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Float2,		ProgramDataTypes::Double2,		ProgramDataTypes::Integer3,		ProgramDataTypes::UnsignedInteger3,	ProgramDataTypes::Float3,		ProgramDataTypes::Double3,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Float4,		ProgramDataTypes::Double4,		ProgramDataTypes::Matrix4F,		ProgramDataTypes::Unknown	},
				/*Float3*/				{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Float3,		ProgramDataTypes::Float3,			ProgramDataTypes::Float3,		ProgramDataTypes::Double3,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Float3,		ProgramDataTypes::Float3,			ProgramDataTypes::Float3,		ProgramDataTypes::Double3,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown	},
				/*Double3*/				{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Double3,		ProgramDataTypes::Double3,			ProgramDataTypes::Double3,		ProgramDataTypes::Double3,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Double3,		ProgramDataTypes::Double3,			ProgramDataTypes::Double3,		ProgramDataTypes::Double3,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown	},
				/*Integer4*/			{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Integer,		ProgramDataTypes::Integer,			ProgramDataTypes::Float,		ProgramDataTypes::Double,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Float2,		ProgramDataTypes::Double2,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Float3,		ProgramDataTypes::Double3,		ProgramDataTypes::Integer4,		ProgramDataTypes::Integer4,			ProgramDataTypes::Float4,		ProgramDataTypes::Double4,		ProgramDataTypes::Matrix4F,		ProgramDataTypes::Matrix4D	},
				/*UnsignedInteger4*/	{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Integer,		ProgramDataTypes::UnsignedInteger,	ProgramDataTypes::Float,		ProgramDataTypes::Double,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Float2,		ProgramDataTypes::Double2,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Float3,		ProgramDataTypes::Double3,		ProgramDataTypes::Integer4,		ProgramDataTypes::UnsignedInteger4,	ProgramDataTypes::Float4,		ProgramDataTypes::Double4,		ProgramDataTypes::Matrix4F,		ProgramDataTypes::Matrix4D	},
				/*Float4*/				{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Float4,		ProgramDataTypes::Float4,			ProgramDataTypes::Float4,		ProgramDataTypes::Double4,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Float4,		ProgramDataTypes::Double4,		ProgramDataTypes::Float3,		ProgramDataTypes::Float3,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Matrix4F,		ProgramDataTypes::Matrix4D	},
				/*Double4*/				{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Double4,		ProgramDataTypes::Double4,			ProgramDataTypes::Double4,		ProgramDataTypes::Double4,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Double4,		ProgramDataTypes::Double4,		ProgramDataTypes::Double3,		ProgramDataTypes::Double3,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Matrix4F,		ProgramDataTypes::Matrix4D	},
				/*Matrix4F*/			{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Matrix4F,		ProgramDataTypes::Matrix4F,			ProgramDataTypes::Matrix4F,		ProgramDataTypes::Matrix4F,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Float4,		ProgramDataTypes::Double4,		ProgramDataTypes::Matrix4F,		ProgramDataTypes::Matrix4D	},
				/*Matrix4D*/			{ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Matrix4D,		ProgramDataTypes::Matrix4D,			ProgramDataTypes::Matrix4D,		ProgramDataTypes::Matrix4D,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,		ProgramDataTypes::Unknown,			ProgramDataTypes::Float4,		ProgramDataTypes::Double4,		ProgramDataTypes::Matrix4D,		ProgramDataTypes::Matrix4D	},
			};

			if (IsAssignableFrom(CurrentStatement, const OperatorStatement))
			{
				const OperatorStatement* stm = ReinterpretCast(const OperatorStatement*, CurrentStatement);

				DataTypeStatement leftType = EvaluateDataType(stm->GetLeft());
				DataTypeStatement rightType = EvaluateDataType(stm->GetRight());

				OperatorStatement::Operators op = stm->GetOperator();

				switch (op)
				{
				case OperatorStatement::Operators::Multiplication:
				case OperatorStatement::Operators::Division:
					return MULTIPLY_RESULT[(uint8)leftType.GetType()][(uint8)rightType.GetType()];

				case OperatorStatement::Operators::Remainder:
				case OperatorStatement::Operators::Addition:
				case OperatorStatement::Operators::Subtraction:
					return leftType;

				case OperatorStatement::Operators::EqualCheck:
				case OperatorStatement::Operators::NotEqualCheck:
				case OperatorStatement::Operators::LessCheck:
				case OperatorStatement::Operators::LessEqualCheck:
				case OperatorStatement::Operators::GreaterCheck:
				case OperatorStatement::Operators::GreaterEqualCheck:
				case OperatorStatement::Operators::LogicalAnd:
				case OperatorStatement::Operators::LogicalOr:
					return ProgramDataTypes::Bool;

				case OperatorStatement::Operators::Assignment:
				case OperatorStatement::Operators::AdditionAssignment:
				case OperatorStatement::Operators::SubtractionAssignment:
				case OperatorStatement::Operators::MultiplicationAssignment:
				case OperatorStatement::Operators::DivisionAssignment:
					return leftType;
				}
			}
			else if (IsAssignableFrom(CurrentStatement, const UnaryOperatorStatement))
			{
				const UnaryOperatorStatement* stm = ReinterpretCast(const UnaryOperatorStatement*, CurrentStatement);

				return EvaluateDataType(stm->GetStatement());
			}
			else if (IsAssignableFrom(CurrentStatement, const ConstantStatement))
			{
				const ConstantStatement* stm = ReinterpretCast(const ConstantStatement*, CurrentStatement);

				return stm->GetType();
			}
			else if (IsAssignableFrom(CurrentStatement, const FunctionCallStatement))
			{
				const FunctionCallStatement* stm = ReinterpretCast(const FunctionCallStatement*, CurrentStatement);

				const FunctionType* functionType = FindMatchingFunction(stm->GetFunctionName(), stm->GetArguments());
				if (functionType != nullptr)
					return *(functionType->GetReturnDataType());

				ProgramDataTypes dataType = IntrinsicsBuilder::EvaluateFunctionReturnValue(stm);
				if (dataType == ProgramDataTypes::Unknown)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't evaluate result of the statement", stm->ToString());

				return dataType;
			}
			else if (IsAssignableFrom(CurrentStatement, const VariableAccessStatement))
			{
				const VariableAccessStatement* stm = ReinterpretCast(const VariableAccessStatement*, CurrentStatement);

				const String& variableName = stm->GetName();

				const VariableType* variableType = FindVariableType(variableName);
				if (variableType != nullptr)
					return *variableType->GetDataType();

				if (TopStatement != nullptr)
				{
					DataTypeStatement topDataType = EvaluateDataType(TopStatement);

					const StructType* structType = FindStructType(topDataType.GetUserDefined());
					if (structType != nullptr)
					{
						const VariableType* variableType = FindVariableType(structType, variableName);
						if (variableType != nullptr)
							return *variableType->GetDataType();
					}
				}

				THROW_PROGRAM_COMPILER_EXCEPTION("Variable not defined", variableName);
			}
			else if (IsAssignableFrom(CurrentStatement, const ArrayElementAccessStatement))
			{
				const ArrayElementAccessStatement* stm = ReinterpretCast(const ArrayElementAccessStatement*, CurrentStatement);

				return EvaluateDataType(stm->GetArrayStatement());
			}
			else if (IsAssignableFrom(CurrentStatement, const MemberAccessStatement))
			{
				const MemberAccessStatement* stm = ReinterpretCast(const MemberAccessStatement*, CurrentStatement);

				DataTypeStatement leftDataType = EvaluateDataType(stm->GetLeft(), TopStatement);

				if (leftDataType.IsBuiltIn())
				{
					ProgramDataTypes leftType = leftDataType.GetType();
					uint8 advance = (stm->GetRight()->ToString().GetLength() - 1) * 2;

					if (leftType == ProgramDataTypes::Float2 ||
						leftType == ProgramDataTypes::Float3 ||
						leftType == ProgramDataTypes::Float4)
						return (ProgramDataTypes)((uint8)ProgramDataTypes::Float + advance);

					if (leftType == ProgramDataTypes::Double2 ||
						leftType == ProgramDataTypes::Double3 ||
						leftType == ProgramDataTypes::Double4)
						return (ProgramDataTypes)((uint8)ProgramDataTypes::Double + advance);

					if (leftType == ProgramDataTypes::Matrix4F ||
						leftType == ProgramDataTypes::Matrix4D)
						return ProgramDataTypes::Float;
				}
				else if (IsAssignableFrom(stm->GetRight(), VariableAccessStatement))
				{
					const StructType* structType = FindStructType(leftDataType.GetUserDefined());
					if (structType != nullptr)
					{
						const VariableType* variableType = FindVariableType(structType, stm->GetRight()->ToString());
						if (variableType != nullptr)
							return *variableType->GetDataType();
					}
				}
				else
					return EvaluateDataType(stm->GetRight(), stm->GetLeft());
			}
			else if (IsAssignableFrom(CurrentStatement, const ArrayStatement))
			{
				const ArrayStatement* stm = ReinterpretCast(const ArrayStatement*, CurrentStatement);

				if (stm->GetItems().GetSize() == 0)
					return ProgramDataTypes::Unknown;

				return EvaluateDataType(stm->GetItems()[0]);
			}

			return ProgramDataTypes::Unknown;
		}

		ProgramDataTypes ASTCompilerBase::EvaluateProgramDataType(const Statement* Statement) const
		{
			return EvaluateDataType(Statement).GetType();
		}

		bool ASTCompilerBase::CompareDataTypes(const DataTypeStatement& Left, const DataTypeStatement& Right) const
		{
			if (Left.IsBuiltIn() != Right.IsBuiltIn())
				return false;

			if (Left.IsArray() != Right.IsArray())
				return false;

			if (Left.IsArray() && (Right.GetElementCount() != Right.GetElementCount()))
				return false;

			if (Left.IsArray() && (Right.GetPostElementCount() != Right.GetPostElementCount()))
				return false;

			if (Left.GetUserDefined() != Right.GetUserDefined())
				return false;

			if (Left.GetType() != Right.GetType())
				return false;

			return true;
		}

		void ASTCompilerBase::CheckForImplicitCast(const DataTypeStatement& Source, const DataTypeStatement& Destination) const
		{
			if (Source.IsBuiltIn())
				return;

			if (Source.GetComponentCount() == 1)
				return;

			if (Source.GetComponentCount() > -Destination.GetComponentCount())
				return;

			THROW_PROGRAM_COMPILER_EXCEPTION("Casting is not applicable", Source.ToString() + " to " + Destination.ToString());
		}

		void ASTCompilerBase::CheckVariableExsitence(const String& Name) const
		{
			auto checkVariableInParent = [this, &Name](const DataTypeStatement* ParentDataType)
			{
				if (ParentDataType->IsBuiltIn())
				{
					auto checkSwizzling = [&Name](const char8* ValidSwizzles, uint8 Length)
					{
						for (uint8 i = 0; i < Name.GetLength(); ++i)
						{
							char8 c = Name[i];

							bool found = false;
							for (uint8 j = 0; j < Length; ++j)
							{
								if (c != ValidSwizzles[j])
									continue;

								found = true;
								break;
							}
							if (found)
								continue;

							THROW_PROGRAM_COMPILER_EXCEPTION("Invalid swizzle", String(c));
						}
					};

					switch (ParentDataType->GetType())
					{
					case ProgramDataTypes::Void:
					case ProgramDataTypes::Bool:
					case ProgramDataTypes::Integer:
					case ProgramDataTypes::UnsignedInteger:
					case ProgramDataTypes::Float:
					case ProgramDataTypes::Double:
					case ProgramDataTypes::Texture1D:
					case ProgramDataTypes::Texture2D:
					case ProgramDataTypes::Texture3D:
					case ProgramDataTypes::TextureCube:
					case ProgramDataTypes::Texture1DRW:
					case ProgramDataTypes::Texture2DRW:
					case ProgramDataTypes::Texture3DRW:
						THROW_PROGRAM_COMPILER_EXCEPTION("Data type hasn't any member", ParentDataType->ToString());

					case ProgramDataTypes::Integer2:
					case ProgramDataTypes::UnsignedInteger2:
					case ProgramDataTypes::Float2:
					case ProgramDataTypes::Double2:
					{
						const char8 VALID_SWIZZLES[] = { 'x', 'y', 'r', 'g', 's', 't' };

						checkSwizzling(VALID_SWIZZLES, _countof(VALID_SWIZZLES));

						return true;
					}

					case ProgramDataTypes::Integer3:
					case ProgramDataTypes::UnsignedInteger3:
					case ProgramDataTypes::Float3:
					case ProgramDataTypes::Double3:
					{
						const char8 VALID_SWIZZLES[] = { 'x', 'y', 'z', 'r', 'g', 'b', 's', 't', 'p' };

						checkSwizzling(VALID_SWIZZLES, _countof(VALID_SWIZZLES));

						return true;
					}

					case ProgramDataTypes::Integer4:
					case ProgramDataTypes::UnsignedInteger4:
					case ProgramDataTypes::Float4:
					case ProgramDataTypes::Double4:
					{
						const char8 VALID_SWIZZLES[] = { 'x', 'y', 'z', 'w', 'r', 'g', 'b', 'a', 's', 't', 'p', 'q' };

						checkSwizzling(VALID_SWIZZLES, _countof(VALID_SWIZZLES));

						return true;
					}

					case ProgramDataTypes::Matrix4F:
					case ProgramDataTypes::Matrix4D:
						break;
					}
				}
				else
				{
					return (FindVariableType(FindStructType(ParentDataType->GetUserDefined()), Name) != nullptr);
				}

				return false;
			};

			if (m_DataAccessStatements.GetSize() != 0)
			{
				const Statement* statement = m_DataAccessStatements[0];

				if (IsAssignableFrom(statement, const VariableAccessStatement))
				{
					const VariableAccessStatement* stm = ReinterpretCast(const VariableAccessStatement*, statement);

					const VariableType* type = FindVariableType(stm->GetName());
					if (type != nullptr && checkVariableInParent(type->GetDataType()))
						return;
				}
				else if (IsAssignableFrom(statement, const ArrayElementAccessStatement))
				{
					const ArrayElementAccessStatement* stm = ReinterpretCast(const ArrayElementAccessStatement*, statement);

					DataTypeStatement dataType = EvaluateDataType(stm->GetArrayStatement());

					if (checkVariableInParent((dataType.GetTemplateElementDataType() == nullptr ? &dataType : dataType.GetTemplateElementDataType())))
						return;
				}
			}

			if (FindVariableType(Name) != nullptr)
				return;

			THROW_PROGRAM_COMPILER_EXCEPTION("Variable not defined", Name);
		}

		const VariableType* ASTCompilerBase::FindVariableType(const String& Name, bool LatestBlockOnly) const
		{
			for (int8 i = m_BlockVariables.GetSize() - 1; i >= 0; --i)
			{
				const auto& variables = m_BlockVariables[i];

				int32 index = variables.FindIf([&Name](auto item) { return item->GetName() == Name; });
				if (index == -1)
				{
					if (LatestBlockOnly)
						break;

					continue;
				}

				return variables[index];
			}

			return nullptr;
		}

		void ASTCompilerBase::IncreaseBlockIndex(void)
		{
			CoreDebugAssert(Categories::ProgramCompiler, m_BlockIndex + 1 == m_BlockVariables.GetSize(), "Mismatch detected");

			++m_BlockIndex;

			m_BlockVariables.Add(VariableList(m_BlockVariables.GetAllocator()));
		}

		void ASTCompilerBase::DecreaseBlockIndex(void)
		{
			CoreDebugAssert(Categories::ProgramCompiler, m_BlockIndex + 1 == m_BlockVariables.GetSize(), "Mismatch detected");

			m_BlockVariables.RemoveAt(m_BlockIndex--);
		}

		void ASTCompilerBase::PushVariable(const VariableType* Variable)
		{
			m_BlockVariables[m_BlockIndex].Add(ConstCast(VariableType*, Variable));
		}

		void ASTCompilerBase::IncreaseBlockIndexAndPushStructVariables(const StructType* Struct)
		{
			IncreaseBlockIndex();

			for (const auto& variable : Struct->GetItems())
				PushVariable(variable);
		}

		const FunctionType* ASTCompilerBase::FindFunctionType(const String& Name) const
		{
			int32 index = m_Functions.FindIf([&Name](auto structType) { return structType->GetName() == Name; });
			if (index == -1)
				return nullptr;

			return m_Functions[index];
		}

		const FunctionType* ASTCompilerBase::FindMatchingFunction(const String& Name, const StatementItemHolder* Arguments) const
		{
			int32 index = m_Functions.FindIf([&](auto item)
				{
					if (item->GetName() != Name)
					return false;

			const auto& parameters = item->GetParameters();

			if (parameters.GetSize() != Arguments->GetItems().GetSize())
				return false;

			for (uint8 i = 0; i < parameters.GetSize(); ++i)
			{
				const auto& parameter = parameters[i];
				const auto& argument = Arguments->GetItems()[i];

				const DataTypeStatement argumentDataType = EvaluateDataType(argument);
				if (!CompareDataTypes(*parameter->GetDataType(), argumentDataType))
				{
					if (parameter->GetDataType()->IsNumeric() != argumentDataType.IsNumeric())
						return false;
				}
			}

			return true;
				});

			if (index == -1)
			{
				if (FindFunctionType(Name) != nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find a matching version of function with the parameters", Name);

				return nullptr;
			}

			return m_Functions[index];
		}

		const FunctionType* ASTCompilerBase::GetFunctionType(const String& Name) const
		{
			const FunctionType* type = FindFunctionType(Name);
			if (type == nullptr)
				THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find function", Name);

			return type;
		}

		const FunctionType* ASTCompilerBase::GetEntrypointFunctionType(FunctionType::Types Type, StageData& Data) const
		{
			CoreDebugAssert(Categories::ProgramCompiler, Type != FunctionType::Types::None, "Type cannot be None");

			int32 index = Data.Functions.FindIf([Type](auto& item) { return item->GetType() == Type; });
			if (index == -1)
				THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find entrypoint function", String::Empty);

			return Data.Functions[index];
		}

		const StructType* ASTCompilerBase::FindStructType(const String& Name) const
		{
			int32 index = m_Structs.FindIf([&Name](auto structType) { return structType->GetName() == Name; });
			if (index == -1)
				return nullptr;

			return m_Structs[index];
		}

		const StructType* ASTCompilerBase::GetStructType(const String& Name) const
		{
			const StructType* type = FindStructType(Name);
			if (type == nullptr)
				THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find struct", Name);

			return type;
		}

		const StructVariableType* ASTCompilerBase::FindVariableType(const StructType* StructType, const String& Name) const
		{
			return FindVariableType(StructType, [&Name](auto variableType) { return variableType->GetName() == Name; });
		}

		const StructVariableType* ASTCompilerBase::GetVariableType(const StructType* StructType, const String& Name) const
		{
			const StructVariableType* type = FindVariableType(StructType, Name);
			if (type == nullptr)
				THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find the variable", StructType->GetName() + "::" + Name);

			return type;
		}

		const StructVariableType* ASTCompilerBase::FindVariableType(const StructType* StructType, std::function<bool(const StructVariableType*)> Condition) const
		{
			int32 index = StructType->GetItems().FindIf(Condition);
			if (index == -1)
				return nullptr;

			return StructType->GetItems()[index];
		}

		const StructVariableType* ASTCompilerBase::GetVariableType(const StructType* StructType, std::function<bool(const StructVariableType*)> Condition) const
		{
			const StructVariableType* type = FindVariableType(StructType, Condition);
			if (type == nullptr)
				THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find the variable", StructType->GetName());

			return type;
		}

		String ASTCompilerBase::StatementToString(const Statement* Statement, const StageData& Data)
		{
			String stmStr;
			StageData data = { Data.FunctionType, Data.Stage, Data.Structs, Data.Variables, Data.Functions, stmStr, 0 };
			BuildStatement(Statement, data);

			return stmStr;
		}

		void ASTCompilerBase::AddCode(const String& Value, StageData& Data)
		{
#ifdef DEBUG_MODE
			if (Data.Shader.EndsWith('\n'))
				for (uint8 i = 0; i < m_BlockIndex + Data.IndentOffset; ++i)
					Data.Shader += '\t';
#endif

			Data.Shader += Value;
		}

		void ASTCompilerBase::AddNewLine(StageData& Data)
		{
#ifdef DEBUG_MODE
			Data.Shader += '\n';
#endif
		}

		String ASTCompilerBase::BuildSequentialVariable(const Statement* IntializerStatement, StageData& Data)
		{
			DataTypeStatement dataType = EvaluateDataType(IntializerStatement);
			return BuildSequentialVariable(&dataType, IntializerStatement, Data);
		}

		String ASTCompilerBase::BuildSequentialVariable(const DataTypeStatement* DataType, const Statement* IntializerStatement, StageData& Data)
		{
			const String varName = GetSequentialVariableName();

			BuildDataTypeStatement(DataType, Data);
			AddCode(' ', Data);
			AddCode(varName, Data);
			AddCode(" = ", Data);
			BuildStatement(IntializerStatement, Data);
			AddCode(';', Data);
			AddNewLine(Data);

			return varName;
		}

		String ASTCompilerBase::GetSequentialVariableName(void)
		{
			return '_' + StringUtility::ToString<char8>(++m_SequentialVariableNumber);
		}

		String ASTCompilerBase::GetSamplerVariableName(const String& TextureVariableName)
		{
			return TextureVariableName + "Sampler";
		}
	}
}