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
			m_OpenScopeCount(0),
			m_LastFunction(0)
		{
		}

		void ASTCompilerBase::Initialize(DeviceTypes DeviceType)
		{
			IntrinsicsBuilder::Initialize(DeviceType);
		}

		void ASTCompilerBase::Compile(AllocatorBase* Allocator, const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, OutputInfo& Output)
		{
			m_OpenScopeCount = 0;
			m_LastFunction = 0;
			m_Allocator = Allocator;

			m_Structs = Structs;
			m_Functions = Functions;

			auto ContainsType = [&Functions](FunctionType::Types Type)
			{
				return Functions.ContainsIf([Type](auto functionType) {return functionType->GetType() == Type; });
			};

			if (ContainsType(FunctionType::Types::VertexMain))
				BuildVertexShader(Structs, Variables, Functions, Output.VertexShader);

			if (ContainsType(FunctionType::Types::HullMain))
				BuildHullShader(Structs, Variables, Functions, Output.HullShader);

			if (ContainsType(FunctionType::Types::DomainMain))
				BuildDomainShader(Structs, Variables, Functions, Output.DomainShader);

			if (ContainsType(FunctionType::Types::GeometryMain))
				BuildGeometryShader(Structs, Variables, Functions, Output.GeometryShader);

			if (ContainsType(FunctionType::Types::FragmentMain))
				BuildFragmentShader(Structs, Variables, Functions, Output.FragmentShader);

			if (ContainsType(FunctionType::Types::ComputeMain))
				BuildComputeShader(Structs, Variables, Functions, Output.ComputeShader);

			m_Variables.Clear();
		}

		void ASTCompilerBase::BuildStageShader(Stages Stage, const StructList& Structs, const GlobalVariableList& Variables, const FunctionList& Functions, String& Shader)
		{
			m_LastFunction = nullptr;
			m_ReturnValueAlreadyBuilt = false;

			ResetPerStageValues(Stage);

			BuildHeader(Shader);

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
			m_Variables.Clear();
		}

		void ASTCompilerBase::BuildHeader(String& Shader)
		{
		}

		void ASTCompilerBase::BuildStructs(const StructList& Structs, Stages Stage, String& Shader)
		{
			for (auto structType : Structs)
				BuildStruct(structType, Stage, Shader);
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
				m_Variables[variable->GetName()] = variable->GetDataType();

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

				m_OpenScopeCount = 0;

				m_LastFunction = function;

				for (auto parameter : function->GetParameters())
					m_Variables[parameter->GetName()] = parameter->GetDataType();

				if (function->IsEntrypoint())
					BuildEntrypointFunction(function, Stage, Shader);
				else
					BuildFunction(function, Stage, Shader);

				while (m_OpenScopeCount > 0)
				{
					--m_OpenScopeCount;

					Shader += "}";

					ADD_NEW_LINE();
				}
			}
		}

		void ASTCompilerBase::BuildEntrypointFunction(FunctionType* Function, Stages Stage, String& Shader)
		{
			if (Stage == Stages::Hull)
			{
				const ConstantEntrypointAttributeType* constantEntryPoint = Function->GetAttribute<ConstantEntrypointAttributeType>();

				const StructType* structType = FindStructType(Function->GetReturnDataType()->GetUserDefined());
				if (structType == nullptr)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find %S struct type", Function->GetReturnDataType()->GetUserDefined());

				StructVariableType::Registers requiredRegisters[]{ StructVariableType::Registers::TessellationFactor, StructVariableType::Registers::InsideTessellationFactor };
				uint8 requiredRegisterCount = 2;

				for (uint8 i = 0; i < requiredRegisterCount; ++i)
				{
					StructVariableType::Registers reg = requiredRegisters[i];

					if (structType->GetItems().ContainsIf([reg](auto item) { return item->GetRegister() == reg; }))
						continue;

					THROW_PROGRAM_COMPILER_EXCEPTION(StringUtility::Format<char8>("A variable with %S is required inside %S", StructVariableType::GetRegisterName(reg), structType->GetName()), Function->GetName());
				}
			}

			BuildFunction(Function, Stage, Shader);
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
				if (!isFirst)
					Shader += ", ";
				isFirst = false;

				BuildParameter(parameter, Type, Stage, Shader);
			}
		}

		void ASTCompilerBase::BuildParameter(ParameterType* Parameter, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			BuildDataTypeStatement(Parameter->GetDataType(), Shader);
			Shader += " ";
			Shader += Parameter->GetName();
		}

		void ASTCompilerBase::BuildStatementHolder(StatementItemHolder* Holder, FunctionType::Types Type, Stages Stage, String& Shader)
		{
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
				if (EvaluateDataType(Statement->GetLeft()).GetType() == ProgramDataTypes::Matrix4)
				{
					IntrinsicsBuilder::BuildFunctionCallStatement("Multiply", { Statement->GetLeft(), Statement->GetRight() }, Type, Stage, Shader);

					return;
				}
			}
			else if (op == OperatorStatement::Operators::MultiplicationAssignment)
			{
				if (EvaluateDataType(Statement->GetLeft()).GetType() == ProgramDataTypes::Matrix4)
				{
					BuildStatement(Statement->GetLeft(), Type, Stage, Shader);

					Shader += '=';

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

			Shader += OperatorStatement::GetOperatorSymbol(op);

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

			if (m_Functions.ContainsIf([funcName](auto item) { return item->GetName() == funcName; }))
			{
				Shader += funcName;

				Shader += "(";

				BuildArguments(Statement->GetArguments(), Type, Stage, Shader);

				Shader += ")";

				return;
			}

			if (IntrinsicsBuilder::BuildFunctionCallStatement(Statement, Type, Stage, Shader))
				return;

			THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't find variable", funcName);
		}

		void ASTCompilerBase::BuildArguments(StatementItemHolder* Statements, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			BuildArguments(Statements->GetItems(), Type, Stage, Shader);
		}

		void ASTCompilerBase::BuildVariableStatement(VariableStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			m_Variables[Statement->GetName()] = Statement->GetDataType();

			BuildDataTypeStatement(Statement->GetDataType(), Shader);

			Shader += " ";
			Shader += Statement->GetName();

			if (Statement->GetInitialStatement() != nullptr)
			{
				Shader += "=";
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

			BuildStatement(Statement->GetCondition(), Type, Stage, Shader);

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
			Shader += "switch(";

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
				BuildStatement(Statement->GetCondition(), Type, Stage, Shader);

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

			BuildStatement(Statement->GetCondition(), Type, Stage, Shader);

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

			Shader += '=';

			BuildStatement(Statement, Type, Stage, Shader);

			Shader += ";";

			ADD_NEW_LINE();

			return elementCount;
		}

		void ASTCompilerBase::BuildDataTypeStatement(DataTypeStatement* Statement, String& Shader)
		{
			if (Statement->IsBuiltIn())
				BuildType(Statement->GetType(), Shader);
			else
				Shader += Statement->GetUserDefined();

			if (Statement->GetElementCount() != nullptr)
			{
				Shader += "[";
				BuildStatement(Statement->GetElementCount(), FunctionType::Types::None, Stages::Vertex, Shader);
				Shader += "]";
			}
		}

		bool ASTCompilerBase::ContainsReturnStatement(StatementItemHolder* Statement)
		{
			const auto& statements = Statement->GetItems();
			for (auto statement : statements)
			{
				if (IsAssignableFrom(statement, ReturnStatement))
					return true;

				if (!IsAssignableFrom(statement, StatementItemHolder))
					continue;

				if (ContainsReturnStatement(DynamicCast(StatementItemHolder*, statement)))
					return true;
			}

			return false;
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
				//					Void						Bool						Integer						UnsignedInteger						Float						Double						Float2						Double2						Float3						Double3						Float4						Double4						Matrix4
				/*Void*/			{ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown},
				/*Bool*/			{ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,			ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown },
				/*Integer*/			{ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Integer,	ProgramDataTypes::Integer,			ProgramDataTypes::Float,	ProgramDataTypes::Double,	ProgramDataTypes::Float2,	ProgramDataTypes::Double2,	ProgramDataTypes::Float3,	ProgramDataTypes::Double3,	ProgramDataTypes::Float4,	ProgramDataTypes::Double4,	ProgramDataTypes::Matrix4 },
				/*UnsignedInteger*/ {ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Integer,	ProgramDataTypes::UnsignedInteger,	ProgramDataTypes::Float,	ProgramDataTypes::Double,	ProgramDataTypes::Float2,	ProgramDataTypes::Double2,	ProgramDataTypes::Float3,	ProgramDataTypes::Double3,	ProgramDataTypes::Float4,	ProgramDataTypes::Double4,	ProgramDataTypes::Matrix4 },
				/*Float*/			{ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Float,	ProgramDataTypes::Float,			ProgramDataTypes::Float,	ProgramDataTypes::Double,	ProgramDataTypes::Float2,	ProgramDataTypes::Double2,	ProgramDataTypes::Float3,	ProgramDataTypes::Double3,	ProgramDataTypes::Float4,	ProgramDataTypes::Double4,	ProgramDataTypes::Matrix4 },
				/*Double*/			{ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Double,	ProgramDataTypes::Double,			ProgramDataTypes::Double,	ProgramDataTypes::Double,	ProgramDataTypes::Double2,	ProgramDataTypes::Double2,	ProgramDataTypes::Double3,	ProgramDataTypes::Double3,	ProgramDataTypes::Double4,	ProgramDataTypes::Double4,	ProgramDataTypes::Matrix4 },
				/*Float2*/			{ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Float2,	ProgramDataTypes::Float2,			ProgramDataTypes::Float2,	ProgramDataTypes::Double2,	ProgramDataTypes::Float2,	ProgramDataTypes::Double2,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown },
				/*Double2*/			{ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Double2,	ProgramDataTypes::Double2,			ProgramDataTypes::Double2,	ProgramDataTypes::Double2,	ProgramDataTypes::Double2,	ProgramDataTypes::Double2,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown },
				/*Float3*/			{ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Float3,	ProgramDataTypes::Float3,			ProgramDataTypes::Float3,	ProgramDataTypes::Double3,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Float3,	ProgramDataTypes::Double3,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown },
				/*Double3*/			{ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Double3,	ProgramDataTypes::Double3,			ProgramDataTypes::Double3,	ProgramDataTypes::Double3,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Double3,	ProgramDataTypes::Double3,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown },
				/*Float4*/			{ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Float4,	ProgramDataTypes::Float4,			ProgramDataTypes::Float4,	ProgramDataTypes::Double4,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Float4,	ProgramDataTypes::Double4,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown },
				/*Double4*/			{ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Double4,	ProgramDataTypes::Double4,			ProgramDataTypes::Double4,	ProgramDataTypes::Double4,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Double4,	ProgramDataTypes::Double4,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown },
				/*Matrix4*/			{ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Matrix4,	ProgramDataTypes::Matrix4,			ProgramDataTypes::Matrix4,	ProgramDataTypes::Matrix4,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Unknown,	ProgramDataTypes::Float4,	ProgramDataTypes::Double4,	ProgramDataTypes::Matrix4 },
			};

			if (IsAssignableFrom(CurrentStatement, OperatorStatement))
			{
				OperatorStatement* stm = ReinterpretCast(OperatorStatement*, CurrentStatement);

				DataTypeStatement leftType = EvaluateDataType(stm->GetLeft());
				DataTypeStatement rightType = EvaluateDataType(stm->GetRight());

				OperatorStatement::Operators op = stm->GetOperator();

				if (op == OperatorStatement::Operators::Multiplication ||
					op == OperatorStatement::Operators::Division)
					return MULTIPLY_RESULT[(uint8)leftType.GetType()][(uint8)rightType.GetType()];

				if (op == OperatorStatement::Operators::Remainder ||
					op == OperatorStatement::Operators::Addition ||
					op == OperatorStatement::Operators::Subtraction)
					return leftType;

				if (op == OperatorStatement::Operators::EqualCheck ||
					op == OperatorStatement::Operators::NotEqualCheck ||
					op == OperatorStatement::Operators::LessCheck ||
					op == OperatorStatement::Operators::LessEqualCheck ||
					op == OperatorStatement::Operators::GreaterCheck ||
					op == OperatorStatement::Operators::GreaterEqualCheck ||
					op == OperatorStatement::Operators::LogicalAnd ||
					op == OperatorStatement::Operators::LogicalOr)
					return ProgramDataTypes::Bool;
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

				uint32 hash = IntrinsicsBuilder::CalculateFunctionSignatureHash(stm->GetFunctionName(), stm->GetArguments()->GetItems());

				int32 index = m_Functions.FindIf([hash](auto item)
					{
						ProgramDataTypes parameterTypes[16];
						uint8 parameterTypeCount = 0;
						const auto& parameters = item->GetParameters();
						for (auto& parameter : parameters)
							parameterTypes[parameterTypeCount++] = parameter->GetDataType()->GetType();

						return (hash == IntrinsicsBuilder::CalculateFunctionSignatureHash(item->GetName(), parameterTypes, parameterTypeCount));
					});

				if (index != -1)
					return *m_Functions[index]->GetReturnDataType();

				ProgramDataTypes dataType = IntrinsicsBuilder::EvaluateFunctionReturnValue(stm);
				if (dataType == ProgramDataTypes::Unknown)
					THROW_PROGRAM_COMPILER_EXCEPTION("Couldn't evaluate result of the statement", stm->ToString());

				return dataType;
			}
			else if (IsAssignableFrom(CurrentStatement, VariableAccessStatement))
			{
				VariableAccessStatement* stm = ReinterpretCast(VariableAccessStatement*, CurrentStatement);

				const String& variableName = stm->GetName();

				if (m_Variables.Contains(variableName))
					return *m_Variables[stm->GetName()];

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

					if (leftType == ProgramDataTypes::Matrix4)
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

		const StructType* ASTCompilerBase::FindStructType(const String& Name) const
		{
			int32 index = m_Structs.FindIf([&Name](auto structType) { return structType->GetName() == Name; });
			if (index == -1)
				return nullptr;

			return m_Structs[index];
		}

		const VariableType* ASTCompilerBase::FindVariableType(const StructType* StructType, const String& Name) const
		{
			int32 index = StructType->GetItems().FindIf([&Name](auto variableType) { return variableType->GetName() == Name; });
			if (index == -1)
				return nullptr;

			return StructType->GetItems()[index];
		}

		cstr ASTCompilerBase::GetStageResultArrayVariableName(void)
		{
			static cstr name = "__result_value__";

			return name;
		}

		ProgramDataTypes ASTCompilerBase::EvaluateProgramDataType(Statement* Statement) const
		{
			return EvaluateDataType(Statement).GetType();
		}

		void ASTCompilerBase::BuildArguments(const Vector<Statement*>& Statements, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			bool isFirst = true;
			for (auto argument : Statements)
			{
				if (!isFirst)
					Shader += ",";
				isFirst = false;

				BuildStatement(argument, Type, Stage, Shader);
			}
		}

#undef ADD_NEW_LINE
	}
}