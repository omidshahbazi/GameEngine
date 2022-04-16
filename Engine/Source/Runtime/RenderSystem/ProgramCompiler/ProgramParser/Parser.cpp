// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ProgramParser\Parser.h>
#include <ProgramParser\AbstractSyntaxTree\StructType.h>
#include <ProgramParser\AbstractSyntaxTree\FunctionType.h>
#include <ProgramParser\AbstractSyntaxTree\ParameterType.h>
#include <ProgramParser\AbstractSyntaxTree\DataTypeStatement.h>
#include <ProgramParser\AbstractSyntaxTree\IfStatement.h>
#include <ProgramParser\AbstractSyntaxTree\ElseStatement.h>
#include <ProgramParser\AbstractSyntaxTree\SwitchStatement.h>
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

		UnaryOperatorStatement::Operators GetUnaryOperator(const String& Symbol)
		{
			static bool initialized = false;
			static Map<String, UnaryOperatorStatement::Operators> operators;

			if (!initialized)
			{
				initialized = true;

				operators["-"] = UnaryOperatorStatement::Operators::Minus;
			}

			if (operators.Contains(Symbol))
				return operators[Symbol];

			return UnaryOperatorStatement::Operators::Unknown;
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

			case OperatorStatement::Operators::Multiplication:
			case OperatorStatement::Operators::Division:
				return 3;

			case OperatorStatement::Operators::Remainder:
				return 4;

			case OperatorStatement::Operators::Addition:
			case OperatorStatement::Operators::Subtraction:
				return 5;
			}

			return -1;
		}

		Parser::Parser(AllocatorBase* Allocator, const String& Text) :
			Tokenizer(Text),
			m_Allocator(Allocator),
			m_Parameters(nullptr),
			m_Structs(m_Allocator)
		{
			m_KeywordParsers[IF] = std::make_shared<KeywordParseFunction>([&](Token& Token) { return ParseIfStatement(Token); });
			m_KeywordParsers[ELSE] = std::make_shared<KeywordParseFunction>([&](Token& Token) { return ParseElseStatement(Token); });
			m_KeywordParsers[SWITCH] = std::make_shared<KeywordParseFunction>([&](Token& Token) { return ParseSwitchStatement(Token); });
			m_KeywordParsers[CASE] = std::make_shared<KeywordParseFunction>([&](Token& Token) { return ParseCaseStatement(Token); });
			m_KeywordParsers[DEFAULT] = std::make_shared<KeywordParseFunction>([&](Token& Token) { return ParseCaseStatement(Token); });
			m_KeywordParsers[FOR] = std::make_shared<KeywordParseFunction>([&](Token& Token) { return ParseForStatement(Token); });
			m_KeywordParsers[DO] = std::make_shared<KeywordParseFunction>([&](Token& Token) { return ParseDoStatement(Token); });
			m_KeywordParsers[WHILE] = std::make_shared<KeywordParseFunction>([&](Token& Token) { return ParseWhileStatement(Token); });
			m_KeywordParsers[CONTINUE] = std::make_shared<KeywordParseFunction>([&](Token& Token) { return ParseContinueStatement(Token); });
			m_KeywordParsers[BREAK] = std::make_shared<KeywordParseFunction>([&](Token& Token) { return ParseBreakStatement(Token); });
			m_KeywordParsers[RETURN] = std::make_shared<KeywordParseFunction>([&](Token& Token) { return ParseReturnStatement(Token); });
			m_KeywordParsers[DISCARD] = std::make_shared<KeywordParseFunction>([&](Token& Token) { return ParseDiscardStatement(Token); });
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

		bool Parser::ParseStruct(Token& DeclarationToken)
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

		bool Parser::ParseVariable(Token& DeclarationToken)
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

		bool Parser::ParseFunction(Token& DeclarationToken)
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

			return ParseScopedStatements(functionType);
		}

		bool Parser::ParseFunctionParameter(Token& DeclarationToken, ParameterType* Parameter)
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

		DataTypeStatement* Parser::ParseDataType(Token& DeclarationToken)
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

		Statement* Parser::ParseIfStatement(Token& DeclarationToken)
		{
			IfStatement* stm = Allocate<IfStatement>(m_Allocator);

			RequireSymbol(OPEN_BRACE, "if statement");

			Token token;
			RequireToken(token);
			stm->SetCondition(ParseExpression(token, EndConditions::Brace));

			RequireSymbol(CLOSE_BRACE, "if statement");

			ParseScopedStatements(stm);

			if (MatchIdentifier(ELSE))
				stm->SetElse(ParseElseStatement(DeclarationToken));

			return stm;
		}

		Statement* Parser::ParseElseStatement(Token& DeclarationToken)
		{
			ElseStatement* stm = Allocate<ElseStatement>(m_Allocator);

			if (!ParseScopedStatements(stm))
			{
				Deallocate(stm);

				return nullptr;
			}

			return stm;
		}

		Statement* Parser::ParseSwitchStatement(Token& DeclarationToken)
		{
			SwitchStatement* stm = Allocate<SwitchStatement>(m_Allocator);

			RequireSymbol(OPEN_BRACE, "switch statement");

			Token token;
			RequireToken(token);

			stm->SetSelector(ParseExpression(token, EndConditions::Brace));

			RequireSymbol(CLOSE_BRACE, "switch statement");

			ParseScopedStatements(stm);

			return stm;
		}

		Statement* Parser::ParseCaseStatement(Token& DeclarationToken)
		{
			return nullptr;
		}

		Statement* Parser::ParseForStatement(Token& DeclarationToken)
		{
			return nullptr;
		}

		Statement* Parser::ParseDoStatement(Token& DeclarationToken)
		{
			return nullptr;
		}

		Statement* Parser::ParseWhileStatement(Token& DeclarationToken)
		{
			return nullptr;
		}

		Statement* Parser::ParseContinueStatement(Token& DeclarationToken)
		{
			return nullptr;
		}

		Statement* Parser::ParseBreakStatement(Token& DeclarationToken)
		{
			return nullptr;
		}

		Statement* Parser::ParseReturnStatement(Token& DeclarationToken)
		{
			ReturnStatement* stm = Allocate<ReturnStatement>();

			Token token;
			RequireToken(token);

			stm->SetStatement(ParseExpression(token, EndConditions::Semicolon));

			RequireSymbol(SEMICOLON, "return statement");

			return stm;
		}

		Statement* Parser::ParseDiscardStatement(Token& DeclarationToken)
		{
			return Allocate<DiscardStatement>();
		}

		bool Parser::ParseScopedStatements(StatementItemHolder* StatementItemHolder)
		{
			bool hasOpenBracket = MatchSymbol(OPEN_BRACKET);

			bool firstStatementParsed = false;
			while (true)
			{
				if (!hasOpenBracket && firstStatementParsed)
					break;

				if (MatchSymbol(CLOSE_BRACKET))
					break;

				Token token;
				RequireToken(token);

				Statement* bodyStm = nullptr;

				if (m_KeywordParsers.Contains(token.GetIdentifier()))
					bodyStm = (*m_KeywordParsers[token.GetIdentifier()])(token);
				else
				{
					bodyStm = ParseVariableStatement(token, EndConditions::Semicolon);

					if (bodyStm == nullptr)
						bodyStm = ParseExpression(token, EndConditions::Semicolon);

					RequireSymbol(SEMICOLON, "expression");
				}

				if (bodyStm == nullptr)
					THROW_PROGRAM_PARSER_EXCEPTION("Unexpected token", token);

				StatementItemHolder->AddItem(bodyStm);

				firstStatementParsed = true;
			}

			return true;
		}

		Statement* Parser::ParseVariableStatement(Token& DeclarationToken, EndConditions ConditionMask)
		{
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

		Statement* Parser::ParseExpression(Token& DeclarationToken, EndConditions ConditionMask)
		{
			Statement* leftHand = ParseUnaryExpression(DeclarationToken, ConditionMask);

			if (leftHand == nullptr)
				return nullptr;

			return ParseBinaryExpression(0, leftHand, ConditionMask);
		}

		Statement* Parser::ParseUnaryExpression(Token& DeclarationToken, EndConditions ConditionMask)
		{
			Statement* stm = ParseUnaryExpressionPrefix(DeclarationToken, ConditionMask);

			if (MatchSymbol(INCREMENT))
			{

			}
			else if (MatchSymbol(DECREMENT))
			{

			}

			return stm;
		}

		Statement* Parser::ParseUnaryExpressionPrefix(Token& DeclarationToken, EndConditions ConditionMask)
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

			if (DeclarationToken.Matches(EXLAMATION, Token::SearchCases::CaseSensitive))
			{
				return ParseUnaryOperatorExpression(DeclarationToken, ConditionMask);
			}

			if (DeclarationToken.Matches(TILDE, Token::SearchCases::CaseSensitive))
			{
				return ParseUnaryOperatorExpression(DeclarationToken, ConditionMask);
			}

			if (DeclarationToken.Matches(MINES, Token::SearchCases::CaseSensitive))
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

		Statement* Parser::ParseUnaryOperatorExpression(Token& DeclarationToken, EndConditions ConditionMask)
		{
			Token token;
			RequireToken(token);

			if (IsEndCondition(token, ConditionMask))
				return nullptr;

			UnaryOperatorStatement::Operators op = GetUnaryOperator(DeclarationToken.GetIdentifier());

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

		Statement* Parser::ParseArrayExpression(Token& DeclarationToken, EndConditions ConditionMask)
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

		Statement* Parser::ParseConstantStatement(Token& DeclarationToken)
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

		Statement* Parser::ParseVariableAccessStatement(Token& DeclarationToken)
		{
			VariableAccessStatement* stm = Allocate<VariableAccessStatement>(m_Allocator);

			stm->SetName(DeclarationToken.GetIdentifier());

			Token token;
			RequireToken(token);

			return ParseMemberAccessStatement(token, stm);
		}

		Statement* Parser::ParseArrayElementAccessStatement(Token& DeclarationToken, Statement* ArrayStatement)
		{
			ArrayElementAccessStatement* stm = Allocate<ArrayElementAccessStatement>();

			stm->SetArrayStatement(ArrayStatement);
			stm->SetElementStatement(ParseExpression(DeclarationToken, EndConditions::SquareBracket));

			RequireSymbol(CLOSE_SQUARE_BRACKET, "array element access");

			return stm;
		}

		Statement* Parser::ParseMemberAccessStatement(Token& DeclarationToken, Statement* LeftStatement)
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

		Statement* Parser::ParseFunctionCallStatement(Token& DeclarationToken)
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

		bool Parser::IsEndCondition(Token& DeclarationToken, EndConditions ConditionMask)
		{
			bool isTheEnd =
				(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Semicolon) && DeclarationToken.Matches(SEMICOLON, Token::SearchCases::CaseSensitive)) ||
				(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Brace) && DeclarationToken.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive)) ||
				(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Comma) && DeclarationToken.Matches(COMMA, Token::SearchCases::CaseSensitive)) ||
				(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Bracket) && (DeclarationToken.Matches(OPEN_BRACKET, Token::SearchCases::CaseSensitive) || DeclarationToken.Matches(CLOSE_BRACKET, Token::SearchCases::CaseSensitive))) ||
				(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::SquareBracket) && (DeclarationToken.Matches(OPEN_SQUARE_BRACKET, Token::SearchCases::CaseSensitive) || DeclarationToken.Matches(CLOSE_SQUARE_BRACKET, Token::SearchCases::CaseSensitive)));

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