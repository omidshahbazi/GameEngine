// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ProgramParser\Parser.h>
#include <ProgramParser\AbstractSyntaxTree\StructType.h>
#include <ProgramParser\AbstractSyntaxTree\StructVariableType.h>
#include <ProgramParser\AbstractSyntaxTree\GlobalVariableType.h>
#include <ProgramParser\AbstractSyntaxTree\FunctionType.h>
#include <ProgramParser\AbstractSyntaxTree\ParameterType.h>
#include <ProgramParser\AbstractSyntaxTree\DomainAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\PartitioningAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\TopologyAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\ControlPointsAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\ConstantEntrypointAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\MaxVertexCountAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\PrimitiveTypeAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\OutputStreamTypeAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\ThreadCountAttributeType.h>
#include <ProgramParser\AbstractSyntaxTree\DataTypeStatement.h>
#include <ProgramParser\AbstractSyntaxTree\IfStatement.h>
#include <ProgramParser\AbstractSyntaxTree\ElseStatement.h>
#include <ProgramParser\AbstractSyntaxTree\SwitchStatement.h>
#include <ProgramParser\AbstractSyntaxTree\DefaultStatement.h>
#include <ProgramParser\AbstractSyntaxTree\CaseStatement.h>
#include <ProgramParser\AbstractSyntaxTree\ForStatement.h>
#include <ProgramParser\AbstractSyntaxTree\DoStatement.h>
#include <ProgramParser\AbstractSyntaxTree\WhileStatement.h>
#include <ProgramParser\AbstractSyntaxTree\ContinueStatement.h>
#include <ProgramParser\AbstractSyntaxTree\BreakStatement.h>
#include <ProgramParser\AbstractSyntaxTree\ReturnStatement.h>
#include <ProgramParser\AbstractSyntaxTree\DiscardStatement.h>
#include <ProgramParser\AbstractSyntaxTree\OperatorStatement.h>
#include <ProgramParser\AbstractSyntaxTree\UnaryOperatorStatement.h>
#include <ProgramParser\AbstractSyntaxTree\VariableStatement.h>
#include <ProgramParser\AbstractSyntaxTree\FunctionCallStatement.h>
#include <ProgramParser\AbstractSyntaxTree\ConstantStatement.h>
#include <ProgramParser\AbstractSyntaxTree\VariableAccessStatement.h>
#include <ProgramParser\AbstractSyntaxTree\ArrayElementAccessStatement.h>
#include <ProgramParser\AbstractSyntaxTree\MemberAccessStatement.h>
#include <ProgramParser\AbstractSyntaxTree\ArrayStatement.h>
#include <ProgramParser\ProgramParserException.h>
#include <ProgramCompilerCommon\Common.h>
#include <Containers\StringUtility.h>
#include <Common\BitwiseUtils.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Lexer;
	using namespace ProgramCompilerCommon;

	namespace ProgramParser
	{
		using namespace AbstractSyntaxTree;

		cstr IF = STRINGIZE(if);
		cstr ELSE = STRINGIZE(else);
		cstr SWITCH = STRINGIZE(switch);
		cstr CASE = STRINGIZE(case);
		cstr DEFAULT = STRINGIZE(default);
		cstr FOR = STRINGIZE(for);
		cstr DO = STRINGIZE(do);
		cstr WHILE = STRINGIZE(while);
		cstr CONTINUE = STRINGIZE(continue);
		cstr BREAK = STRINGIZE(break);
		cstr RETURN = STRINGIZE(return);
		cstr STRUCT = STRINGIZE(struct);
		cstr DISCARD = "discard";
		cstr INCREMENT("++");
		cstr DECREMENT("--");

		OperatorStatement::Operators GetOperator(const String& Symbol)
		{
			static bool initialized = false;
			static Map<String, OperatorStatement::Operators> operators;

			if (!initialized)
			{
				initialized = true;

				operators["*"] = OperatorStatement::Operators::Multiplication;
				operators["/"] = OperatorStatement::Operators::Division;
				operators["%"] = OperatorStatement::Operators::Remainder;
				operators["+"] = OperatorStatement::Operators::Addition;
				operators["-"] = OperatorStatement::Operators::Subtraction;
				operators["="] = OperatorStatement::Operators::Assignment;
				operators["+="] = OperatorStatement::Operators::AdditionAssignment;
				operators["-="] = OperatorStatement::Operators::SubtractionAssignment;
				operators["*="] = OperatorStatement::Operators::MultiplicationAssignment;
				operators["/="] = OperatorStatement::Operators::DivisionAssignment;
				operators["=="] = OperatorStatement::Operators::EqualCheck;
				operators["!="] = OperatorStatement::Operators::NotEqualCheck;
				operators["<"] = OperatorStatement::Operators::LessCheck;
				operators["<="] = OperatorStatement::Operators::LessEqualCheck;
				operators[">"] = OperatorStatement::Operators::GreaterCheck;
				operators[">="] = OperatorStatement::Operators::GreaterEqualCheck;
				operators["&&"] = OperatorStatement::Operators::LogicalAnd;
				operators["||"] = OperatorStatement::Operators::LogicalOr;
			}

			if (operators.Contains(Symbol))
				return operators[Symbol];

			return OperatorStatement::Operators::Unknown;
		}

		UnaryOperatorStatement::Operators GetUnaryOperator(const String& Symbol, bool IsPostfix)
		{
			static bool initialized = false;
			static Map<String, UnaryOperatorStatement::Operators> operators;

			if (!initialized)
			{
				initialized = true;

				operators["!"] = UnaryOperatorStatement::Operators::Exlamation;
				operators["-"] = UnaryOperatorStatement::Operators::Minus;
				operators["++"] = UnaryOperatorStatement::Operators::PrefixIncrement;
				operators["--"] = UnaryOperatorStatement::Operators::PrefixDecrement;
			}

			UnaryOperatorStatement::Operators op = UnaryOperatorStatement::Operators::Unknown;

			if (operators.Contains(Symbol))
				op = operators[Symbol];

			if (IsPostfix)
				switch (op)
				{
				case UnaryOperatorStatement::Operators::PrefixIncrement:
					op = UnaryOperatorStatement::Operators::PostfixIncrement;
					break;
				case UnaryOperatorStatement::Operators::PrefixDecrement:
					op = UnaryOperatorStatement::Operators::PostfixDecrement;
					break;
				}

			return op;
		}

		int8 GetOperatorPrecedence(OperatorStatement::Operators Operator)
		{
			switch (Operator)
			{
			case OperatorStatement::Operators::Assignment:
			case OperatorStatement::Operators::AdditionAssignment:
			case OperatorStatement::Operators::SubtractionAssignment:
			case OperatorStatement::Operators::MultiplicationAssignment:
			case OperatorStatement::Operators::DivisionAssignment:
				return 0;

			case OperatorStatement::Operators::LogicalAnd:
			case OperatorStatement::Operators::LogicalOr:
				return 1;

			case OperatorStatement::Operators::EqualCheck:
			case OperatorStatement::Operators::NotEqualCheck:
			case OperatorStatement::Operators::LessCheck:
			case OperatorStatement::Operators::LessEqualCheck:;
			case OperatorStatement::Operators::GreaterCheck:
			case OperatorStatement::Operators::GreaterEqualCheck:
				return 2;

			case OperatorStatement::Operators::Division:
				return 3;

			case OperatorStatement::Operators::Multiplication:
				return 4;

			case OperatorStatement::Operators::Remainder:
				return 5;

			case OperatorStatement::Operators::Addition:
			case OperatorStatement::Operators::Subtraction:
				return 6;
			}

			return -1;
		}

		Parser::Parser(AllocatorBase* Allocator, const String& Text) :
			Tokenizer(Text),
			m_Allocator(Allocator),
			m_Parameters(nullptr),
			m_StructsStack(m_Allocator),
			m_AttributesList(m_Allocator),
			m_VariablesStack(m_Allocator)
		{
			m_KeywordParsers[IF] = std::make_shared<KeywordParseFunction>([&](const Token& DeclarationToken) { return ParseIfStatement(DeclarationToken); });
			m_KeywordParsers[ELSE] = std::make_shared<KeywordParseFunction>([&](const Token& DeclarationToken) { return ParseElseStatement(DeclarationToken); });
			m_KeywordParsers[SWITCH] = std::make_shared<KeywordParseFunction>([&](const Token& DeclarationToken) { return ParseSwitchStatement(DeclarationToken); });
			m_KeywordParsers[DEFAULT] = std::make_shared<KeywordParseFunction>([&](const Token& DeclarationToken) { return ParseDefaultStatement(DeclarationToken); });
			m_KeywordParsers[CASE] = std::make_shared<KeywordParseFunction>([&](const Token& DeclarationToken) { return ParseCaseStatement(DeclarationToken); });
			m_KeywordParsers[FOR] = std::make_shared<KeywordParseFunction>([&](const Token& DeclarationToken) { return ParseForStatement(DeclarationToken); });
			m_KeywordParsers[DO] = std::make_shared<KeywordParseFunction>([&](const Token& DeclarationToken) { return ParseDoStatement(DeclarationToken); });
			m_KeywordParsers[WHILE] = std::make_shared<KeywordParseFunction>([&](const Token& DeclarationToken) { return ParseWhileStatement(DeclarationToken); });
			m_KeywordParsers[CONTINUE] = std::make_shared<KeywordParseFunction>([&](const Token& DeclarationToken) { return ParseContinueStatement(DeclarationToken); });
			m_KeywordParsers[BREAK] = std::make_shared<KeywordParseFunction>([&](const Token& DeclarationToken) { return ParseBreakStatement(DeclarationToken); });
			m_KeywordParsers[RETURN] = std::make_shared<KeywordParseFunction>([&](const Token& DeclarationToken) { return ParseReturnStatement(DeclarationToken); });
			m_KeywordParsers[DISCARD] = std::make_shared<KeywordParseFunction>([&](const Token& DeclarationToken) { return ParseDiscardStatement(DeclarationToken); });

			m_AttributeParsers["Domain"] = std::make_shared<AttributeParseFunction>([&](const Token& DeclarationToken) { return ParseDomainAttributeType(DeclarationToken); });
			m_AttributeParsers["Partitioning"] = std::make_shared<AttributeParseFunction>([&](const Token& DeclarationToken) { return ParsePartitioningAttributeType(DeclarationToken); });
			m_AttributeParsers["Topology"] = std::make_shared<AttributeParseFunction>([&](const Token& DeclarationToken) { return ParseTopologyAttributeType(DeclarationToken); });
			m_AttributeParsers["ControlPoints"] = std::make_shared<AttributeParseFunction>([&](const Token& DeclarationToken) { return ParseControlPointsAttributeType(DeclarationToken); });
			m_AttributeParsers["ConstantEntrypoint"] = std::make_shared<AttributeParseFunction>([&](const Token& DeclarationToken) { return ParseConstantEntrypointAttributeType(DeclarationToken); });
			m_AttributeParsers["MaxVertexCount"] = std::make_shared<AttributeParseFunction>([&](const Token& DeclarationToken) { return ParseMaxVertexCountAttributeType(DeclarationToken); });
			m_AttributeParsers["PrimitiveType"] = std::make_shared<AttributeParseFunction>([&](const Token& DeclarationToken) { return ParsePrimitiveTypeAttributeType(DeclarationToken); });
			m_AttributeParsers["OutputStreamType"] = std::make_shared<AttributeParseFunction>([&](const Token& DeclarationToken) { return ParseOutputStreamTypeAttributeType(DeclarationToken); });
			m_AttributeParsers["ThreadCount"] = std::make_shared<AttributeParseFunction>([&](const Token& DeclarationToken) { return ParseThreadCountAttributeType(DeclarationToken); });
		}

		void Parser::Parse(Parameters& Parameters)
		{
			Tokenizer::Reset();

			m_Parameters = &Parameters;

			PushAVariableList();

			while (true)
			{
				Token token;
				if (!GetToken(token))
					break;

				if (ParseStruct(token))
					continue;

				if (ParseGlobalVariable(token))
					continue;

				if (ParseAttribute(token))
					continue;

				if (ParseFunction(token))
					continue;
			}

			PopAVariableList();

			CoreDebugAssert(Categories::RenderSystem, m_StructsStack.GetSize() == 0, "m_StructsStack didn't get evacuated from stack");
			CoreDebugAssert(Categories::RenderSystem, m_VariablesStack.GetSize() == 0, "m_VariablesStack didn't get evacuated from stack");

			m_Parameters = nullptr;
		}

		bool Parser::ParseStruct(const Token& DeclarationToken)
		{
			if (DeclarationToken.GetType() != Token::Types::Identifier)
				return false;

			if (!DeclarationToken.Matches(STRUCT, Token::SearchCases::CaseSensitive))
				return false;

			StructType* structType = Allocate<StructType>(m_Allocator);

			Token nameToken;
			RequireIdentifierToken(nameToken, "struct");
			structType->SetName(nameToken.GetName());

			RequireSymbol(OPEN_BRACKET, "struct");

			m_StructsStack.Push(structType);

			while (true)
			{
				if (MatchSymbol(CLOSE_BRACKET))
					break;

				StructVariableType* variableType = Allocate<StructVariableType>(structType, m_Allocator);
				structType->AddItem(variableType);

				Token variableToken;
				RequireToken(variableToken, "struct");

				ParseStructVariable(variableToken, variableType);

				auto variables = structType->GetItems();

				if (variables.ContainsIf([](auto item) { return item->GetRegister() != StructVariableType::Registers::None; }))
				{
					if (variables.ContainsIf([](auto item) { return item->GetRegister() == StructVariableType::Registers::None; }))
					{
						THROW_PROGRAM_PARSER_EXCEPTION("Combination of input layout variables and user layout variables in a same struct is forbidden", variableToken);
					}
				}
			}

			m_StructsStack.Pop();

			RequireSymbol(SEMICOLON, "struct");

			m_Parameters->Structs.Add(structType);

			return true;
		}

		bool Parser::ParseStructVariable(const Token& DeclarationToken, StructVariableType* VariableType)
		{
			if (DeclarationToken.GetType() != Token::Types::Identifier)
				return false;

			DataTypeStatement* dataType = ParseDataType(DeclarationToken);
			ValidateDataType(dataType);
			VariableType->SetDataType(dataType);

			Token nameToken;
			if (!MatchIdentifierToken(nameToken))
			{
				UngetToken(nameToken);

				return false;
			}

			if (MatchSymbol(OPEN_BRACE))
			{
				UngetToken(nameToken);

				return false;
			}

			VariableType->SetName(nameToken.GetName());

			ParsePostArrayDataType(dataType);

			if (MatchSymbol(COLON))
			{
				Token registerToken;
				RequireIdentifierToken(registerToken, "Parse struct variable");

				String identifier = registerToken.GetName();

				uint8 indexStartIndex = identifier.GetLength() - 1;
				for (; indexStartIndex >= 0; --indexStartIndex)
					if (!CharacterUtility::IsDigit(identifier[indexStartIndex]))
					{
						++indexStartIndex;
						break;
					}

				String registerName = identifier.SubString(0, indexStartIndex);
				StructVariableType::Registers reg = StructVariableType::Registers::None;

				if (registerName == "POSITION")
					reg = StructVariableType::Registers::Position;
				else if (registerName == "NORMAL")
					reg = StructVariableType::Registers::Normal;
				else if (registerName == "COLOR")
					reg = StructVariableType::Registers::Color;
				else if (registerName == "UV")
					reg = StructVariableType::Registers::UV;
				else if (registerName == "PRIMITIVE_ID")
					reg = StructVariableType::Registers::PrimitiveID;
				else if (registerName == "TESSELLATION_FACTOR")
					reg = StructVariableType::Registers::TessellationFactor;
				else if (registerName == "INSIDE_TESSELLATION_FACTOR")
					reg = StructVariableType::Registers::InsideTessellationFactor;
				else if (registerName == "DOMAIN_LOCATION")
					reg = StructVariableType::Registers::DomainLocation;
				else if (registerName == "INSTANCE_ID")
					reg = StructVariableType::Registers::InstanceID;
				else if (registerName == "OUTPUT_CONTROL_POINT_ID")
					reg = StructVariableType::Registers::OutputControlPointID;
				else if (registerName == "FRAGMENT_POSITION")
					reg = StructVariableType::Registers::FragmentPosition;
				else if (registerName == "TARGET")
					reg = StructVariableType::Registers::Target;
				else if (registerName == "DISPATCH_THREAD_ID")
					reg = StructVariableType::Registers::DispatchThreadID;
				else if (registerName == "GROUP_ID")
					reg = StructVariableType::Registers::GroupID;
				else if (registerName == "GROUP_INDEX")
					reg = StructVariableType::Registers::GroupIndex;
				else if (registerName == "GROUP_THREAD_ID")
					reg = StructVariableType::Registers::GroupThreadID;
				else
					THROW_PROGRAM_PARSER_EXCEPTION("Couldn't recognize a register", registerToken);

				if (reg == StructVariableType::Registers::PrimitiveID ||
					reg == StructVariableType::Registers::InstanceID ||
					reg == StructVariableType::Registers::DispatchThreadID ||
					reg == StructVariableType::Registers::GroupID ||
					reg == StructVariableType::Registers::GroupIndex ||
					reg == StructVariableType::Registers::GroupThreadID)
				{
					if (!dataType->IsIntegral())
						THROW_PROGRAM_PARSER_EXCEPTION("Data type must be integral", registerToken);
				}

				VariableType->SetRegister(reg);

				if (indexStartIndex != identifier.GetLength())
					VariableType->SetRegisterIndex(StringUtility::ToInt8(identifier.SubString(indexStartIndex)));
			}

			RequireSymbol(SEMICOLON, "variable");

			return true;
		}

		bool Parser::ParseGlobalVariable(const Token& DeclarationToken)
		{
			bool result = true;

			if (DeclarationToken.GetType() != Token::Types::Identifier)
				return false;

			DataTypeStatement* dataType = ParseDataType(DeclarationToken);

			GlobalVariableType* variableType = Allocate<GlobalVariableType>(m_Allocator);
			variableType->SetDataType(dataType);

			Token nameToken;
			if (!MatchIdentifierToken(nameToken))
			{
				UngetToken(nameToken);
				result = false;
				goto FinishUp;
			}

			if (MatchSymbol(OPEN_BRACE))
			{
				UngetToken(nameToken);

				return false;
			}

			ValidateDataType(dataType);

			variableType->SetName(nameToken.GetName());

			RequireSymbol(SEMICOLON, "variable");

		FinishUp:
			if (result)
			{
				if (dataType->IsBuiltIn())
				{
					bool allowed = false;

					for (auto allowedType : ALLOWED_CONTEXT_FREE_DATA_TYPES)
					{
						if (allowedType != dataType->GetType())
							continue;

						allowed = true;
						break;
					}

					if (!allowed)
						THROW_PROGRAM_PARSER_EXCEPTION("Cannot be declared in global context", DeclarationToken);
				}

				m_Parameters->Variables.Add(variableType);
				AddVariableToStack(variableType);
			}
			else
				Deallocate(variableType);

			return result;
		}

		bool Parser::ParseAttribute(const Token& DeclarationToken)
		{
			if (!DeclarationToken.Matches(OPEN_SQUARE_BRACKET, Token::SearchCases::CaseSensitive))
				return false;

			Token token;
			RequireToken(token, "parse attribute");

			if (!m_AttributeParsers.Contains(token.GetName()))
				ThrowMissingException("valid attribute", "parse attributes");

			m_AttributesList.Add((*m_AttributeParsers[token.GetName()])(token));

			RequireSymbol(CLOSE_SQUARE_BRACKET, "parse attribute");

			return true;
		}

		bool Parser::ParseFunction(const Token& DeclarationToken)
		{
			DataTypeStatement* dataType = ParseDataType(DeclarationToken);

			Token nameToken;
			if (!MatchIdentifierToken(nameToken))
				return false;

			if (!MatchSymbol(OPEN_BRACE))
				return false;

			FunctionType* functionType = Allocate<FunctionType>(m_Allocator);
			m_Parameters->Functions.Add(functionType);

			functionType->AddAttributes(m_AttributesList);
			m_AttributesList.Clear();

			functionType->SetReturnDataType(dataType);

			const String& name = nameToken.GetName();

			if (name.ToLower() == Constants::VERTEX_ENTRY_POINT_NAME)
				functionType->SetType(FunctionType::Types::VertexMain);
			else if (name.ToLower() == Constants::HULL_ENTRY_POINT_NAME)
				functionType->SetType(FunctionType::Types::HullMain);
			else if (name.ToLower() == Constants::DOMAIN_ENTRY_POINT_NAME)
				functionType->SetType(FunctionType::Types::DomainMain);
			else if (name.ToLower() == Constants::GEOMETRY_ENTRY_POINT_NAME)
				functionType->SetType(FunctionType::Types::GeometryMain);
			else if (name.ToLower() == Constants::FRAGMENT_ENTRY_POINT_NAME)
				functionType->SetType(FunctionType::Types::FragmentMain);
			else if (name.ToLower() == Constants::COMPUTE_ENTRY_POINT_NAME)
				functionType->SetType(FunctionType::Types::ComputeMain);

			functionType->SetName(name);

			PushAVariableList();

			while (true)
			{
				if (MatchSymbol(CLOSE_BRACE))
					break;

				if (MatchSymbol(COMMA))
					continue;

				ParameterType* parameterType = Allocate<ParameterType>(functionType, m_Allocator);
				functionType->AddParamaeter(parameterType);

				Token parameterToken;
				RequireToken(parameterToken, "function");

				ParseParameter(parameterToken, parameterType);

				AddVariableToStack(parameterType);
			}

			ParseScopedStatements(functionType, true, EndConditions::None);

			PopAVariableList();

			return true;
		}

		bool Parser::ParseParameter(const Token& DeclarationToken, ParameterType* ParameterType)
		{
			DataTypeStatement* dataType = ParseDataType(DeclarationToken);
			ValidateDataType(dataType);

			ParameterType->SetDataType(dataType);

			Token nameToken;
			RequireIdentifierToken(nameToken, "function parameter");

			ParameterType->SetName(nameToken.GetName());

			ParsePostArrayDataType(dataType);

			return true;
		}

		BaseAttributeType* Parser::ParseDomainAttributeType(const Token& DeclarationToken)
		{
			RequireSymbol(OPEN_BRACE, "domain attribute");

			Token typeToken;
			RequireIdentifierToken(typeToken, "domain attribute");

			DomainAttributeType::Types type;
			if (typeToken.Matches("Triangle", Token::SearchCases::CaseSensitive))
				type = DomainAttributeType::Types::Triangle;
			else if (typeToken.Matches("Quad", Token::SearchCases::CaseSensitive))
				type = DomainAttributeType::Types::Quad;
			else if (typeToken.Matches("Isoline", Token::SearchCases::CaseSensitive))
				type = DomainAttributeType::Types::Isoline;
			else
				THROW_PROGRAM_PARSER_EXCEPTION("Invalid Domain type", typeToken);

			RequireSymbol(CLOSE_BRACE, "domain attribute");

			DomainAttributeType* attr = Allocate<DomainAttributeType>();
			attr->SetType(type);

			return attr;
		}

		BaseAttributeType* Parser::ParsePartitioningAttributeType(const Token& DeclarationToken)
		{
			RequireSymbol(OPEN_BRACE, "partitioning attribute");

			Token typeToken;
			RequireIdentifierToken(typeToken, "partitioning attribute");

			PartitioningAttributeType::Types type;
			if (typeToken.Matches("Integer", Token::SearchCases::CaseSensitive))
				type = PartitioningAttributeType::Types::Integer;
			else if (typeToken.Matches("FractionalEven", Token::SearchCases::CaseSensitive))
				type = PartitioningAttributeType::Types::FractionalEven;
			else if (typeToken.Matches("FractionalOdd", Token::SearchCases::CaseSensitive))
				type = PartitioningAttributeType::Types::FractionalOdd;
			else if (typeToken.Matches("PowerOfTwo", Token::SearchCases::CaseSensitive))
				type = PartitioningAttributeType::Types::PowerOfTwo;
			else
				THROW_PROGRAM_PARSER_EXCEPTION("Invalid Partitioning type", typeToken);

			RequireSymbol(CLOSE_BRACE, "partitioning attribute");

			PartitioningAttributeType* attr = Allocate<PartitioningAttributeType>();
			attr->SetType(type);

			return attr;
		}

		BaseAttributeType* Parser::ParseTopologyAttributeType(const Token& DeclarationToken)
		{
			RequireSymbol(OPEN_BRACE, "topology attribute");

			Token typeToken;
			RequireIdentifierToken(typeToken, "topology attribute");

			TopologyAttributeType::Types type;
			if (typeToken.Matches("Point", Token::SearchCases::CaseSensitive))
				type = TopologyAttributeType::Types::Point;
			else if (typeToken.Matches("Line", Token::SearchCases::CaseSensitive))
				type = TopologyAttributeType::Types::Line;
			else if (typeToken.Matches("TriangleClockwise", Token::SearchCases::CaseSensitive))
				type = TopologyAttributeType::Types::TriangleClockwise;
			else if (typeToken.Matches("TriangleCounterClockwise", Token::SearchCases::CaseSensitive))
				type = TopologyAttributeType::Types::TriangleCounterClockwise;
			else
				THROW_PROGRAM_PARSER_EXCEPTION("Invalid Topology type", typeToken);

			RequireSymbol(CLOSE_BRACE, "topology attribute");

			TopologyAttributeType* attr = Allocate<TopologyAttributeType>();
			attr->SetType(type);

			return attr;
		}

		BaseAttributeType* Parser::ParseControlPointsAttributeType(const Token& DeclarationToken)
		{
			RequireSymbol(OPEN_BRACE, "control points attribute");

			Token numberToken;
			RequireConstantToken(numberToken, "control points attribute");

			RequireSymbol(CLOSE_BRACE, "control points attribute");

			ControlPointsAttributeType* attr = Allocate<ControlPointsAttributeType>();
			attr->SetNumber(numberToken.GetConstantInt32());

			return attr;
		}

		BaseAttributeType* Parser::ParseConstantEntrypointAttributeType(const Token& DeclarationToken)
		{
			RequireSymbol(OPEN_BRACE, "constant entrypoint attribute");

			Token entrypointToken;
			RequireIdentifierToken(entrypointToken, "constant entrypoint attribute");

			RequireSymbol(CLOSE_BRACE, "constant entrypoint attribute");

			ConstantEntrypointAttributeType* attr = Allocate<ConstantEntrypointAttributeType>();
			attr->SetEntrypoint(entrypointToken.GetName());

			return attr;
		}

		BaseAttributeType* Parser::ParseMaxVertexCountAttributeType(const Token& DeclarationToken)
		{
			RequireSymbol(OPEN_BRACE, "max vertex count attribute");

			Token numberToken;
			RequireConstantToken(numberToken, "max vertex count attribute");

			RequireSymbol(CLOSE_BRACE, "max vertex count attribute");

			MaxVertexCountAttributeType* attr = Allocate<MaxVertexCountAttributeType>();
			attr->SetCount(numberToken.GetConstantInt32());

			return attr;
		}

		BaseAttributeType* Parser::ParsePrimitiveTypeAttributeType(const Token& DeclarationToken)
		{
			RequireSymbol(OPEN_BRACE, "primitive type attribute");

			Token typeToken;
			RequireIdentifierToken(typeToken, "primitive type attribute");

			PrimitiveTypeAttributeType::Types type;
			if (typeToken.Matches("Point", Token::SearchCases::CaseSensitive))
				type = PrimitiveTypeAttributeType::Types::Point;
			else if (typeToken.Matches("Line", Token::SearchCases::CaseSensitive))
				type = PrimitiveTypeAttributeType::Types::Line;
			else if (typeToken.Matches("Triangle", Token::SearchCases::CaseSensitive))
				type = PrimitiveTypeAttributeType::Types::Triangle;
			else if (typeToken.Matches("LineAdjacency", Token::SearchCases::CaseSensitive))
				type = PrimitiveTypeAttributeType::Types::LineAdjacency;
			else if (typeToken.Matches("TriangleAdjacency", Token::SearchCases::CaseSensitive))
				type = PrimitiveTypeAttributeType::Types::TriangleAdjacency;
			else
				THROW_PROGRAM_PARSER_EXCEPTION("Invalid Primitive type", typeToken);

			RequireSymbol(CLOSE_BRACE, "primitive type attribute");

			PrimitiveTypeAttributeType* attr = Allocate<PrimitiveTypeAttributeType>();
			attr->SetType(type);

			return attr;
		}

		BaseAttributeType* Parser::ParseOutputStreamTypeAttributeType(const Token& DeclarationToken)
		{
			RequireSymbol(OPEN_BRACE, "output stream type attribute");

			Token typeToken;
			RequireIdentifierToken(typeToken, "output stream type attribute");

			OutputStreamTypeAttributeType::Types type;
			if (typeToken.Matches("Point", Token::SearchCases::CaseSensitive))
				type = OutputStreamTypeAttributeType::Types::Point;
			else if (typeToken.Matches("Line", Token::SearchCases::CaseSensitive))
				type = OutputStreamTypeAttributeType::Types::Line;
			else if (typeToken.Matches("Triangle", Token::SearchCases::CaseSensitive))
				type = OutputStreamTypeAttributeType::Types::Triangle;
			else
				THROW_PROGRAM_PARSER_EXCEPTION("Invalid OuputStream type", typeToken);

			RequireSymbol(COMMA, "output stream type attribute");

			Token dataTypeToken;
			RequireIdentifierToken(dataTypeToken, "output stream type attribute");

			RequireSymbol(CLOSE_BRACE, "output stream type attribute");

			OutputStreamTypeAttributeType* attr = Allocate<OutputStreamTypeAttributeType>();
			attr->SetType(type);
			attr->SetDataType(dataTypeToken.GetName());

			return attr;
		}

		BaseAttributeType* Parser::ParseThreadCountAttributeType(const Token& DeclarationToken)
		{
			RequireSymbol(OPEN_BRACE, "thread count attribute");

			Token xCountToken;
			RequireConstantToken(xCountToken, "thread count attribute");
			RequireSymbol(COMMA, "thread count attribute");

			Token yCountToken;
			RequireConstantToken(yCountToken, "thread count attribute");
			RequireSymbol(COMMA, "thread count attribute");

			Token zCountToken;
			RequireConstantToken(zCountToken, "thread count attribute");

			RequireSymbol(CLOSE_BRACE, "thread count attribute");

			ThreadCountAttributeType* attr = Allocate<ThreadCountAttributeType>();

			attr->SetXCount(xCountToken.GetConstantInt32());
			attr->SetYCount(yCountToken.GetConstantInt32());
			attr->SetZCount(zCountToken.GetConstantInt32());

			return attr;
		}

		DataTypeStatement* Parser::ParseDataType(const Token& DeclarationToken)
		{
			const String& identifier = DeclarationToken.GetName();

			if (DeclarationToken.GetType() != Token::Types::Identifier)
				THROW_PROGRAM_PARSER_EXCEPTION("Unexpected token", DeclarationToken);

			ProgramDataTypes primitiveType = GetPrimitiveDataType(identifier);

			String userDefinedType;
			if (primitiveType == ProgramDataTypes::Unknown)
			{
				userDefinedType = identifier;

				if (!m_Parameters->Structs.ContainsIf([&identifier](auto item) { return item->GetName() == identifier; }))
					THROW_PROGRAM_PARSER_EXCEPTION("No data type found", DeclarationToken);
			}

			DataTypeStatement* stm = Allocate<DataTypeStatement>(primitiveType);

			if (primitiveType == ProgramDataTypes::Unknown)
				Construct(stm, m_Allocator, userDefinedType);

			stm->SetElementCount(ParseArrayElementCountStatement());

			return stm;
		}

		void Parser::ParsePostArrayDataType(DataTypeStatement* DataType)
		{
			DataType->SetPostElementCount(ParseArrayElementCountStatement());
		}

		Statement* Parser::ParseArrayElementCountStatement(void)
		{
			if (!MatchSymbol(OPEN_SQUARE_BRACKET))
				return nullptr;

			Token elementCountToken;
			RequireToken(elementCountToken, "data type definition");

			Statement* stm = ParseExpression(elementCountToken, EndConditions::SquareBracket);

			RequireSymbol(CLOSE_SQUARE_BRACKET, "data type definition");

			return stm;
		}

		void Parser::ValidateDataType(DataTypeStatement* DataType)
		{
			if (DataType->GetType() == ProgramDataTypes::Void)
				THROW_EXCEPTION(Categories::ProgramCompiler, "Expected a valid data type");
		}

		Statement* Parser::ParseIfStatement(const Token& DeclarationToken)
		{
			IfStatement* stm = Allocate<IfStatement>(m_Allocator);

			RequireSymbol(OPEN_BRACE, "if statement");

			Token token;
			RequireToken(token, "if statement");
			stm->SetCondition(ParseExpression(token, EndConditions::Brace));

			RequireSymbol(CLOSE_BRACE, "if statement");

			ParseScopedStatements(stm, false, EndConditions::None);

			if (MatchIdentifier(ELSE))
				stm->SetElse(ParseElseStatement(DeclarationToken));

			return stm;
		}

		Statement* Parser::ParseElseStatement(const Token& DeclarationToken)
		{
			ElseStatement* stm = Allocate<ElseStatement>(m_Allocator);

			ParseScopedStatements(stm, false, EndConditions::None);

			return stm;
		}

		Statement* Parser::ParseSwitchStatement(const Token& DeclarationToken)
		{
			SwitchStatement* stm = Allocate<SwitchStatement>(m_Allocator);

			RequireSymbol(OPEN_BRACE, "switch statement");

			Token token;
			RequireToken(token, "switch statement");

			stm->SetSelector(ParseExpression(token, EndConditions::Brace));

			RequireSymbol(CLOSE_BRACE, "switch statement");

			ParseScopedStatements(stm, true, EndConditions::None);

			return stm;
		}

		Statement* Parser::ParseDefaultStatement(const Token& DeclarationToken)
		{
			DefaultStatement* stm = Allocate<DefaultStatement>(m_Allocator);

			RequireSymbol(COLON, "default statement");

			Token nextToken;
			RequireToken(nextToken, "default statement");

			bool isSingle =
				nextToken.Matches(CASE, Token::SearchCases::CaseSensitive) ||
				nextToken.Matches(DEFAULT, Token::SearchCases::CaseSensitive);

			UngetToken(nextToken);

			if (!isSingle)
				ParseScopedStatements(stm, false, EndConditions::Break);

			return stm;
		}

		Statement* Parser::ParseCaseStatement(const Token& DeclarationToken)
		{
			CaseStatement* stm = Allocate<CaseStatement>(m_Allocator);

			Token token;
			RequireToken(token, "case statement");

			stm->SetCondition(ParseExpression(token, EndConditions::Colon));

			RequireSymbol(COLON, "case statement");

			Token nextToken;
			RequireToken(nextToken, "case statement");

			bool isSingle =
				nextToken.Matches(CASE, Token::SearchCases::CaseSensitive) ||
				nextToken.Matches(DEFAULT, Token::SearchCases::CaseSensitive);

			UngetToken(nextToken);

			if (!isSingle)
				ParseScopedStatements(stm, false, EndConditions::Break);

			return stm;
		}

		Statement* Parser::ParseForStatement(const Token& DeclarationToken)
		{
			ForStatement* stm = Allocate<ForStatement>(m_Allocator);

			RequireSymbol(OPEN_BRACE, "for statement");

			if (!MatchSymbol(SEMICOLON))
			{
				Token token;
				RequireToken(token, "for statement");

				stm->SetInitializer(ParseExpression(token, EndConditions::Semicolon));

				RequireSymbol(SEMICOLON, "for statement");
			}

			if (!MatchSymbol(SEMICOLON))
			{
				Token token;
				RequireToken(token, "for statement");

				stm->SetCondition(ParseExpression(token, EndConditions::Semicolon));

				RequireSymbol(SEMICOLON, "for statement");
			}

			if (!MatchSymbol(CLOSE_BRACE))
			{
				Token token;
				RequireToken(token, "for statement");

				stm->SetStep(ParseExpression(token, EndConditions::Brace));

				RequireSymbol(CLOSE_BRACE, "for statement");
			}

			if (!MatchSymbol(SEMICOLON))
				ParseScopedStatements(stm, false, EndConditions::None);

			return stm;
		}

		Statement* Parser::ParseDoStatement(const Token& DeclarationToken)
		{
			DoStatement* stm = Allocate<DoStatement>(m_Allocator);

			ParseScopedStatements(stm, false, EndConditions::None);

			Token token;
			RequireToken(token, "do statement");

			if (!token.Matches(WHILE, Token::SearchCases::CaseSensitive))
				THROW_PROGRAM_PARSER_EXCEPTION("Expected while after do", token);

			stm->SetWhile(ParseWhileStatement(token));

			return stm;
		}

		Statement* Parser::ParseWhileStatement(const Token& DeclarationToken)
		{
			WhileStatement* stm = Allocate<WhileStatement>(m_Allocator);

			RequireSymbol(OPEN_BRACE, "while statement");

			Token token;
			RequireToken(token, "while statement");

			stm->SetCondition(ParseExpression(token, EndConditions::Brace));

			RequireSymbol(CLOSE_BRACE, "while statement");

			if (!MatchSymbol(SEMICOLON))
				ParseScopedStatements(stm, false, EndConditions::None);

			return stm;
		}

		Statement* Parser::ParseContinueStatement(const Token& DeclarationToken)
		{
			ContinueStatement* stm = Allocate<ContinueStatement>();

			RequireSymbol(SEMICOLON, "continue statement");

			return stm;
		}

		Statement* Parser::ParseBreakStatement(const Token& DeclarationToken)
		{
			BreakStatement* stm = Allocate<BreakStatement>();

			RequireSymbol(SEMICOLON, "break statement");

			return stm;
		}

		Statement* Parser::ParseReturnStatement(const Token& DeclarationToken)
		{
			ReturnStatement* stm = Allocate<ReturnStatement>();

			Token token;
			RequireToken(token, "return statement");

			stm->SetStatement(ParseExpression(token, EndConditions::Semicolon));

			RequireSymbol(SEMICOLON, "return statement");

			return stm;
		}

		Statement* Parser::ParseDiscardStatement(const Token& DeclarationToken)
		{
			return Allocate<DiscardStatement>();
		}

		void Parser::ParseScopedStatements(StatementItemHolder* StatementItemHolder, bool MustHaveBrackets, EndConditions ConditionMask)
		{
			bool hasOpenBracket = false;

			if (MustHaveBrackets)
			{
				RequireSymbol(OPEN_BRACKET, "scope");
				hasOpenBracket = true;
			}
			else
				hasOpenBracket = MatchSymbol(OPEN_BRACKET);

			if (hasOpenBracket)
				ConditionMask = EndConditions::Bracket;

			while (true)
			{
				Token token;
				RequireToken(token, "scoped statement");

				if (IsEndCondition(token, ConditionMask))
				{
					if (hasOpenBracket)
						RequireSymbol(CLOSE_BRACKET, "scope");

					break;
				}

				Statement* bodyStm = nullptr;

				if (m_KeywordParsers.Contains(token.GetName()))
					bodyStm = (*m_KeywordParsers[token.GetName()])(token);
				else
				{
					bodyStm = ParseExpression(token, EndConditions::Semicolon);
					if (bodyStm == nullptr)
						UngetToken(token);

					RequireSymbol(SEMICOLON, "expression");
				}

				if (bodyStm != nullptr)
					StatementItemHolder->AddItem(bodyStm);

				if (ConditionMask == EndConditions::None && !hasOpenBracket)
					break;
			}
		}

		Statement* Parser::ParseVariableStatement(const Token& DeclarationToken, EndConditions ConditionMask)
		{
			if (DeclarationToken.GetType() == Token::Types::Symbol)
				return nullptr;

			Token nameToken;
			if (!MatchIdentifierToken(nameToken))
				return nullptr;

			DataTypeStatement* dataType = ParseDataType(DeclarationToken);
			ValidateDataType(dataType);

			VariableStatement* stm = Allocate<VariableStatement>(m_Parameters->Functions[m_Parameters->Functions.GetSize() - 1], m_Allocator);
			stm->SetDataType(dataType);
			stm->SetName(nameToken.GetName());

			if (MatchSymbol(EQUAL))
			{
				Token initialToken;
				RequireToken(initialToken, "variable statement");

				stm->SetInitialStatement(ParseExpression(initialToken, ConditionMask));
			}

			AddVariableToStack(stm);

			return stm;
		}

		Statement* Parser::ParseExpression(const Token& DeclarationToken, EndConditions ConditionMask)
		{
			Statement* stm = ParseVariableStatement(DeclarationToken, ConditionMask);
			if (stm != nullptr)
				return stm;

			Statement* leftHand = ParseUnaryExpression(DeclarationToken, ConditionMask);

			if (leftHand == nullptr)
				return nullptr;

			return ParseBinaryExpression(0, leftHand, ConditionMask);
		}

		Statement* Parser::ParseUnaryExpression(const Token& DeclarationToken, EndConditions ConditionMask)
		{
			Statement* stm = ParseUnaryExpressionPrefix(DeclarationToken, ConditionMask);

			Token token;
			RequireToken(token, "unary expression");

			if (token.Matches(INCREMENT, Token::SearchCases::CaseSensitive) ||
				token.Matches(DECREMENT, Token::SearchCases::CaseSensitive))
			{
				UnaryOperatorStatement::Operators op = GetUnaryOperator(token.GetName(), true);
				if (op == UnaryOperatorStatement::Operators::Unknown)
					THROW_PROGRAM_PARSER_EXCEPTION("Unrecognized operator", token);

				UnaryOperatorStatement* unaryStm = Allocate<UnaryOperatorStatement>();
				unaryStm->SetOperator(op);

				unaryStm->SetStatement(stm);

				stm = unaryStm;
			}
			else
				UngetToken(token);

			return stm;
		}

		Statement* Parser::ParseUnaryExpressionPrefix(const Token& DeclarationToken, EndConditions ConditionMask)
		{
			if (DeclarationToken.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive) ||
				DeclarationToken.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive))
			{
				Token token;
				RequireToken(token, "unary expression");

				Statement* stm = ParseExpression(token, EndConditions::Brace);

				RequireSymbol(CLOSE_BRACE, "expression");

				return stm;
			}

			if (DeclarationToken.Matches(EXLAMATION, Token::SearchCases::CaseSensitive) ||
				DeclarationToken.Matches(TILDE, Token::SearchCases::CaseSensitive) ||
				DeclarationToken.Matches(MINES, Token::SearchCases::CaseSensitive) ||
				DeclarationToken.Matches(INCREMENT, Token::SearchCases::CaseSensitive) ||
				DeclarationToken.Matches(DECREMENT, Token::SearchCases::CaseSensitive))
			{
				return ParseUnaryOperatorExpression(DeclarationToken, ConditionMask);
			}

			if (DeclarationToken.Matches(OPEN_BRACKET, Token::SearchCases::CaseSensitive))
			{
				return ParseArrayExpression(DeclarationToken, ConditionMask);
			}

			if (DeclarationToken.GetType() == Token::Types::Identifier)
			{
				Statement* stm = ParseFunctionCallStatement(DeclarationToken);
				if (stm != nullptr)
					return stm;

				stm = ParseVariableAccessStatement(DeclarationToken);

				if (MatchSymbol(OPEN_SQUARE_BRACKET))
				{
					Token elementToekn;
					RequireToken(elementToekn, "unary expression");

					Statement* arrayAccessStm = ParseArrayElementAccessStatement(elementToekn, stm);

					if (arrayAccessStm != nullptr)
						stm = arrayAccessStm;
				}

				Token memberAccessToken;
				RequireToken(memberAccessToken, "unary expression");

				DataTypeStatement* parentDataType = nullptr;
				VariableType* varType = FindVariableType(DeclarationToken.GetName());
				if (varType != nullptr)
					parentDataType = varType->GetDataType();

				Statement* memberAccessStm = ParseMemberAccessStatement(memberAccessToken, stm, parentDataType);
				if (memberAccessStm != stm)
					return memberAccessStm;

				UngetToken(memberAccessToken);

				return stm;
			}

			if (DeclarationToken.GetIsConstant())
			{
				return ParseConstantStatement(DeclarationToken);
			}

			return nullptr;
		}

		Statement* Parser::ParseUnaryOperatorExpression(const Token& DeclarationToken, EndConditions ConditionMask)
		{
			Token token;
			RequireToken(token, "unary expression");

			if (IsEndCondition(token, ConditionMask))
				return nullptr;

			UnaryOperatorStatement::Operators op = GetUnaryOperator(DeclarationToken.GetName(), false);
			if (op == UnaryOperatorStatement::Operators::Unknown)
				THROW_PROGRAM_PARSER_EXCEPTION("Unrecognized token", DeclarationToken);

			UnaryOperatorStatement* stm = Allocate<UnaryOperatorStatement>();
			stm->SetOperator(op);

			stm->SetStatement(ParseUnaryExpression(token, ConditionMask));

			return stm;
		}

		Statement* Parser::ParseBinaryExpression(int8 LeftHandPrecedence, Statement* LeftHandStatement, EndConditions ConditionMask)
		{
			while (true)
			{
				Token token;
				RequireToken(token, "binary expression");

				if (IsEndCondition(token, ConditionMask))
					break;

				OperatorStatement::Operators op = GetOperator(token.GetName());
				if (op == OperatorStatement::Operators::Unknown)
				{
					UngetToken(token);
					break;
				}

				int8 precedence = GetOperatorPrecedence(op);
				if (precedence < LeftHandPrecedence)
					break;

				OperatorStatement* stm = Allocate<OperatorStatement>();
				stm->SetOperator(op);
				stm->SetLeft(LeftHandStatement);

				Token rightHandToken;
				RequireToken(rightHandToken, "binary expression");

				Statement* rightHandStm = ParseUnaryExpression(rightHandToken, ConditionMask);

				Token nextToken;
				RequireToken(nextToken, "binary expression");

				op = GetOperator(nextToken.GetName());
				int8 rightPrecedence = GetOperatorPrecedence(op);
				UngetToken(nextToken);

				if (precedence < rightPrecedence)
					if ((rightHandStm = ParseBinaryExpression(precedence + 1, rightHandStm, ConditionMask)) == nullptr)
					{
						Deallocate(stm);

						return nullptr;
					}

				stm->SetRight(rightHandStm);
				LeftHandStatement = stm;
			}

			return LeftHandStatement;
		}

		Statement* Parser::ParseArrayExpression(const Token& DeclarationToken, EndConditions ConditionMask)
		{
			ArrayStatement* stm = Allocate<ArrayStatement>(m_Allocator);

			while (true)
			{
				if (MatchSymbol(CLOSE_BRACKET))
					break;

				if (MatchSymbol(COMMA))
					continue;

				Token token;
				RequireToken(token, "binary expression");

				if (IsEndCondition(token, ConditionMask))
					return nullptr;

				stm->AddItem(ParseExpression(token, EndConditions::Comma | EndConditions::Bracket));
			}

			return stm;
		}

		Statement* Parser::ParseConstantStatement(const Token& DeclarationToken)
		{
			ConstantStatement* stm = Allocate<ConstantStatement>();

			switch (DeclarationToken.GetType())
			{
			case Token::Types::ConstantBool:
				stm->SetBool(DeclarationToken.GetConstantBool());
				break;

			case Token::Types::ConstantInt32:
				stm->SetFloat32(DeclarationToken.GetConstantInt32());
				break;
				break;

			case Token::Types::ConstantFloat32:
				stm->SetFloat32(DeclarationToken.GetConstantFloat32());
				break;

			default:
				THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
			}

			return stm;
		}

		Statement* Parser::ParseVariableAccessStatement(const Token& DeclarationToken)
		{
			VariableAccessStatement* stm = Allocate<VariableAccessStatement>(m_Allocator);

			stm->SetName(DeclarationToken.GetName());

			Token token;
			RequireToken(token, "variable access statement");

			DataTypeStatement* parentDataType = nullptr;
			VariableType* varType = FindVariableType(stm->GetName());
			if (varType != nullptr)
				parentDataType = varType->GetDataType();

			return ParseMemberAccessStatement(token, stm, parentDataType);
		}

		Statement* Parser::ParseArrayElementAccessStatement(const Token& DeclarationToken, Statement* ArrayStatement)
		{
			ArrayElementAccessStatement* stm = Allocate<ArrayElementAccessStatement>();

			stm->SetArrayStatement(ArrayStatement);
			stm->SetElementStatement(ParseExpression(DeclarationToken, EndConditions::SquareBracket));

			RequireSymbol(CLOSE_SQUARE_BRACKET, "array element access");

			return stm;
		}

		Statement* Parser::ParseMemberAccessStatement(const Token& DeclarationToken, Statement* LeftStatement, const DataTypeStatement* ParentDataType)
		{
			StructType* parentStructType = nullptr;
			if (ParentDataType != nullptr && !ParentDataType->IsBuiltIn())
				parentStructType = FindStructType(ParentDataType->GetUserDefined());

			if (DeclarationToken.Matches(DOT, Token::SearchCases::CaseSensitive))
			{
				MemberAccessStatement* stm = Allocate<MemberAccessStatement>();

				stm->SetLeft(LeftStatement);

				Token memberToken;
				RequireIdentifierToken(memberToken, "member access statement");

				stm->SetRight(ParseVariableAccessStatement(memberToken));

				return stm;
			}

			UngetToken(DeclarationToken);

			return LeftStatement;
		}

		Statement* Parser::ParseFunctionCallStatement(const Token& DeclarationToken)
		{
			if (!MatchSymbol(OPEN_BRACE))
				return nullptr;

			FunctionCallStatement* stm = Allocate<FunctionCallStatement>(m_Allocator);

			stm->SetFunctionName(DeclarationToken.GetName());

			while (true)
			{
				if (MatchSymbol(CLOSE_BRACE))
					break;

				if (MatchSymbol(COMMA))
					continue;

				Token token;
				RequireToken(token, "function call statement");

				stm->AddArgument(ParseExpression(token, EndConditions::Comma | EndConditions::Brace));
			}

			Token token;
			RequireToken(token, "function call statement");

			DataTypeStatement* parentDataType = nullptr;
			FunctionType* funcType = FindFunctionType(stm->GetFunctionName());
			if (funcType != nullptr)
				parentDataType = funcType->GetReturnDataType();

			return ParseMemberAccessStatement(token, stm, parentDataType);
		}

		bool Parser::IsEndCondition(const Token& DeclarationToken, EndConditions ConditionMask)
		{
			bool isTheEnd =
				(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Colon) && DeclarationToken.Matches(COLON, Token::SearchCases::CaseSensitive)) ||
				(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Semicolon) && DeclarationToken.Matches(SEMICOLON, Token::SearchCases::CaseSensitive)) ||
				(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Brace) && DeclarationToken.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive)) ||
				(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Comma) && DeclarationToken.Matches(COMMA, Token::SearchCases::CaseSensitive)) ||
				(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Bracket) && (DeclarationToken.Matches(OPEN_BRACKET, Token::SearchCases::CaseSensitive) || DeclarationToken.Matches(CLOSE_BRACKET, Token::SearchCases::CaseSensitive))) ||
				(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::SquareBracket) && (DeclarationToken.Matches(OPEN_SQUARE_BRACKET, Token::SearchCases::CaseSensitive) || DeclarationToken.Matches(CLOSE_SQUARE_BRACKET, Token::SearchCases::CaseSensitive)) ||
					(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Break) && DeclarationToken.Matches(BREAK, Token::SearchCases::CaseSensitive)));

			if (!isTheEnd)
				return false;

			UngetToken(DeclarationToken);

			return true;
		}

		void Parser::PushAVariableList(void)
		{
			VariableList variables(m_Allocator);

			m_VariablesStack.Push(variables);
		}

		void Parser::PopAVariableList(void)
		{
			m_VariablesStack.Pop();
		}

		void Parser::AddVariableToStack(VariableType* Variable)
		{
			CoreDebugAssert(Categories::ProgramCompiler, m_VariablesStack.GetSize() != 0, "There's nothing in the m_VariablesStack");

			VariableList list(m_Allocator);
			m_VariablesStack.Pop(&list);

			list.Add(Variable);

			m_VariablesStack.Push(list);
		}

		StructType* Parser::FindStructType(const String& Name)
		{
			int32 index = m_Parameters->Structs.FindIf([&Name](const auto structType) { return structType->GetName() == Name; });
			if (index == -1)
				return nullptr;

			return m_Parameters->Structs[index];
		}

		VariableType* Parser::FindVariableType(const String& Name)
		{
			for (const auto& list : m_VariablesStack)
			{
				int32 index = list.FindIf([&Name](auto& variable) { return variable->GetName() == Name; });
				if (index == -1)
					continue;

				return list[index];
			}

			return nullptr;
		}

		FunctionType* Parser::FindFunctionType(const String& Name)
		{
			int32 index = m_Parameters->Functions.FindIf([&Name](const auto structType) { return structType->GetName() == Name; });
			if (index == -1)
				return nullptr;

			return m_Parameters->Functions[index];
		}

		ProgramDataTypes Parser::GetPrimitiveDataType(const String& Name)
		{
			static bool initialized = false;
			static Map<String, ProgramDataTypes> dataTypesName;

			if (!initialized)
			{
				initialized = true;

				dataTypesName["void"] = ProgramDataTypes::Void;
				dataTypesName["bool"] = ProgramDataTypes::Bool;
				dataTypesName["int"] = ProgramDataTypes::Integer;
				dataTypesName["uint"] = ProgramDataTypes::UnsignedInteger;
				dataTypesName["float"] = ProgramDataTypes::Float;
				dataTypesName["double"] = ProgramDataTypes::Double;
				dataTypesName["int2"] = ProgramDataTypes::Integer2;
				dataTypesName["uint2"] = ProgramDataTypes::UnsignedInteger2;
				dataTypesName["float2"] = ProgramDataTypes::Float2;
				dataTypesName["double2"] = ProgramDataTypes::Double2;
				dataTypesName["int3"] = ProgramDataTypes::Integer3;
				dataTypesName["uint3"] = ProgramDataTypes::UnsignedInteger3;
				dataTypesName["float3"] = ProgramDataTypes::Float3;
				dataTypesName["double3"] = ProgramDataTypes::Double3;
				dataTypesName["int4"] = ProgramDataTypes::Integer4;
				dataTypesName["uint4"] = ProgramDataTypes::UnsignedInteger4;
				dataTypesName["float4"] = ProgramDataTypes::Float4;
				dataTypesName["double4"] = ProgramDataTypes::Double4;
				dataTypesName["matrix4d"] = ProgramDataTypes::Matrix4F;
				dataTypesName["matrix4f"] = ProgramDataTypes::Matrix4D;
				dataTypesName["texture2D"] = ProgramDataTypes::Texture2D;
			}

			if (dataTypesName.Contains(Name))
				return dataTypesName[Name];

			return ProgramDataTypes::Unknown;
		}
	}
}