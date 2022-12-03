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
			m_LastFunction(nullptr)
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
				BuildStageShader({ FunctionType::Types::None, Stages::Vertex, Structs, Variables, Functions, Output.VertexShader });

			if (containsEntrypoint(FunctionType::Types::HullMain))
				BuildStageShader({ FunctionType::Types::None, Stages::Hull, Structs, Variables, Functions, Output.HullShader });

			if (containsEntrypoint(FunctionType::Types::DomainMain))
				BuildStageShader({ FunctionType::Types::None, Stages::Domain, Structs, Variables, Functions, Output.DomainShader });

			if (containsEntrypoint(FunctionType::Types::GeometryMain))
				BuildStageShader({ FunctionType::Types::None, Stages::Geometry, Structs, Variables, Functions, Output.GeometryShader });

			if (containsEntrypoint(FunctionType::Types::FragmentMain))
				BuildStageShader({ FunctionType::Types::None, Stages::Fragment, Structs, Variables, Functions, Output.FragmentShader });

			if (containsEntrypoint(FunctionType::Types::ComputeMain))
				BuildStageShader({ FunctionType::Types::None, Stages::Compute, Structs, Variables, Functions, Output.ComputeShader });

			m_Structs.Clear();
			m_Functions.Clear();
			m_BlockVariables.Clear();
		}

		void ASTCompilerBase::BuildStageShader(const StageData& Data)
		{
			ResetPerStageValues(Data);

			BuildStructs(Data);

			BuildGlobalVariables(Data);

			BuildFunctions(Data);
		}

		void ASTCompilerBase::ResetPerStageValues(const StageData& Data)
		{
			m_Structs.Clear();
			m_Functions.Clear();
			m_BlockVariables.Clear();
			m_BlockIndex = -1;
			m_LastFunction = nullptr;
			m_ReturnValueAlreadyBuilt = false;

			IncreaseBlockIndex();
		}

		void ASTCompilerBase::BuildStructs(const StageData& Data)
		{
			for (auto structType : Data.Structs)
			{
				m_Structs.Add(structType);

				BuildStruct(structType, Data);
			}
		}

		void ASTCompilerBase::BuildGlobalVariables(const StageData& Data)
		{
			for (auto variable : Data.Variables)
			{
				PushVariable(variable);

				BuildGlobalVariable(variable, Data);
			}
		}

		void ASTCompilerBase::BuildFunctions(const StageData& Data)
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

				for (auto parameter : function->GetParameters())
					PushVariable(parameter);

				StageData data = { funcType, Data.Stage, Data.Structs, Data.Variables, Data.Functions, Data.Shader };

				if (function->IsEntrypoint())
					ValidateEntrypointFunction(function, data);

				BuildFunction(function, data);
			}
		}

		void ASTCompilerBase::ValidateEntrypointFunction(FunctionType* Function, const StageData& Data)
		{
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

			auto checkRequiredOutputRegisters = [&](const FunctionType* Function, const StructVariableType::Registers* RequiredRegisters, uint8 RequiredRegisterCount)
			{
				checkRequiredRegisters(Function->GetReturnDataType()->GetUserDefined(), RequiredRegisters, RequiredRegisterCount);
			};

			auto checkRequiredInputRegisters = [&](const FunctionType* Function, const StructVariableType::Registers* RequiredRegisters, uint8 RequiredRegisterCount)
			{
				for (auto& param : Function->GetParameters())
				{
					auto& structName = param->GetDataType()->GetUserDefined();
					if (structName == String::Empty)
						continue;

					checkRequiredRegisters(structName, RequiredRegisters, RequiredRegisterCount);
				}
			};

			const StructVariableType::Registers RequiredTessFactorsRegisters[]{ StructVariableType::Registers::TessellationFactor, StructVariableType::Registers::InsideTessellationFactor };

			if (Function->GetParameters().GetSize() > 1)
				THROW_PROGRAM_COMPILER_EXCEPTION("Entrypoints cannot have more than one parameter", Function->GetName());

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
				const ConstantEntrypointAttributeType* constantEntryPoint = Function->GetAttribute<ConstantEntrypointAttributeType>();
				if (constantEntryPoint == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find ConstantEntrypoint attribute", Function->GetName());

				const FunctionType* constantEntryPointFunc = FindFunctionType(constantEntryPoint->GetEntrypoint());
				if (constantEntryPointFunc == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find ConstantEntrypoint function", Function->GetName());

				checkRequiredOutputRegisters(constantEntryPointFunc, RequiredTessFactorsRegisters, _countof(RequiredTessFactorsRegisters));
			}
			else if (Data.Stage == Stages::Domain)
			{
				if (Function->GetAttribute<DomainAttributeType>() == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find Domain attribute", Function->GetName());

				checkRequiredInputRegisters(Function, RequiredTessFactorsRegisters, _countof(RequiredTessFactorsRegisters));
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

		void ASTCompilerBase::BuildAttributes(const AttributeList& Attributes, const StageData& Data)
		{
			for (auto attribute : Attributes)
			{
				BuildAttribute(attribute, Data);

				AddNewLine(Data);
			}
		}

		void ASTCompilerBase::BuildAttribute(BaseAttributeType* Attribute, const StageData& Data)
		{
			if (IsAssignableFrom(Attribute, DomainAttributeType))
			{
				DomainAttributeType* stm = ReinterpretCast(DomainAttributeType*, Attribute);

				BuildDomainAttributeType(stm, Data);
			}
			else if (IsAssignableFrom(Attribute, PartitioningAttributeType))
			{
				PartitioningAttributeType* stm = ReinterpretCast(PartitioningAttributeType*, Attribute);

				BuildPartitioningAttributeType(stm, Data);
			}
			else if (IsAssignableFrom(Attribute, TopologyAttributeType))
			{
				TopologyAttributeType* stm = ReinterpretCast(TopologyAttributeType*, Attribute);

				BuildTopologyAttributeType(stm, Data);
			}
			else if (IsAssignableFrom(Attribute, ControlPointsAttributeType))
			{
				ControlPointsAttributeType* stm = ReinterpretCast(ControlPointsAttributeType*, Attribute);

				BuildControlPointsAttributeType(stm, Data);
			}
			else if (IsAssignableFrom(Attribute, ConstantEntrypointAttributeType))
			{
				ConstantEntrypointAttributeType* stm = ReinterpretCast(ConstantEntrypointAttributeType*, Attribute);

				BuildConstantEntrypointAttributeType(stm, Data);
			}
			else if (IsAssignableFrom(Attribute, MaxVertexCountAttributeType))
			{
				MaxVertexCountAttributeType* stm = ReinterpretCast(MaxVertexCountAttributeType*, Attribute);

				BuildMaxVertexCountAttributeType(stm, Data);
			}
			else if (IsAssignableFrom(Attribute, PrimitiveTypeAttributeType))
			{
				PrimitiveTypeAttributeType* stm = ReinterpretCast(PrimitiveTypeAttributeType*, Attribute);

				BuildPrimitiveTypeAttributeType(stm, Data);
			}
			else if (IsAssignableFrom(Attribute, OutputStreamTypeAttributeType))
			{
				OutputStreamTypeAttributeType* stm = ReinterpretCast(OutputStreamTypeAttributeType*, Attribute);

				BuildOutputStreamTypeAttributeType(stm, Data);
			}
			else if (IsAssignableFrom(Attribute, ThreadCountAttributeType))
			{
				ThreadCountAttributeType* stm = ReinterpretCast(ThreadCountAttributeType*, Attribute);

				BuildThreadCountAttributeType(stm, Data);
			}
		}

		void ASTCompilerBase::BuildParameters(const ParameterList& Parameters, const StageData& Data)
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

		void ASTCompilerBase::BuildParameter(ParameterType* Parameter, const StageData& Data)
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

		void ASTCompilerBase::BuildStatementHolder(StatementItemHolder* Holder, const StageData& Data)
		{
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

			DecreaseBlockIndex();
		}

		void ASTCompilerBase::BuildStatement(Statement* Statement, const StageData& Data)
		{
			if (IsAssignableFrom(Statement, OperatorStatement))
			{
				OperatorStatement* stm = ReinterpretCast(OperatorStatement*, Statement);

				BuildOperatorStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, UnaryOperatorStatement))
			{
				UnaryOperatorStatement* stm = ReinterpretCast(UnaryOperatorStatement*, Statement);

				BuildUnaryOperatorStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, ConstantStatement))
			{
				ConstantStatement* stm = ReinterpretCast(ConstantStatement*, Statement);

				BuildConstantStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, FunctionCallStatement))
			{
				FunctionCallStatement* stm = ReinterpretCast(FunctionCallStatement*, Statement);

				BuildFunctionCallStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, VariableStatement))
			{
				VariableStatement* stm = ReinterpretCast(VariableStatement*, Statement);

				BuildVariableStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, VariableAccessStatement))
			{
				VariableAccessStatement* stm = ReinterpretCast(VariableAccessStatement*, Statement);

				BuildVariableAccessStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, ArrayElementAccessStatement))
			{
				ArrayElementAccessStatement* stm = ReinterpretCast(ArrayElementAccessStatement*, Statement);

				BuildArrayElementAccessStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, MemberAccessStatement))
			{
				MemberAccessStatement* stm = ReinterpretCast(MemberAccessStatement*, Statement);

				BuildMemberAccessStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, IfStatement))
			{
				IfStatement* stm = ReinterpretCast(IfStatement*, Statement);

				BuildIfStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, ElseStatement))
			{
				ElseStatement* stm = ReinterpretCast(ElseStatement*, Statement);

				BuildElseStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, SwitchStatement))
			{
				SwitchStatement* stm = ReinterpretCast(SwitchStatement*, Statement);

				BuildSwitchStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, CaseStatement))
			{
				CaseStatement* stm = ReinterpretCast(CaseStatement*, Statement);

				BuildCaseStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, DefaultStatement))
			{
				DefaultStatement* stm = ReinterpretCast(DefaultStatement*, Statement);

				BuildDefaultStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, ForStatement))
			{
				ForStatement* stm = ReinterpretCast(ForStatement*, Statement);

				BuildForStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, DoStatement))
			{
				DoStatement* stm = ReinterpretCast(DoStatement*, Statement);

				BuildDoStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, WhileStatement))
			{
				WhileStatement* stm = ReinterpretCast(WhileStatement*, Statement);

				BuildWhileStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, BreakStatement))
			{
				BreakStatement* stm = ReinterpretCast(BreakStatement*, Statement);

				BuildBreakStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, ReturnStatement))
			{
				ReturnStatement* stm = ReinterpretCast(ReturnStatement*, Statement);

				BuildReturnStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, ArrayStatement))
			{
				ArrayStatement* stm = ReinterpretCast(ArrayStatement*, Statement);

				BuildArrayStatement(stm, Data);
			}
			else if (IsAssignableFrom(Statement, DiscardStatement))
			{
				DiscardStatement* stm = ReinterpretCast(DiscardStatement*, Statement);

				BuildDiscardStatement(stm, Data);
			}
			else
				THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
		}

		void ASTCompilerBase::BuildOperatorStatement(OperatorStatement* Statement, const StageData& Data)
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

		void ASTCompilerBase::BuildUnaryOperatorStatement(UnaryOperatorStatement* Statement, const StageData& Data)
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

		void ASTCompilerBase::BuildConstantStatement(ConstantStatement* Statement, const StageData& Data)
		{
			if (Statement->GetType() == ProgramDataTypes::Bool)
				AddCode(StringUtility::ToString<char8>(Statement->GetBool()), Data);
			else if (Statement->GetFloat32() == 0 || Statement->GetFloat32() / (int32)Statement->GetFloat32() == 1)
				AddCode(StringUtility::ToString<char8>((int32)Statement->GetFloat32()), Data);
			else
				AddCode(StringUtility::ToString<char8>(Statement->GetFloat32()), Data);
		}

		void ASTCompilerBase::BuildFunctionCallStatement(FunctionCallStatement* Statement, const StageData& Data)
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

		void ASTCompilerBase::BuildArguments(const Vector<Statement*>& Statements, const StageData& Data)
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

		void ASTCompilerBase::BuildArguments(StatementItemHolder* Statements, const StageData& Data)
		{
			BuildArguments(Statements->GetItems(), Data);
		}

		void ASTCompilerBase::BuildVariableStatement(VariableStatement* Statement, const StageData& Data)
		{
			if (FindVariableType(Statement->GetName(), true))
				THROW_PROGRAM_COMPILER_EXCEPTION("Variable redifinition", Statement->GetName());

			PushVariable(Statement);

			BuildDataTypeStatement(Statement->GetDataType(), Data);

			AddCode(' ', Data);
			AddCode(Statement->GetName(), Data);

			if (Statement->GetInitialStatement() != nullptr)
			{
				AddCode(" = ", Data);
				BuildStatement(Statement->GetInitialStatement(), Data);
			}
		}

		void ASTCompilerBase::BuildArrayElementAccessStatement(ArrayElementAccessStatement* Statement, const StageData& Data)
		{
			BuildStatement(Statement->GetArrayStatement(), Data);

			AddCode('[', Data);

			BuildStatement(Statement->GetElementStatement(), Data);

			AddCode(']', Data);
		}

		void ASTCompilerBase::BuildMemberAccessStatement(MemberAccessStatement* Statement, const StageData& Data)
		{
			BuildStatement(Statement->GetLeft(), Data);

			AddCode('.', Data);

			BuildStatement(Statement->GetRight(), Data);
		}

		void ASTCompilerBase::BuildIfStatement(IfStatement* Statement, const StageData& Data)
		{
			AddCode("if (", Data);

			BuildExplicitCast(Statement->GetCondition(), ProgramDataTypes::Bool, Data);

			AddCode(')', Data);

			AddNewLine(Data);

			AddCode('{', Data);

			AddNewLine(Data);

			BuildStatementHolder(Statement, Data);

			AddCode('}', Data);

			AddNewLine(Data);

			if (Statement->GetElse() != nullptr)
				BuildStatement(Statement->GetElse(), Data);
		}

		void ASTCompilerBase::BuildElseStatement(ElseStatement* Statement, const StageData& Data)
		{
			AddCode("else", Data);

			AddNewLine(Data);

			AddCode('{', Data);

			AddNewLine(Data);

			BuildStatementHolder(Statement, Data);

			AddCode('}', Data);

			AddNewLine(Data);
		}

		void ASTCompilerBase::BuildSwitchStatement(SwitchStatement* Statement, const StageData& Data)
		{
			AddCode("switch (", Data);

			BuildStatement(Statement->GetSelector(), Data);

			AddCode(')', Data);
			AddNewLine(Data);
			AddCode('{', Data);
			AddNewLine(Data);

			BuildStatementHolder(Statement, Data);

			AddCode('}', Data);
			AddNewLine(Data);
		}

		void ASTCompilerBase::BuildCaseStatement(CaseStatement* Statement, const StageData& Data)
		{
			AddCode("case ", Data);

			BuildStatement(Statement->GetCondition(), Data);

			AddCode(':', Data);
			AddNewLine(Data);

			if (Statement->GetItems().GetSize() != 0)
			{
				AddCode('{', Data);
				AddNewLine(Data);

				BuildStatementHolder(Statement, Data);

				AddCode('}', Data);
				AddNewLine(Data);
			}
		}

		void ASTCompilerBase::BuildDefaultStatement(DefaultStatement* Statement, const StageData& Data)
		{
			AddCode("default:", Data);
			AddNewLine(Data);

			if (Statement->GetItems().GetSize() != 0)
			{
				AddCode('{', Data);
				AddNewLine(Data);

				BuildStatementHolder(Statement, Data);

				AddCode('}', Data);
				AddNewLine(Data);
			}
		}

		void ASTCompilerBase::BuildForStatement(ForStatement* Statement, const StageData& Data)
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

			BuildStatementHolder(Statement, Data);

			AddCode('}', Data);
			AddNewLine(Data);
		}

		void ASTCompilerBase::BuildDoStatement(DoStatement* Statement, const StageData& Data)
		{
			AddCode("do", Data);

			AddCode('{', Data);
			AddNewLine(Data);

			BuildStatementHolder(Statement, Data);

			AddCode('}', Data);

			BuildStatement(Statement->GetWhile(), Data);
		}

		void ASTCompilerBase::BuildWhileStatement(WhileStatement* Statement, const StageData& Data)
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

			BuildStatementHolder(Statement, Data);

			AddCode('}', Data);
			AddNewLine(Data);
		}

		void ASTCompilerBase::BuildContinueStatement(ContinueStatement* Statement, const StageData& Data)
		{
			AddCode("continue;", Data);
			AddNewLine(Data);
		}

		void ASTCompilerBase::BuildBreakStatement(BreakStatement* Statement, const StageData& Data)
		{
			AddCode("break;", Data);
			AddNewLine(Data);
		}

		void ASTCompilerBase::BuildDiscardStatement(DiscardStatement* Statement, const StageData& Data)
		{
			if (Data.Stage != Stages::Fragment)
				THROW_PROGRAM_COMPILER_EXCEPTION("Not a valid statement in this stage", Statement->ToString());

			AddCode("discard;", Data);
			AddNewLine(Data);
		}

		uint8 ASTCompilerBase::BuildReturnValue(Statement* Statement, const StageData& Data)
		{
			uint8 elementCount = EvaluateDataTypeElementCount(m_LastFunction->GetReturnDataType());

			if (m_ReturnValueAlreadyBuilt)
				return elementCount;
			m_ReturnValueAlreadyBuilt = true;

			bool isArray = IsAssignableFrom(Statement, ArrayStatement);

			if (elementCount > 1 && !isArray)
			{
				return 0;
			}

			BuildType(m_LastFunction->GetReturnDataType()->GetType(), Data);
			AddCode(' ', Data);
			AddCode(GetStageResultArrayVariableName(), Data);

			if (isArray)
			{
				AddCode('[', Data);
				AddCode(StringUtility::ToString<char8>(elementCount), Data);
				AddCode(']', Data);
			}

			AddCode(" = ", Data);

			BuildStatement(Statement, Data);

			AddCode(';', Data);

			AddNewLine(Data);

			return elementCount;
		}

		void ASTCompilerBase::BuildDataTypeStatement(const DataTypeStatement* Statement, const StageData& Data)
		{
			if (Statement->IsBuiltIn())
				BuildType(Statement->GetType(), Data);
			else
				AddCode(Statement->GetUserDefined(), Data);

			if (Statement->GetElementCount() != nullptr)
			{
				AddCode('[', Data);
				BuildStatement(Statement->GetElementCount(), Data);
				AddCode(']', Data);
			}
		}

		void ASTCompilerBase::BuildExplicitCast(Statement* Statement, const DataTypeStatement* DataType, const StageData& Data)
		{
			bool needsCasting = !CompareDataTypes(EvaluateDataType(Statement), *DataType);

			if (needsCasting)
			{
				AddCode('(', Data);

				BuildDataTypeStatement(DataType, Data);

				AddCode(")(", Data);
			}

			BuildStatement(Statement, Data);

			if (needsCasting)
				AddCode(')', Data);
		}

		void ASTCompilerBase::BuildExplicitCast(Statement* Statement, ProgramDataTypes DataType, const StageData& Data)
		{
			DataTypeStatement dataType = DataType;

			BuildExplicitCast(Statement, &dataType, Data);
		}

		uint8 ASTCompilerBase::EvaluateDataTypeElementCount(DataTypeStatement* Statement)
		{
			if (Statement == nullptr)
				return 0;

			uint8 elementCount = 1;

			if (Statement->GetElementCount() != nullptr)
			{
				String elementCountString;
				BuildStatement(Statement->GetElementCount(), { FunctionType::Types::None, Stages::Vertex, {}, {}, {} , elementCountString });

				elementCount = StringUtility::ToInt8(elementCountString, 1);
			}

			return elementCount;
		}

		DataTypeStatement ASTCompilerBase::EvaluateDataType(Statement* CurrentStatement, Statement* TopStatement) const
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

			if (IsAssignableFrom(CurrentStatement, OperatorStatement))
			{
				OperatorStatement* stm = ReinterpretCast(OperatorStatement*, CurrentStatement);

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
			else if (IsAssignableFrom(CurrentStatement, UnaryOperatorStatement))
			{
				UnaryOperatorStatement* stm = ReinterpretCast(UnaryOperatorStatement*, CurrentStatement);

				return EvaluateDataType(stm->GetStatement());
			}
			else if (IsAssignableFrom(CurrentStatement, ConstantStatement))
			{
				ConstantStatement* stm = ReinterpretCast(ConstantStatement*, CurrentStatement);

				return stm->GetType();
			}
			else if (IsAssignableFrom(CurrentStatement, FunctionCallStatement))
			{
				FunctionCallStatement* stm = ReinterpretCast(FunctionCallStatement*, CurrentStatement);

				const FunctionType* functionType = FindMatchingFunction(stm->GetFunctionName(), stm->GetArguments());
				if (functionType != nullptr)
					return *(functionType->GetReturnDataType());

				ProgramDataTypes dataType = IntrinsicsBuilder::EvaluateFunctionReturnValue(stm);
				if (dataType == ProgramDataTypes::Unknown)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't evaluate result of the statement", stm->ToString());

				return dataType;
			}
			else if (IsAssignableFrom(CurrentStatement, VariableAccessStatement))
			{
				VariableAccessStatement* stm = ReinterpretCast(VariableAccessStatement*, CurrentStatement);

				const String& variableName = stm->GetName();

				const VariableType* variableType = FindVariableType(stm->GetName());
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

				THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't evaluate result of the statement", stm->ToString());
			}
			else if (IsAssignableFrom(CurrentStatement, ArrayElementAccessStatement))
			{
				ArrayElementAccessStatement* stm = ReinterpretCast(ArrayElementAccessStatement*, CurrentStatement);

				return EvaluateDataType(stm->GetArrayStatement());
			}
			else if (IsAssignableFrom(CurrentStatement, MemberAccessStatement))
			{
				MemberAccessStatement* stm = ReinterpretCast(MemberAccessStatement*, CurrentStatement);

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
			else if (IsAssignableFrom(CurrentStatement, ArrayStatement))
			{
				ArrayStatement* stm = ReinterpretCast(ArrayStatement*, CurrentStatement);

				if (stm->GetItems().GetSize() == 0)
					return ProgramDataTypes::Unknown;

				return EvaluateDataType(stm->GetItems()[0]);
			}

			return ProgramDataTypes::Unknown;
		}

		ProgramDataTypes ASTCompilerBase::EvaluateProgramDataType(Statement* Statement) const
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

		void ASTCompilerBase::PushVariable(VariableType* Variable)
		{
			m_BlockVariables[m_BlockIndex].Add(Variable);
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

						return true;
					}
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

		void ASTCompilerBase::AddCode(const String& Value, const StageData& Data)
		{
#ifdef DEBUG_MODE
			if (Data.Shader.EndsWith('\n'))
				for (uint8 i = 0; i < m_BlockIndex; ++i)
					Data.Shader += '\t';
#endif

			Data.Shader += Value;
		}

		void ASTCompilerBase::AddNewLine(const StageData& Data)
		{
#ifdef DEBUG_MODE
			Data.Shader += '\n';
#endif
		}

		cstr ASTCompilerBase::GetStageResultArrayVariableName(void)
		{
			static cstr name = "__result_value__";

			return name;
		}
	}
}