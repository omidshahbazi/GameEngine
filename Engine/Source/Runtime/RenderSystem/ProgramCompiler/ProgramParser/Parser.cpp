// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ProgramParser\Parser.h>
#include <ProgramParser\AbstractSyntaxTree\StructType.h>
#include <ProgramParser\AbstractSyntaxTree\FunctionType.h>
#include <ProgramParser\AbstractSyntaxTree\ParameterType.h>
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
			m_Structs(m_Allocator)
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
		}

		void Parser::Parse(Parameters& Parameters)
		{
			Tokenizer::Parse();

			m_Parameters = &Parameters;

			while (true)
			{
				Token token;
				if (!GetToken(token))
					break;

				if (ParseStruct(token))
					continue;

				if (ParseVariable(token))
					continue;

				if (ParseFunction(token))
					continue;
			}

			CoreDebugAssert(Categories::RenderSystem, m_Structs.GetSize() == 0, "Structs didn't get evacuated from stack");

			m_Parameters = nullptr;
		}

		bool Parser::ParseStruct(const Token& DeclarationToken)
		{
			if (DeclarationToken.GetTokenType() != Token::Types::Identifier)
			{
				UngetToken(DeclarationToken);
				return false;
			}

			if (!DeclarationToken.Matches(STRUCT, Token::SearchCases::CaseSensitive))
				return false;

			StructType* structType = Allocate<StructType>(m_Allocator);

			Token nameToken;
			RequireIdentifierToken(nameToken);
			structType->SetName(nameToken.GetIdentifier());

			RequireSymbol(OPEN_BRACKET, "struct");

			m_Structs.Push(structType);

			while (true)
			{
				if (MatchSymbol(CLOSE_BRACKET))
					break;

				Token variableToken;
				RequireToken(variableToken);

				ParseVariable(variableToken);
			}

			m_Structs.Pop();

			RequireSymbol(SEMICOLON, "struct");

			m_Parameters->Structs.Add(structType);

			return true;
		}

		bool Parser::ParseVariable(const Token& DeclarationToken)
		{
			bool result = true;

			VariableType* variableType = Allocate<VariableType>(m_Allocator);

			DataTypeStatement* dataType = ParseDataType(DeclarationToken);
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

			variableType->SetName(nameToken.GetIdentifier());

			if (MatchSymbol(COLON))
			{
				Token registerToken;
				RequireIdentifierToken(registerToken);
				if (variableType->GetName() == registerToken.GetIdentifier())
					THROW_PROGRAM_PARSER_EXCEPTION("Variable name cannot be same as register", nameToken);

				variableType->SetRegister(registerToken.GetIdentifier());
			}

			RequireSymbol(SEMICOLON, "variable");

		FinishUp:
			if (result)
			{
				if (m_Structs.GetSize() == 0)
				{
					bool allowed = true;

					if (dataType->IsBuiltIn())
					{
						allowed = false;

						for (auto allowedType : ALLOWED_CONTEXT_FREE_DATA_TYPES)
						{
							if (allowedType != dataType->GetType())
								continue;

							allowed = true;
							break;
						}

						if (!allowed)
							THROW_PROGRAM_PARSER_EXCEPTION("Cannot declared in global context", DeclarationToken);
					}

					if (allowed)
						m_Parameters->Variables.Add(variableType);
				}
				else
				{
					StructType* structType = nullptr;
					m_Structs.Peek(&structType);

					structType->AddItem(variableType);
				}
			}
			else
				Deallocate(variableType);

			return result;
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

			functionType->SetReturnDataType(dataType);

			const String& name = nameToken.GetIdentifier();

			if (name.ToLower() == Constants::VERTEX_ENTRY_POINT_NAME)
				functionType->SetType(FunctionType::Types::VertexMain);
			else if (name.ToLower() == Constants::TESSELLATION_ENTRY_POINT_NAME)
				functionType->SetType(FunctionType::Types::TessellationMain);
			else if (name.ToLower() == Constants::GEOMETRY_ENTRY_POINT_NAME)
				functionType->SetType(FunctionType::Types::GeometryMain);
			else if (name.ToLower() == Constants::FRAGMENT_ENTRY_POINT_NAME)
				functionType->SetType(FunctionType::Types::FragmentMain);
			else if (name.ToLower() == Constants::COMPUTE_ENTRY_POINT_NAME)
				functionType->SetType(FunctionType::Types::ComputeMain);

			functionType->SetName(name);

			while (true)
			{
				if (MatchSymbol(CLOSE_BRACE))
					break;

				if (MatchSymbol(COMMA))
					continue;

				ParameterType* parameterType = Allocate<ParameterType>(m_Allocator);
				functionType->AddParamaeter(parameterType);

				Token parameterToken;
				RequireToken(parameterToken);

				ParseFunctionParameter(parameterToken, parameterType);
			}

			return ParseScopedStatements(functionType, true, EndConditions::None);
		}

		bool Parser::ParseFunctionParameter(const Token& DeclarationToken, ParameterType* Parameter)
		{
			DataTypeStatement* dataType = ParseDataType(DeclarationToken);

			Parameter->SetDataType(dataType);

			Token nameToken;
			RequireIdentifierToken(nameToken);

			Parameter->SetName(nameToken.GetIdentifier());

			if (MatchSymbol(COLON))
			{
				Token registerToken;
				RequireIdentifierToken(registerToken);
				Parameter->SetRegister(registerToken.GetIdentifier());
			}

			return true;
		}

		DataTypeStatement* Parser::ParseDataType(const Token& DeclarationToken)
		{
			const String& identifier = DeclarationToken.GetIdentifier();

			if (DeclarationToken.GetTokenType() != Token::Types::Identifier)
				THROW_PROGRAM_PARSER_EXCEPTION("Unexpected token", DeclarationToken);

			ProgramDataTypes primitiveType = GetPrimitiveDataType(identifier);
			if (primitiveType == ProgramDataTypes::Void)
				THROW_PROGRAM_PARSER_EXCEPTION("Expected a valid data type", DeclarationToken);

			String userDefinedType;
			if (primitiveType == ProgramDataTypes::Unknown)
			{
				userDefinedType = identifier;

				if (!m_Parameters->Structs.ContainsIf([&identifier](auto item) { return item->GetName() == identifier; }))
					THROW_PROGRAM_PARSER_EXCEPTION("No data type found", DeclarationToken);
			}

			Statement* elementCountStatement = nullptr;
			if (MatchSymbol(OPEN_SQUARE_BRACKET))
			{
				Token elementCountToken;
				RequireToken(elementCountToken);

				elementCountStatement = ParseExpression(elementCountToken, EndConditions::Bracket);

				RequireToken(elementCountToken);

				RequireSymbol(CLOSE_BRACKET, "data type definition");
			}

			DataTypeStatement* stm = Allocate<DataTypeStatement>();

			if (primitiveType == ProgramDataTypes::Unknown)
				Construct(stm, m_Allocator, userDefinedType, elementCountStatement);
			else
				Construct(stm, primitiveType, elementCountStatement);

			return stm;
		}

		Statement* Parser::ParseIfStatement(const Token& DeclarationToken)
		{
			IfStatement* stm = Allocate<IfStatement>(m_Allocator);

			RequireSymbol(OPEN_BRACE, "if statement");

			Token token;
			RequireToken(token);
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

			if (!ParseScopedStatements(stm, false, EndConditions::None))
			{
				Deallocate(stm);

				return nullptr;
			}

			return stm;
		}

		Statement* Parser::ParseSwitchStatement(const Token& DeclarationToken)
		{
			SwitchStatement* stm = Allocate<SwitchStatement>(m_Allocator);

			RequireSymbol(OPEN_BRACE, "switch statement");

			Token token;
			RequireToken(token);

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
			RequireToken(nextToken);

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
			RequireToken(token);

			stm->SetCondition(ParseExpression(token, EndConditions::Colon));

			RequireSymbol(COLON, "case statement");

			Token nextToken;
			RequireToken(nextToken);

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
				RequireToken(token);

				stm->SetInitializer(ParseExpression(token, EndConditions::Semicolon));

				RequireSymbol(SEMICOLON, "for statement");
			}

			if (!MatchSymbol(SEMICOLON))
			{
				Token token;
				RequireToken(token);

				stm->SetCondition(ParseExpression(token, EndConditions::Semicolon));

				RequireSymbol(SEMICOLON, "for statement");
			}

			if (!MatchSymbol(CLOSE_BRACE))
			{
				Token token;
				RequireToken(token);

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
			RequireToken(token);

			if (!token.Matches(WHILE, Token::SearchCases::CaseSensitive))
				THROW_PROGRAM_PARSER_EXCEPTION("expected while after do", token);

			stm->SetWhile(ParseWhileStatement(token));

			return stm;
		}

		Statement* Parser::ParseWhileStatement(const Token& DeclarationToken)
		{
			WhileStatement* stm = Allocate<WhileStatement>(m_Allocator);

			RequireSymbol(OPEN_BRACE, "while statement");

			Token token;
			RequireToken(token);

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
			RequireToken(token);

			stm->SetStatement(ParseExpression(token, EndConditions::Semicolon));

			RequireSymbol(SEMICOLON, "return statement");

			return stm;
		}

		Statement* Parser::ParseDiscardStatement(const Token& DeclarationToken)
		{
			return Allocate<DiscardStatement>();
		}

		bool Parser::ParseScopedStatements(StatementItemHolder* StatementItemHolder, bool MustHaveBrackets, EndConditions ConditionMask)
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
				RequireToken(token);

				if (IsEndCondition(token, ConditionMask))
				{
					if (hasOpenBracket)
						RequireSymbol(CLOSE_BRACKET, "scope");

					break;
				}

				Statement* bodyStm = nullptr;

				if (m_KeywordParsers.Contains(token.GetIdentifier()))
					bodyStm = (*m_KeywordParsers[token.GetIdentifier()])(token);
				else
				{
					bodyStm = ParseExpression(token, EndConditions::Semicolon);

					RequireSymbol(SEMICOLON, "expression");
				}

				if (bodyStm == nullptr)
					THROW_PROGRAM_PARSER_EXCEPTION("Unexpected token", token);

				StatementItemHolder->AddItem(bodyStm);

				if (ConditionMask == EndConditions::None && !hasOpenBracket)
					break;
			}

			return true;
		}

		Statement* Parser::ParseVariableStatement(const Token& DeclarationToken, EndConditions ConditionMask)
		{
			if (DeclarationToken.GetTokenType() == Token::Types::Symbol)
				return nullptr;

			Token nameToken;
			if (!MatchIdentifierToken(nameToken))
				return nullptr;

			VariableStatement* stm = Allocate<VariableStatement>(m_Allocator);
			stm->SetDataType(ParseDataType(DeclarationToken));
			stm->SetName(nameToken.GetIdentifier());

			if (MatchSymbol(EQUAL))
			{
				Token initialToken;
				RequireToken(initialToken);

				stm->SetInitialStatement(ParseExpression(initialToken, ConditionMask));
			}

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
			RequireToken(token);
			if (token.Matches(INCREMENT, Token::SearchCases::CaseSensitive) ||
				token.Matches(DECREMENT, Token::SearchCases::CaseSensitive))
			{
				UnaryOperatorStatement::Operators op = GetUnaryOperator(token.GetIdentifier(), true);
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
				RequireToken(token);

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

			if (DeclarationToken.GetTokenType() == Token::Types::Identifier)
			{
				Statement* stm = ParseFunctionCallStatement(DeclarationToken);
				if (stm != nullptr)
					return stm;

				stm = ParseVariableAccessStatement(DeclarationToken);

				while (true)
				{
					if (MatchSymbol(OPEN_SQUARE_BRACKET))
					{
						Token elementToekn;
						RequireToken(elementToekn);

						Statement* arrayAccessStm = ParseArrayElementAccessStatement(elementToekn, stm);

						if (arrayAccessStm != nullptr)
							stm = arrayAccessStm;
					}
					else
						break;
				}

				return stm;
			}

			if (DeclarationToken.GetTokenType() == Token::Types::Constant)
			{
				return ParseConstantStatement(DeclarationToken);
			}

			return nullptr;
		}

		Statement* Parser::ParseUnaryOperatorExpression(const Token& DeclarationToken, EndConditions ConditionMask)
		{
			Token token;
			RequireToken(token);

			if (IsEndCondition(token, ConditionMask))
				return nullptr;

			UnaryOperatorStatement::Operators op = GetUnaryOperator(DeclarationToken.GetIdentifier(), false);
			if (op == UnaryOperatorStatement::Operators::Unknown)
				THROW_PROGRAM_PARSER_EXCEPTION("Unrecognized operator", DeclarationToken);

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
				RequireToken(token);

				if (IsEndCondition(token, ConditionMask))
					break;

				OperatorStatement::Operators op = GetOperator(token.GetIdentifier());
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
				RequireToken(rightHandToken);

				Statement* rightHandStm = ParseUnaryExpression(rightHandToken, ConditionMask);

				Token nextToken;
				RequireToken(nextToken);

				op = GetOperator(nextToken.GetIdentifier());
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
				RequireToken(token);

				if (IsEndCondition(token, ConditionMask))
					return nullptr;

				stm->AddItem(ParseExpression(token, EndConditions::Comma | EndConditions::Bracket));
			}

			return stm;
		}

		Statement* Parser::ParseConstantStatement(const Token& DeclarationToken)
		{
			ConstantStatement* stm = Allocate<ConstantStatement>();

			if (DeclarationToken.GetIdentifier() == "true")
				stm->SetBool(true);
			else if (DeclarationToken.GetIdentifier() == "false")
				stm->SetBool(false);
			else if (DeclarationToken.GetIdentifier().Contains("."))
				stm->SetFloat32(DeclarationToken.GetConstantFloat32());
			else
				stm->SetFloat32(DeclarationToken.GetConstantInt32());

			return stm;
		}

		Statement* Parser::ParseVariableAccessStatement(const Token& DeclarationToken)
		{
			VariableAccessStatement* stm = Allocate<VariableAccessStatement>(m_Allocator);

			stm->SetName(DeclarationToken.GetIdentifier());

			Token token;
			RequireToken(token);

			return ParseMemberAccessStatement(token, stm);
		}

		Statement* Parser::ParseArrayElementAccessStatement(const Token& DeclarationToken, Statement* ArrayStatement)
		{
			ArrayElementAccessStatement* stm = Allocate<ArrayElementAccessStatement>();

			stm->SetArrayStatement(ArrayStatement);
			stm->SetElementStatement(ParseExpression(DeclarationToken, EndConditions::SquareBracket));

			RequireSymbol(CLOSE_SQUARE_BRACKET, "array element access");

			return stm;
		}

		Statement* Parser::ParseMemberAccessStatement(const Token& DeclarationToken, Statement* LeftStatement)
		{
			if (DeclarationToken.Matches(DOT, Token::SearchCases::CaseSensitive))
			{
				MemberAccessStatement* stm = Allocate<MemberAccessStatement>();

				stm->SetLeft(LeftStatement);

				Token memberToken;
				RequireIdentifierToken(memberToken);

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

			stm->SetFunctionName(DeclarationToken.GetIdentifier());

			while (true)
			{
				if (MatchSymbol(CLOSE_BRACE))
					break;

				if (MatchSymbol(COMMA))
					continue;

				Token token;
				RequireToken(token);

				stm->GetArguments()->AddItem(ParseExpression(token, EndConditions::Comma | EndConditions::Brace));
			}

			Token token;
			RequireToken(token);

			return ParseMemberAccessStatement(token, stm);
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

		ProgramDataTypes Parser::GetPrimitiveDataType(const String& Name)
		{
			static bool initialized = false;
			static Map<String, ProgramDataTypes> dataTypesName;

			if (!initialized)
			{
				initialized = true;

				dataTypesName["void"] = ProgramDataTypes::Void;
				dataTypesName["bool"] = ProgramDataTypes::Bool;
				dataTypesName["float"] = ProgramDataTypes::Float;
				dataTypesName["double"] = ProgramDataTypes::Double;
				dataTypesName["float2"] = ProgramDataTypes::Float2;
				dataTypesName["double2"] = ProgramDataTypes::Double2;
				dataTypesName["float3"] = ProgramDataTypes::Float3;
				dataTypesName["double3"] = ProgramDataTypes::Double3;
				dataTypesName["float4"] = ProgramDataTypes::Float4;
				dataTypesName["double4"] = ProgramDataTypes::Double4;
				dataTypesName["matrix4"] = ProgramDataTypes::Matrix4;
				dataTypesName["texture2D"] = ProgramDataTypes::Texture2D;
			}

			if (dataTypesName.Contains(Name))
				return dataTypesName[Name];

			return ProgramDataTypes::Unknown;
		}
	}
}