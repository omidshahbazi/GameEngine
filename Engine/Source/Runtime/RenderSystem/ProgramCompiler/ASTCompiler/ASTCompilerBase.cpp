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

		void ASTCompilerBase::Compile(AllocatorBase* Allocator, const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, OutputInfo& Output)
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

			if (ContainsType(FunctionType::Types::TessellationMain))
				BuildTessellationShader(Structs, Variables, Functions, Output.TessellationShader);

			if (ContainsType(FunctionType::Types::GeometryMain))
				BuildGeometryShader(Structs, Variables, Functions, Output.GeometryShader);

			if (ContainsType(FunctionType::Types::FragmentMain))
				BuildFragmentShader(Structs, Variables, Functions, Output.FragmentShader);

			if (ContainsType(FunctionType::Types::ComputeMain))
				BuildComputeShader(Structs, Variables, Functions, Output.ComputeShader);
		}

		void ASTCompilerBase::BuildStageShader(Stages Stage, const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
		{
			m_LastFunction = nullptr;

			ResetPerStageValues(Stage);

			BuildHeader(Shader);

			BuildStructs(Structs, Stage, Shader);

			BuildVariables(Variables, Stage, Shader);

			BuildFunctions(Functions, Stage, Shader);
		}

		void ASTCompilerBase::BuildVertexShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
		{
			BuildStageShader(Stages::Vertex, Structs, Variables, Functions, Shader);
		}

		void ASTCompilerBase::BuildTessellationShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
		{
			BuildStageShader(Stages::Tessellation, Structs, Variables, Functions, Shader);
		}

		void ASTCompilerBase::BuildGeometryShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
		{
			BuildStageShader(Stages::Geometry, Structs, Variables, Functions, Shader);
		}

		void ASTCompilerBase::BuildFragmentShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
		{
			BuildStageShader(Stages::Fragment, Structs, Variables, Functions, Shader);
		}

		void ASTCompilerBase::BuildComputeShader(const StructList& Structs, const VariableList& Variables, const FunctionList& Functions, String& Shader)
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

		void ASTCompilerBase::BuildVariables(const VariableList& Variables, Stages Stage, String& Shader)
		{
			for (auto variable : Variables)
			{
				m_Variables[variable->GetName()] = variable->GetDataType();

				BuildVariable(variable, Stage, Shader);
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

					if (funcType == FunctionType::Types::FragmentMain && Stage != Stages::Fragment)
						continue;
				}

				m_OpenScopeCount = 0;

				m_LastFunction = function;

				for (auto parameter : function->GetParameters())
					m_Variables[parameter->GetName()] = parameter->GetDataType();

				BuildFunction(function, Stage, Shader);

				while (m_OpenScopeCount > 0)
				{
					--m_OpenScopeCount;

					Shader += "}";

					ADD_NEW_LINE();
				}
			}
		}

		void ASTCompilerBase::BuildStatementHolder(StatementItemHolder* Holder, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			for (auto statement : Holder->GetItems())
				BuildStatement(statement, Type, Stage, Shader);
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
			else if (IsAssignableFrom(Statement, SemicolonStatement))
			{
				SemicolonStatement* stm = ReinterpretCast(SemicolonStatement*, Statement);

				BuildSemicolonStatement(stm, Type, Stage, Shader);
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

			//ContinueStatement
			//DoStatement
			//ForStatement
			//SwitchStatement
			//WhileStatement
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

			Shader += UnaryOperatorStatement::GetOperatorSymbol(Statement->GetOperator());

			BuildStatement(Statement->GetStatement(), Type, Stage, Shader);

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

			if (Statement->GetInitialStatement() == nullptr)
				Shader += ';';
			else
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

		void ASTCompilerBase::BuildSemicolonStatement(SemicolonStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			Shader += ";";

			ADD_NEW_LINE();
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

		void ASTCompilerBase::BuildDiscardStatement(DiscardStatement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			Shader += "discard";
		}

		uint8 ASTCompilerBase::BuildReturnValue(Statement* Statement, FunctionType::Types Type, Stages Stage, String& Shader)
		{
			bool isArray = IsAssignableFrom(Statement, ArrayStatement);
			uint8 elementCount = EvaluateDataTypeElementCount(m_LastFunction->GetReturnDataType());

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

		bool ASTCompilerBase::ContainsVariable(const String& Name)
		{
			return m_Variables.Contains(Name);
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
				{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
				{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
				{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float,ProgramDataTypes::Double,ProgramDataTypes::Float2,ProgramDataTypes::Double2,ProgramDataTypes::Float3,ProgramDataTypes::Double3,ProgramDataTypes::Float4,ProgramDataTypes::Double4,ProgramDataTypes::Matrix4 },
				{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double,ProgramDataTypes::Double,ProgramDataTypes::Double2,ProgramDataTypes::Double2,ProgramDataTypes::Double3,ProgramDataTypes::Double3,ProgramDataTypes::Double4,ProgramDataTypes::Double4,ProgramDataTypes::Matrix4 },
				{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float2,ProgramDataTypes::Double2,ProgramDataTypes::Float2,ProgramDataTypes::Double2,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
				{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double2,ProgramDataTypes::Double2,ProgramDataTypes::Double2,ProgramDataTypes::Double2,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
				{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float3,ProgramDataTypes::Double3,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float3,ProgramDataTypes::Double3,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
				{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double3,ProgramDataTypes::Double3,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double3,ProgramDataTypes::Double3,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
				{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float4,ProgramDataTypes::Double4,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float4,ProgramDataTypes::Double4,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
				{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double4,ProgramDataTypes::Double4,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Double4,ProgramDataTypes::Double4,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown },
				{ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Matrix4,ProgramDataTypes::Matrix4,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Unknown,ProgramDataTypes::Float4,ProgramDataTypes::Double4,ProgramDataTypes::Matrix4 },
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

		void ASTCompilerBase::InjectParameterIntoTopFunction(ProgramDataTypes Type, const String& Name, const String& Register)
		{
		}

#undef ADD_NEW_LINE
	}
}