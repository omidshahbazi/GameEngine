// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ASTCompiler\ASTCompilerBase.h>
#include <ProgramCompilerCommon\ProgramCompilerException.h>

namespace Engine
{
	using namespace ProgramCompilerCommon;

	namespace ASTCompiler
	{
#ifdef DEBUG_MODE
#define ADD_NEW_LINE() Shader += "\n"
#else
#define ADD_NEW_LINE()
#endif

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
				BuildVertexShader(Structs, Variables, Functions, Output.VertexShader);

			if (containsEntrypoint(FunctionType::Types::HullMain))
				BuildHullShader(Structs, Variables, Functions, Output.HullShader);

			if (containsEntrypoint(FunctionType::Types::DomainMain))
				BuildDomainShader(Structs, Variables, Functions, Output.DomainShader);

			if (containsEntrypoint(FunctionType::Types::GeometryMain))
				BuildGeometryShader(Structs, Variables, Functions, Output.GeometryShader);

			if (containsEntrypoint(FunctionType::Types::FragmentMain))
				BuildFragmentShader(Structs, Variables, Functions, Output.FragmentShader);

			if (containsEntrypoint(FunctionType::Types::ComputeMain))
				BuildComputeShader(Structs, Variables, Functions, Output.ComputeShader);

			m_Structs.Clear();
			m_Functions.Clear();
			m_BlockVariables.Clear();
		}

		void ASTCompilerBase::BuildStageShader(Stages Stage, const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, String& Shader)
		{
			ResetPerStageValues(Stage);

			BuildStructs(Structs, Stage, Shader);

			BuildGlobalVariables(Variables, Stage, Shader);

			BuildFunctions(Functions, Stage, Shader);
		}

		void ASTCompilerBase::BuildVertexShader(const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, String& Shader)
		{
			BuildStageShader(Stages::Vertex, Structs, Variables, Functions, Shader);
		}

		void ASTCompilerBase::BuildHullShader(const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, String& Shader)
		{
			BuildStageShader(Stages::Hull, Structs, Variables, Functions, Shader);
		}

		void ASTCompilerBase::BuildDomainShader(const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, String& Shader)
		{
			BuildStageShader(Stages::Domain, Structs, Variables, Functions, Shader);
		}

		void ASTCompilerBase::BuildGeometryShader(const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, String& Shader)
		{
			BuildStageShader(Stages::Geometry, Structs, Variables, Functions, Shader);
		}

		void ASTCompilerBase::BuildFragmentShader(const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, String& Shader)
		{
			BuildStageShader(Stages::Fragment, Structs, Variables, Functions, Shader);
		}

		void ASTCompilerBase::BuildComputeShader(const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, String& Shader)
		{
			BuildStageShader(Stages::Compute, Structs, Variables, Functions, Shader);
		}

		void ASTCompilerBase::ResetPerStageValues(Stages Stage)
		{
			m_Structs.Clear();
			m_Functions.Clear();
			m_BlockVariables.Clear();
			m_BlockIndex = -1;
			m_LastFunction = nullptr;
			m_ReturnValueAlreadyBuilt = false;

			IncreaseBlockIndex();
		}

		void ASTCompilerBase::BuildStructs(const StructList& Structs, Stages Stage, String& Shader)
		{
			for (auto structType : Structs)
			{
				m_Structs.Add(structType);

				BuildStruct(structType, Stage, Shader);
			}
		}

		void ASTCompilerBase::BuildStructVariables(const StructVariableList& Variables, Stages Stage, String& Shader)
		{
			for (auto variable : Variables)
				BuildStructVariable(variable, Stage, Shader);
		}

		void ASTCompilerBase::BuildGlobalVariables(const GlobalVariableList& Variables, Stages Stage, String& Shader)
		{
			for (auto variable : Variables)
			{
				PushVariable(variable);

				BuildGlobalVariable(variable, Stage, Shader);
			}
		}

		void ASTCompilerBase::BuildFunctions(const FunctionList& Functions, Stages Stage, String& Shader)
		{
			for (auto function : Functions)
			{
				if (function->IsEntrypoint())
				{
					FunctionType::Types funcType = function->GetType();

					if (funcType == FunctionType::Types::VertexMain && Stage != Stages::Vertex)
						continue;

					if (funcType == FunctionType::Types::HullMain && Stage != Stages::Hull)
						continue;

					if (funcType == FunctionType::Types::DomainMain && Stage != Stages::Domain)
						continue;

					if (funcType == FunctionType::Types::GeometryMain && Stage != Stages::Geometry)
						continue;

					if (funcType == FunctionType::Types::FragmentMain && Stage != Stages::Fragment)
						continue;

					if (funcType == FunctionType::Types::ComputeMain && Stage != Stages::Compute)
						continue;
				}

				m_Functions.Add(function);
				m_LastFunction = function;

				for (auto parameter : function->GetParameters())
					PushVariable(parameter);

				if (function->IsEntrypoint())
					ValidateEntrypointFunction(function, Stage, Shader);

				BuildFunction(function, Stage, Shader);
			}
		}

		void ASTCompilerBase::ValidateEntrypointFunction(FunctionType* Function, Stages Stage, String& Shader)
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

			if (Stage == Stages::Hull)
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
			else if (Stage == Stages::Domain)
			{
				if (Function->GetAttribute<DomainAttributeType>() == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find Domain attribute", Function->GetName());

				checkRequiredInputRegisters(Function, RequiredTessFactorsRegisters, _countof(RequiredTessFactorsRegisters));
			}
			else if (Stage == Stages::Geometry)
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
			else if (Stage == Stages::Compute)
			{
				if (Function->GetAttribute<ThreadCountAttributeType>() == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find ThreadCount attribute", Function->GetName());

				if (Function->GetReturnDataType()->GetType() != ProgramDataTypes::Void)
					THROW_PROGRAM_COMPILER_EXCEPTION("Compute program must not return any value", Function->GetName());
			}
		}

		void ASTCompilerBase::BuildAttributes(const AttributeList& Attributes, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			for (auto attribute : Attributes)
			{
				BuildAttribute(attribute, Type, Stage, Shader);

				ADD_NEW_LINE();
			}
		}

		void ASTCompilerBase::BuildAttribute(BaseAttributeType* Attribute, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			if (IsAssignableFrom(Attribute, DomainAttributeType))
			{
				DomainAttributeType* stm = ReinterpretCast(DomainAttributeType*, Attribute);

				BuildDomainAttributeType(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Attribute, PartitioningAttributeType))
			{
				PartitioningAttributeType* stm = ReinterpretCast(PartitioningAttributeType*, Attribute);

				BuildPartitioningAttributeType(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Attribute, TopologyAttributeType))
			{
				TopologyAttributeType* stm = ReinterpretCast(TopologyAttributeType*, Attribute);

				BuildTopologyAttributeType(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Attribute, ControlPointsAttributeType))
			{
				ControlPointsAttributeType* stm = ReinterpretCast(ControlPointsAttributeType*, Attribute);

				BuildControlPointsAttributeType(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Attribute, ConstantEntrypointAttributeType))
			{
				ConstantEntrypointAttributeType* stm = ReinterpretCast(ConstantEntrypointAttributeType*, Attribute);

				BuildConstantEntrypointAttributeType(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Attribute, MaxVertexCountAttributeType))
			{
				MaxVertexCountAttributeType* stm = ReinterpretCast(MaxVertexCountAttributeType*, Attribute);

				BuildMaxVertexCountAttributeType(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Attribute, PrimitiveTypeAttributeType))
			{
				PrimitiveTypeAttributeType* stm = ReinterpretCast(PrimitiveTypeAttributeType*, Attribute);

				BuildPrimitiveTypeAttributeType(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Attribute, OutputStreamTypeAttributeType))
			{
				OutputStreamTypeAttributeType* stm = ReinterpretCast(OutputStreamTypeAttributeType*, Attribute);

				BuildOutputStreamTypeAttributeType(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Attribute, ThreadCountAttributeType))
			{
				ThreadCountAttributeType* stm = ReinterpretCast(ThreadCountAttributeType*, Attribute);

				BuildThreadCountAttributeType(stm, Type, Stage, Shader);
			}
		}

		void ASTCompilerBase::BuildParameters(const ParameterList& Parameters, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			bool isFirst = true;
			for (auto parameter : Parameters)
			{
				PushVariable(parameter);

				if (!isFirst)
					Shader += ", ";
				isFirst = false;

				BuildParameter(parameter, Type, Stage, Shader);
			}
		}

		void ASTCompilerBase::BuildParameter(ParameterType* Parameter, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			BuildDataTypeStatement(Parameter->GetDataType(), Type, Stage, Shader);
			Shader += " ";
			Shader += Parameter->GetName();

			if (Parameter->GetDataType()->GetPostElementCount() != nullptr)
			{
				Shader += "[";
				BuildStatement(Parameter->GetDataType()->GetPostElementCount(), FunctionType::Types::None, Stage, Shader);
				Shader += "]";
			}
		}

		void ASTCompilerBase::BuildStatementHolder(StatementItemHolder* Holder, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			IncreaseBlockIndex();

			for (auto statement : Holder->GetItems())
			{
				BuildStatement(statement, Type, Stage, Shader);

				if (IsAssignableFrom(statement, VariableStatement) ||
					IsAssignableFrom(statement, OperatorStatement) ||
					IsAssignableFrom(statement, UnaryOperatorStatement))
				{
					Shader += ";";

					ADD_NEW_LINE();
				}
			}

			DecreaseBlockIndex();
		}

		void ASTCompilerBase::BuildStatement(Statement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			if (IsAssignableFrom(Statement, OperatorStatement))
			{
				OperatorStatement* stm = ReinterpretCast(OperatorStatement*, Statement);

				BuildOperatorStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, UnaryOperatorStatement))
			{
				UnaryOperatorStatement* stm = ReinterpretCast(UnaryOperatorStatement*, Statement);

				BuildUnaryOperatorStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, ConstantStatement))
			{
				ConstantStatement* stm = ReinterpretCast(ConstantStatement*, Statement);

				BuildConstantStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, FunctionCallStatement))
			{
				FunctionCallStatement* stm = ReinterpretCast(FunctionCallStatement*, Statement);

				BuildFunctionCallStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, VariableStatement))
			{
				VariableStatement* stm = ReinterpretCast(VariableStatement*, Statement);

				BuildVariableStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, VariableAccessStatement))
			{
				VariableAccessStatement* stm = ReinterpretCast(VariableAccessStatement*, Statement);

				BuildVariableAccessStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, ArrayElementAccessStatement))
			{
				ArrayElementAccessStatement* stm = ReinterpretCast(ArrayElementAccessStatement*, Statement);

				BuildArrayElementAccessStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, MemberAccessStatement))
			{
				MemberAccessStatement* stm = ReinterpretCast(MemberAccessStatement*, Statement);

				BuildMemberAccessStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, IfStatement))
			{
				IfStatement* stm = ReinterpretCast(IfStatement*, Statement);

				BuildIfStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, ElseStatement))
			{
				ElseStatement* stm = ReinterpretCast(ElseStatement*, Statement);

				BuildElseStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, SwitchStatement))
			{
				SwitchStatement* stm = ReinterpretCast(SwitchStatement*, Statement);

				BuildSwitchStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, CaseStatement))
			{
				CaseStatement* stm = ReinterpretCast(CaseStatement*, Statement);

				BuildCaseStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, DefaultStatement))
			{
				DefaultStatement* stm = ReinterpretCast(DefaultStatement*, Statement);

				BuildDefaultStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, ForStatement))
			{
				ForStatement* stm = ReinterpretCast(ForStatement*, Statement);

				BuildForStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, DoStatement))
			{
				DoStatement* stm = ReinterpretCast(DoStatement*, Statement);

				BuildDoStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, WhileStatement))
			{
				WhileStatement* stm = ReinterpretCast(WhileStatement*, Statement);

				BuildWhileStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, BreakStatement))
			{
				BreakStatement* stm = ReinterpretCast(BreakStatement*, Statement);

				BuildBreakStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, ReturnStatement))
			{
				ReturnStatement* stm = ReinterpretCast(ReturnStatement*, Statement);

				BuildReturnStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, ArrayStatement))
			{
				ArrayStatement* stm = ReinterpretCast(ArrayStatement*, Statement);

				BuildArrayStatement(stm, Type, Stage, Shader);
			}
			else if (IsAssignableFrom(Statement, DiscardStatement))
			{
				DiscardStatement* stm = ReinterpretCast(DiscardStatement*, Statement);

				BuildDiscardStatement(stm, Type, Stage, Shader);
			}
			else
				THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
		}

		void ASTCompilerBase::BuildOperatorStatement(OperatorStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			OperatorStatement::Operators op = Statement->GetOperator();

			if (op == OperatorStatement::Operators::Multiplication)
			{
				ProgramDataTypes dataType = EvaluateDataType(Statement->GetLeft()).GetType();

				if (dataType == ProgramDataTypes::Matrix4F ||
					dataType == ProgramDataTypes::Matrix4D)
				{
					IntrinsicsBuilder::BuildFunctionCallStatement("Multiply", { Statement->GetLeft(), Statement->GetRight() }, Type, Stage, Shader);

					return;
				}
			}
			else if (op == OperatorStatement::Operators::MultiplicationAssignment)
			{
				ProgramDataTypes dataType = EvaluateDataType(Statement->GetLeft()).GetType();

				if (dataType == ProgramDataTypes::Matrix4F ||
					dataType == ProgramDataTypes::Matrix4D)
				{
					BuildStatement(Statement->GetLeft(), Type, Stage, Shader);

					Shader += " = ";

					IntrinsicsBuilder::BuildFunctionCallStatement("Multiply", { Statement->GetLeft(), Statement->GetRight() }, Type, Stage, Shader);

					return;
				}
			}
			else if (op == OperatorStatement::Operators::Remainder)
			{
				IntrinsicsBuilder::BuildFunctionCallStatement("Reminder", { Statement->GetLeft(), Statement->GetRight() }, Type, Stage, Shader);

				return;
			}

			bool isAssignment =
				op == OperatorStatement::Operators::Assignment ||
				op == OperatorStatement::Operators::AdditionAssignment ||
				op == OperatorStatement::Operators::DivisionAssignment ||
				op == OperatorStatement::Operators::MultiplicationAssignment ||
				op == OperatorStatement::Operators::SubtractionAssignment;

			if (!isAssignment)
				Shader += "(";

			BuildStatement(Statement->GetLeft(), Type, Stage, Shader);

			Shader += ' ';

			Shader += OperatorStatement::GetOperatorSymbol(op);

			Shader += ' ';

			if (isAssignment)
			{
				DataTypeStatement leftDataType = EvaluateDataType(Statement->GetLeft());
				BuildExplicitCast(Statement->GetRight(), &leftDataType, Type, Stage, Shader);
			}
			else
				BuildStatement(Statement->GetRight(), Type, Stage, Shader);

			if (!isAssignment)
				Shader += ")";
		}

		void ASTCompilerBase::BuildUnaryOperatorStatement(UnaryOperatorStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			Shader += "(";

			switch (Statement->GetOperator())
			{
			case UnaryOperatorStatement::Operators::Exlamation:
			case UnaryOperatorStatement::Operators::Minus:
			case UnaryOperatorStatement::Operators::PrefixIncrement:
			case UnaryOperatorStatement::Operators::PrefixDecrement:
				Shader += UnaryOperatorStatement::GetOperatorSymbol(Statement->GetOperator());
				break;
			}

			BuildStatement(Statement->GetStatement(), Type, Stage, Shader);

			switch (Statement->GetOperator())
			{
			case UnaryOperatorStatement::Operators::PostfixIncrement:
			case UnaryOperatorStatement::Operators::PostfixDecrement:
				Shader += UnaryOperatorStatement::GetOperatorSymbol(Statement->GetOperator());
				break;
			}

			Shader += ")";
		}

		void ASTCompilerBase::BuildConstantStatement(ConstantStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			if (Statement->GetType() == ProgramDataTypes::Bool)
				Shader += StringUtility::ToString<char8>(Statement->GetBool());
			else if (Statement->GetFloat32() == 0 || Statement->GetFloat32() / (int32)Statement->GetFloat32() == 1)
				Shader += StringUtility::ToString<char8>((int32)Statement->GetFloat32());
			else
				Shader += StringUtility::ToString<char8>(Statement->GetFloat32());
		}

		void ASTCompilerBase::BuildFunctionCallStatement(FunctionCallStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			auto& funcName = Statement->GetFunctionName();

			const FunctionType* functionType = FindMatchingFunction(funcName, Statement->GetArguments());
			if (functionType != nullptr)
			{
				Shader += funcName;

				Shader += "(";

				uint8 i = 0;
				for (auto argument : Statement->GetArguments()->GetItems())
				{
					if (i != 0)
						Shader += ", ";

					BuildExplicitCast(argument, functionType->GetParameters()[i++]->GetDataType(), Type, Stage, Shader);
				}

				Shader += ")";

				return;
			}

			if (IntrinsicsBuilder::BuildFunctionCallStatement(Statement, Type, Stage, Shader))
				return;

			THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find variable or function", funcName);
		}

		void ASTCompilerBase::BuildArguments(const Vector<Statement*>& Statements, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			bool isFirst = true;
			for (auto argument : Statements)
			{
				if (!isFirst)
					Shader += ", ";
				isFirst = false;

				BuildStatement(argument, Type, Stage, Shader);
			}
		}

		void ASTCompilerBase::BuildArguments(StatementItemHolder* Statements, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			BuildArguments(Statements->GetItems(), Type, Stage, Shader);
		}

		void ASTCompilerBase::BuildVariableStatement(VariableStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			if (FindVariableType(Statement->GetName(), true))
				THROW_PROGRAM_COMPILER_EXCEPTION("Variable redifinition", Statement->GetName());

			PushVariable(Statement);

			BuildDataTypeStatement(Statement->GetDataType(), Type, Stage, Shader);

			Shader += " ";
			Shader += Statement->GetName();

			if (Statement->GetInitialStatement() != nullptr)
			{
				Shader += " = ";
				BuildStatement(Statement->GetInitialStatement(), Type, Stage, Shader);
			}
		}

		void ASTCompilerBase::BuildArrayElementAccessStatement(ArrayElementAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			BuildStatement(Statement->GetArrayStatement(), Type, Stage, Shader);

			Shader += "[";

			BuildStatement(Statement->GetElementStatement(), Type, Stage, Shader);

			Shader += "]";
		}

		void ASTCompilerBase::BuildMemberAccessStatement(MemberAccessStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			BuildStatement(Statement->GetLeft(), Type, Stage, Shader);

			Shader += ".";

			BuildStatement(Statement->GetRight(), Type, Stage, Shader);
		}

		void ASTCompilerBase::BuildIfStatement(IfStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			Shader += "if (";

			BuildExplicitCast(Statement->GetCondition(), ProgramDataTypes::Bool, Type, Stage, Shader);

			Shader += ")";

			ADD_NEW_LINE();

			Shader += "{";

			ADD_NEW_LINE();

			BuildStatementHolder(Statement, Type, Stage, Shader);

			Shader += "}";

			ADD_NEW_LINE();

			if (Statement->GetElse() != nullptr)
				BuildStatement(Statement->GetElse(), Type, Stage, Shader);
		}

		void ASTCompilerBase::BuildElseStatement(ElseStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			Shader += "else";

			ADD_NEW_LINE();

			Shader += "{";

			ADD_NEW_LINE();

			BuildStatementHolder(Statement, Type, Stage, Shader);

			Shader += "}";

			ADD_NEW_LINE();
		}

		void ASTCompilerBase::BuildSwitchStatement(SwitchStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			Shader += "switch (";

			BuildStatement(Statement->GetSelector(), Type, Stage, Shader);

			Shader += ")";
			ADD_NEW_LINE();
			Shader += "{";
			ADD_NEW_LINE();

			BuildStatementHolder(Statement, Type, Stage, Shader);

			Shader += "}";
			ADD_NEW_LINE();
		}

		void ASTCompilerBase::BuildCaseStatement(CaseStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			Shader += "case ";

			BuildStatement(Statement->GetCondition(), Type, Stage, Shader);

			Shader += ":";
			ADD_NEW_LINE();

			if (Statement->GetItems().GetSize() != 0)
			{
				Shader += "{";
				ADD_NEW_LINE();

				BuildStatementHolder(Statement, Type, Stage, Shader);

				Shader += "}";
				ADD_NEW_LINE();
			}
		}

		void ASTCompilerBase::BuildDefaultStatement(DefaultStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			Shader += "default:";
			ADD_NEW_LINE();

			if (Statement->GetItems().GetSize() != 0)
			{
				Shader += "{";
				ADD_NEW_LINE();

				BuildStatementHolder(Statement, Type, Stage, Shader);

				Shader += "}";
				ADD_NEW_LINE();
			}
		}

		void ASTCompilerBase::BuildForStatement(ForStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			Shader += "for (";

			if (Statement->GetInitializer() != nullptr)
				BuildStatement(Statement->GetInitializer(), Type, Stage, Shader);

			Shader += ";";

			if (Statement->GetCondition() != nullptr)
				BuildExplicitCast(Statement->GetCondition(), ProgramDataTypes::Bool, Type, Stage, Shader);

			Shader += ";";

			if (Statement->GetStep() != nullptr)
				BuildStatement(Statement->GetStep(), Type, Stage, Shader);

			Shader += ")";

			if (Statement->GetItems().GetSize() == 0)
			{
				Shader += ";";
				ADD_NEW_LINE();

				return;
			}

			ADD_NEW_LINE();

			Shader += "{";
			ADD_NEW_LINE();

			BuildStatementHolder(Statement, Type, Stage, Shader);

			Shader += "}";
			ADD_NEW_LINE();
		}

		void ASTCompilerBase::BuildDoStatement(DoStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			Shader += "do";

			Shader += "{";
			ADD_NEW_LINE();

			BuildStatementHolder(Statement, Type, Stage, Shader);

			Shader += "}";

			BuildStatement(Statement->GetWhile(), Type, Stage, Shader);
		}

		void ASTCompilerBase::BuildWhileStatement(WhileStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			Shader += "while (";

			BuildExplicitCast(Statement->GetCondition(), ProgramDataTypes::Bool, Type, Stage, Shader);

			Shader += ")";

			if (Statement->GetItems().GetSize() == 0)
			{
				Shader += ";";
				ADD_NEW_LINE();

				return;
			}

			ADD_NEW_LINE();

			Shader += "{";
			ADD_NEW_LINE();

			BuildStatementHolder(Statement, Type, Stage, Shader);

			Shader += "}";
			ADD_NEW_LINE();
		}

		void ASTCompilerBase::BuildContinueStatement(ContinueStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			Shader += "continue;";
			ADD_NEW_LINE();
		}

		void ASTCompilerBase::BuildBreakStatement(BreakStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			Shader += "break;";
			ADD_NEW_LINE();
		}

		void ASTCompilerBase::BuildDiscardStatement(DiscardStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			if (Stage != Stages::Fragment)
				THROW_PROGRAM_COMPILER_EXCEPTION("Not a valid statement in this stage", Statement->ToString());

			Shader += "discard;";
			ADD_NEW_LINE();
		}

		uint8 ASTCompilerBase::BuildReturnValue(Statement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
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

			BuildType(m_LastFunction->GetReturnDataType()->GetType(), Shader);
			Shader += " ";
			Shader += GetStageResultArrayVariableName();

			if (isArray)
			{
				Shader += '[';
				Shader += StringUtility::ToString<char8>(elementCount);
				Shader += ']';
			}

			Shader += " = ";

			BuildStatement(Statement, Type, Stage, Shader);

			Shader += ";";

			ADD_NEW_LINE();

			return elementCount;
		}

		void ASTCompilerBase::BuildDataTypeStatement(const DataTypeStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			if (Statement->IsBuiltIn())
				BuildType(Statement->GetType(), Shader);
			else
				Shader += Statement->GetUserDefined();

			if (Statement->GetElementCount() != nullptr)
			{
				Shader += "[";
				BuildStatement(Statement->GetElementCount(), Type, Stage, Shader);
				Shader += "]";
			}
		}

		void ASTCompilerBase::BuildExplicitCast(Statement* Statement, const DataTypeStatement* DataType, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			bool needsCasting = !CompareDataTypes(EvaluateDataType(Statement), *DataType);

			if (needsCasting)
			{
				Shader += '(';

				BuildDataTypeStatement(DataType, Type, Stage, Shader);

				Shader += ")(";
			}

			BuildStatement(Statement, Type, Stage, Shader);

			if (needsCasting)
				Shader += ')';
		}

		void ASTCompilerBase::BuildExplicitCast(Statement* Statement, ProgramDataTypes DataType, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			DataTypeStatement dataType = DataType;

			BuildExplicitCast(Statement, &dataType, Type, Stage, Shader);
		}

		uint8 ASTCompilerBase::EvaluateDataTypeElementCount(DataTypeStatement* Statement)
		{
			if (Statement == nullptr)
				return 0;

			uint8 elementCount = 1;

			if (Statement->GetElementCount() != nullptr)
			{
				String elementCountString;
				BuildStatement(Statement->GetElementCount(), FunctionType::Types::None, Stages::Vertex, elementCountString);

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

		cstr ASTCompilerBase::GetStageResultArrayVariableName(void)
		{
			static cstr name = "__result_value__";

			return name;
		}

#undef ADD_NEW_LINE
	}
}