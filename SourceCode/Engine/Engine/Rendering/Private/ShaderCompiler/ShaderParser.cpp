// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderCompiler\ShaderParser.h>
#include <Rendering\Private\ShaderCompiler\Syntax\FunctionType.h>
#include <Rendering\Private\ShaderCompiler\Syntax\VariableType.h>
#include <Rendering\Private\ShaderCompiler\Syntax\ParameterType.h>
#include <Rendering\Private\ShaderCompiler\Syntax\IfStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\ElseStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\SwitchStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\CaseStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\ForStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\DoStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\WhileStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\ContinueStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\BreakStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\ReturnStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\DiscardStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\OperatorStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\UnaryOperatorStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\VariableStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\FunctionCallStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\ConstantStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\VariableAccessStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\ArrayElementAccessStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\MemberAccessStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\SemicolonStatement.h>
#include <Rendering\Private\ShaderCompiler\Syntax\ArrayStatement.h>
#include <Containers\StringUtility.h>
#include <Common\BitwiseUtils.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Utility::Lexer;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				using namespace Syntax;

				const String PREPROCESSOR_INCLUDE = "include";
				const String PREPROCESSOR_DEFINE = "define";
				const String PREPROCESSOR_UNDEF = "undef";
				const String PREPROCESSOR_IFDEF = "ifdef";
				const String PREPROCESSOR_IFNDEF = "ifndef";
				const String PREPROCESSOR_ELSE = "else";
				const String PREPROCESSOR_ENDIF = "endif";
				const String IF = STRINGIZE(if);
				const String ELSE = STRINGIZE(else);
				const String SWITCH = STRINGIZE(switch);
				const String CASE = STRINGIZE(case);
				const String DEFAULT = STRINGIZE(default);
				const String FOR = STRINGIZE(for);
				const String DO = STRINGIZE(do);
				const String WHILE = STRINGIZE(while);
				const String CONTINUE = STRINGIZE(continue);
				const String BREAK = STRINGIZE(break);
				const String RETURN = STRINGIZE(return);
				const String DISCARD = "discard";
				const String CONST = "const";
				const String INCREMENT("++");
				const String DECREMENT("--");

				bool IsDefined(const ShaderInfo::DefineList& Defines, const String& Define)
				{
					for each (auto & define in Defines)
					{
						if (define.Name == Define)
							return true;
					}

					return false;
				}

				OperatorStatement::Operators GetOperator(const String& Symbol)
				{
					static bool initialized = false;
					static Map<String, OperatorStatement::Operators> operators;

					if (!initialized)
					{
						initialized = true;

						operators["*"] = OperatorStatement::Operators::Multipication;
						operators["/"] = OperatorStatement::Operators::Division;
						operators["%"] = OperatorStatement::Operators::Remainder;
						operators["+"] = OperatorStatement::Operators::Addition;
						operators["-"] = OperatorStatement::Operators::Subtraction;
						operators["="] = OperatorStatement::Operators::Assignment;
						operators["+="] = OperatorStatement::Operators::AdditionAssignment;
						operators["-="] = OperatorStatement::Operators::SubtractionAssignment;
						operators["*="] = OperatorStatement::Operators::MultipicationAssignment;
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
					case OperatorStatement::Operators::MultipicationAssignment:
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

					case OperatorStatement::Operators::Multipication:
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

				ShaderParser::ShaderParser(AllocatorBase* Allocator, const String& Text) :
					Tokenizer(Text),
					m_Allocator(Allocator)
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
					m_KeywordParsers[SEMICOLON] = std::make_shared<KeywordParseFunction>([&](Token& Token) { return ParseSemicolonStatement(Token); });
				}

				void ShaderParser::Parse(Parameters& Parameters)
				{
					Tokenizer::Parse();

					Parse(Parameters, EndConditions::None);
				}

				void ShaderParser::Parse(Parameters& Parameters, EndConditions ConditionMask)
				{
					while (true)
					{
						Token token;
						if (!GetToken(token))
							return;

						if (IsEndCondition(token, ConditionMask))
						{
							UngetToken(token);
							return;
						}
						else if (token.Matches(SHARP, Token::SearchCases::CaseSensitive))
						{
							Token preprocessorCommandToken;
							if (!GetToken(preprocessorCommandToken))
								return;

							if (IsEndCondition(preprocessorCommandToken, ConditionMask))
							{
								UngetToken(token);
								return;
							}
							else
								UngetToken(preprocessorCommandToken);
						}

						ParseResults result = ParseResults::Failed;

						if ((result = ParsePreprocessor(token, Parameters)) == ParseResults::Approved)
							continue;
						else if (result == ParseResults::Failed)
							break;

						if ((result = ParseVariable(token, Parameters)) == ParseResults::Approved)
							continue;
						else if (result == ParseResults::Failed)
							break;

						if ((result = ParseFunction(token, Parameters)) == ParseResults::Approved)
							continue;
						else if (result == ParseResults::Failed)
							break;
					}
				}

				ShaderParser::ParseResults ShaderParser::ParsePreprocessor(Token& DeclarationToken, Parameters& Parameters)
				{
					if (!DeclarationToken.Matches(SHARP, Token::SearchCases::CaseSensitive))
						return ShaderParser::ParseResults::Rejected;

					Token preprocessorToken;
					if (!GetToken(preprocessorToken))
						return ShaderParser::ParseResults::Failed;

					if (preprocessorToken.Matches(PREPROCESSOR_INCLUDE, Token::SearchCases::CaseSensitive))
					{
						Token openBracketToken;
						if (!GetToken(openBracketToken) || !openBracketToken.Matches(OPEN_ANGLE_BRACKET, Token::SearchCases::CaseSensitive))
							return ShaderParser::ParseResults::Failed;

						String fileName;
						while (true)
						{
							Token token;
							if (!GetToken(token))
								return ShaderParser::ParseResults::Failed;

							if (token.Matches(CLOSE_ANGLE_BRACKET, Token::SearchCases::CaseSensitive))
								break;

							fileName += token.GetIdentifier();
						}

						String source;
						if (!Parameters.IncludeFunction(fileName, source))
							return ShaderParser::ParseResults::Failed;

						ShaderParser parser(m_Allocator, source);
						parser.Parse(Parameters);

						return ShaderParser::ParseResults::Approved;
					}
					else if (preprocessorToken.Matches(PREPROCESSOR_DEFINE, Token::SearchCases::CaseSensitive))
					{
						Token nameToken;
						if (!GetToken(nameToken))
							return ShaderParser::ParseResults::Failed;

						bool isDuplicate = false;
						for each (const auto & define in Parameters.Defines)
							if (define.Name == nameToken.GetIdentifier())
							{
								isDuplicate = true;
								break;
							}

						if (!isDuplicate)
							Parameters.Defines.Add({ nameToken.GetIdentifier() });

						return ShaderParser::ParseResults::Approved;
					}
					else if (preprocessorToken.Matches(PREPROCESSOR_UNDEF, Token::SearchCases::CaseSensitive))
					{
						Token nameToken;
						if (!GetToken(nameToken))
							return ShaderParser::ParseResults::Failed;

						for (uint32 i = 0; i < Parameters.Defines.GetSize(); ++i)
							if (Parameters.Defines[i].Name == nameToken.GetIdentifier())
								Parameters.Defines.RemoveAt(i--);

						return ShaderParser::ParseResults::Approved;
					}
					else if (preprocessorToken.Matches(PREPROCESSOR_IFDEF, Token::SearchCases::CaseSensitive) || preprocessorToken.Matches(PREPROCESSOR_IFNDEF, Token::SearchCases::CaseSensitive))
					{
						Token nameToken;
						if (!GetToken(nameToken))
							return ShaderParser::ParseResults::Failed;

						bool shouldRemoveBlock = (IsDefined(Parameters.Defines, nameToken.GetIdentifier()) == preprocessorToken.Matches(PREPROCESSOR_IFNDEF, Token::SearchCases::CaseSensitive));

						if (ParsePreprocessorBlock(Parameters, shouldRemoveBlock) == ShaderParser::ParseResults::Failed)
							return ShaderParser::ParseResults::Failed;

						Token sharpToken;
						if (!GetToken(sharpToken))
							return ShaderParser::ParseResults::Failed;

						if (!sharpToken.Matches(SHARP, Token::SearchCases::CaseSensitive))
							return ShaderParser::ParseResults::Failed;

						Token preprocessorToken;
						if (!GetToken(preprocessorToken))
							return ShaderParser::ParseResults::Failed;

						if (preprocessorToken.Matches(PREPROCESSOR_ELSE, Token::SearchCases::CaseSensitive))
							if (ParsePreprocessorBlock(Parameters, !shouldRemoveBlock) == ShaderParser::ParseResults::Failed)
								return ShaderParser::ParseResults::Failed;

						return ShaderParser::ParseResults::Approved;
					}

					return ShaderParser::ParseResults::Failed;
				}

				ShaderParser::ParseResults ShaderParser::ParseVariable(Token& DeclarationToken, Parameters& Parameters)
				{
					if (DeclarationToken.GetTokenType() != Token::Types::Identifier)
					{
						UngetToken(DeclarationToken);
						return ParseResults::Rejected;
					}

					VariableType* variableType = Allocate<VariableType>();

					bool isConst = DeclarationToken.Matches(CONST, Token::SearchCases::CaseSensitive);
					variableType->SetIsConstant(isConst);

					ParseResults result = ParseResults::Approved;

					Token nameToken;

					Token dataTypeToken;
					if (isConst && !GetToken(dataTypeToken))
					{
						result = ParseResults::Failed;
						goto FinishUp;
					}

					ShaderDataType::Types dataType = GetDataType((isConst ? dataTypeToken : DeclarationToken).GetIdentifier());
					if (dataType == ShaderDataType::Types::Unknown)
					{
						result = ParseResults::Failed;
						goto FinishUp;
					}

					variableType->SetDataType(dataType);

					if (!GetToken(nameToken) || nameToken.GetTokenType() != Token::Types::Identifier)
					{
						UngetToken(nameToken);
						result = ParseResults::Rejected;
						goto FinishUp;
					}

					variableType->SetName(nameToken.GetIdentifier());

					while (true)
					{
						Token token;
						if (!GetToken(token))
						{
							result = ParseResults::Failed;
							goto FinishUp;
						}

						if (token.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
						{
							UngetToken(token);
							UngetToken(nameToken);
							result = ParseResults::Rejected;
							goto FinishUp;
						}

						if (token.Matches(SEMICOLON, Token::SearchCases::CaseSensitive))
						{
							result = ParseResults::Approved;
							goto FinishUp;
						}

						if (token.Matches(COLON, Token::SearchCases::CaseSensitive))
						{
							Token registerToken;
							if (!GetToken(registerToken))
							{
								result = ParseResults::Failed;
								goto FinishUp;
							}

							variableType->SetRegister(registerToken.GetIdentifier());
						}
					}

				FinishUp:
					if (result == ParseResults::Approved)
						Parameters.Variables.Add(variableType);
					else
						Deallocate(variableType);

					return result;
				}

				ShaderParser::ParseResults ShaderParser::ParseFunction(Token& DeclarationToken, Parameters& Parameters)
				{
					ShaderDataType::Types type = GetDataType(DeclarationToken.GetIdentifier());
					if (type == ShaderDataType::Types::Unknown)
						return ParseResults::Failed;

					uint8 elementCount = 1;

					while (true)
					{
						Token token;
						if (!GetToken(token))
							return ParseResults::Failed;

						if (token.GetTokenType() == Token::Types::Identifier)
						{
							UngetToken(token);
							break;
						}

						if (token.Matches(OPEN_SQUARE_BRACKET, Token::SearchCases::CaseSensitive))
						{
							Token elementCountToken;
							if (!GetToken(elementCountToken))
								return ParseResults::Failed;

							elementCount = elementCountToken.GetConstantInt32();
						}
					}

					Token nameToken;
					if (!GetToken(nameToken))
						return ParseResults::Failed;

					if (nameToken.GetTokenType() != Token::Types::Identifier)
					{
						UngetToken(nameToken);
						return ParseResults::Rejected;
					}

					Token openBraceToken;
					if (!GetToken(openBraceToken))
						return ParseResults::Failed;

					if (!openBraceToken.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
					{
						UngetToken(nameToken);
						return ParseResults::Rejected;
					}

					FunctionType* functionType = Allocate<FunctionType>();
					Parameters.Functions.Add(functionType);

					functionType->SetReturnDataType({ type, elementCount });

					String name = nameToken.GetIdentifier();

					if (name.ToLower() == VERTEX_ENTRY_POINT_NAME)
					{
						name = VERTEX_ENTRY_POINT_NAME;

						functionType->SetType(FunctionType::Types::VertexMain);
					}
					else if (name.ToLower() == FRAGMENT_ENTRY_POINT_NAME)
					{
						name = FRAGMENT_ENTRY_POINT_NAME;

						functionType->SetType(FunctionType::Types::FragmentMain);
					}

					functionType->SetName(name);

					while (true)
					{
						Token parameterToken;
						if (!GetToken(parameterToken))
							return ParseResults::Failed;

						if (parameterToken.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive))
							break;

						ParameterType* parameterType = Allocate<ParameterType>();

						functionType->AddParamaeter(parameterType);

						if (ParseFunctionParameter(parameterToken, parameterType) == ParseResults::Failed)
							return ParseResults::Failed;
					}

					Token doubleColonToken;
					if (!GetToken(doubleColonToken))
						return ParseResults::Failed;
					if (doubleColonToken.Matches(COLON, Token::SearchCases::CaseSensitive))
					{
						Token registerToken;
						if (!GetToken(registerToken))
							return ParseResults::Failed;

						functionType->SetRegister(registerToken.GetIdentifier());
					}
					else
						UngetToken(doubleColonToken);

					return ParseScopedStatements(functionType);
				}

				ShaderParser::ParseResults ShaderParser::ParseFunctionParameter(Token& DeclarationToken, ParameterType* Parameter)
				{
					ShaderDataType::Types dataType = GetDataType(DeclarationToken.GetIdentifier());
					if (dataType == ShaderDataType::Types::Unknown)
						return ParseResults::Failed;

					Parameter->SetDataType(dataType);

					Token nameToken;
					if (!GetToken(nameToken) || nameToken.GetTokenType() != Token::Types::Identifier)
						return ParseResults::Failed;

					Parameter->SetName(nameToken.GetIdentifier());

					while (true)
					{
						Token token;
						if (!GetToken(token))
							return ParseResults::Failed;

						if (token.Matches(COMMA, Token::SearchCases::CaseSensitive))
							return ParseResults::Approved;

						if (token.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive))
						{
							UngetToken(token);
							return ParseResults::Approved;
						}

						if (token.Matches(COLON, Token::SearchCases::CaseSensitive))
						{
							Token registerToken;
							if (!GetToken(registerToken))
								return ParseResults::Failed;

							Parameter->SetRegister(registerToken.GetIdentifier());
						}
					}
				}

				ShaderParser::ParseResults ShaderParser::ParsePreprocessorBlock(Parameters& Parameters, bool ShouldRemove)
				{
					if (ShouldRemove)
					{
						int32 openBlockCount = 1;
						while (true)
						{
							Token token;
							if (!GetToken(token))
								return ShaderParser::ParseResults::Failed;

							if (token.Matches(PREPROCESSOR_IFDEF, Token::SearchCases::CaseSensitive) || token.Matches(PREPROCESSOR_IFNDEF, Token::SearchCases::CaseSensitive))
							{
								++openBlockCount;
								continue;
							}

							if (token.Matches(PREPROCESSOR_ELSE, Token::SearchCases::CaseSensitive))
							{
								if (--openBlockCount == 0)
								{
									UngetToken(token);

									break;
								}

								continue;
							}

							if (token.Matches(PREPROCESSOR_ENDIF, Token::SearchCases::CaseSensitive))
							{
								if (--openBlockCount == 0)
									break;

								continue;
							}
						}

						return ShaderParser::ParseResults::Approved;
					}

					Parse(Parameters, EndConditions::PreprocessorElse | EndConditions::PreprocessorEndIf);

					Token sharpToken;
					if (!GetToken(sharpToken))
						return ShaderParser::ParseResults::Failed;

					Token preprocessorToken;
					if (!GetToken(preprocessorToken))
						return ShaderParser::ParseResults::Failed;

					if (preprocessorToken.Matches(PREPROCESSOR_ELSE, Token::SearchCases::CaseSensitive))
					{
						UngetToken(sharpToken);
						return ShaderParser::ParseResults::Approved;
					}

					if (preprocessorToken.Matches(PREPROCESSOR_ENDIF, Token::SearchCases::CaseSensitive))
						return ShaderParser::ParseResults::Approved;

					return ShaderParser::ParseResults::Failed;
				}

				Statement* ShaderParser::ParseIfStatement(Token& DeclarationToken)
				{
					Token token;
					if (!GetToken(token))
						return nullptr;

					if (!token.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
						return nullptr;

					Statement* conditionStm = ParseExpression(token, EndConditions::Brace | EndConditions::Bracket);
					if (conditionStm == nullptr)
						return nullptr;

					IfStatement* stm = Allocate<IfStatement>();
					stm->SetCondition(conditionStm);

					ParseScopedStatements(stm);

					Token elseToken;
					if (!GetToken(elseToken))
						return nullptr;

					if (elseToken.Matches(ELSE, Token::SearchCases::CaseSensitive))
					{
						Statement* elseStm = ParseElseStatement(elseToken);

						if (elseStm == nullptr)
							return nullptr;

						stm->SetElse(elseStm);
					}
					else
						UngetToken(elseToken);

					return stm;
				}

				Statement* ShaderParser::ParseElseStatement(Token& DeclarationToken)
				{
					ElseStatement* stm = Allocate<ElseStatement>();

					if (ParseScopedStatements(stm) != ParseResults::Approved)
						return nullptr;

					return stm;
				}

				Statement* ShaderParser::ParseSwitchStatement(Token& DeclarationToken)
				{
					return nullptr;
				}

				Statement* ShaderParser::ParseCaseStatement(Token& DeclarationToken)
				{
					return nullptr;
				}

				Statement* ShaderParser::ParseForStatement(Token& DeclarationToken)
				{
					return nullptr;
				}

				Statement* ShaderParser::ParseDoStatement(Token& DeclarationToken)
				{
					return nullptr;
				}

				Statement* ShaderParser::ParseWhileStatement(Token& DeclarationToken)
				{
					return nullptr;
				}

				Statement* ShaderParser::ParseContinueStatement(Token& DeclarationToken)
				{
					return nullptr;
				}

				Statement* ShaderParser::ParseBreakStatement(Token& DeclarationToken)
				{
					return nullptr;
				}

				Statement* ShaderParser::ParseReturnStatement(Token& DeclarationToken)
				{
					ReturnStatement* stm = Allocate<ReturnStatement>();

					Token token;
					if (!GetToken(token))
						return nullptr;

					Statement* exprStm = ParseExpression(token, EndConditions::Semicolon);

					if (exprStm == nullptr)
						return nullptr;

					stm->SetStatement(exprStm);

					return stm;
				}

				Statement* ShaderParser::ParseDiscardStatement(Token& DeclarationToken)
				{
					return Allocate<DiscardStatement>();
				}

				Statement* ShaderParser::ParseSemicolonStatement(Token& DeclarationToken)
				{
					return Allocate<SemicolonStatement>();
				}

				ShaderParser::ParseResults ShaderParser::ParseScopedStatements(StatementsHolder* StatementHolder)
				{
					Token openBracketToken;
					if (!GetToken(openBracketToken))
						return ParseResults::Failed;

					bool hasOpenBracket = true;
					if (!openBracketToken.Matches(OPEN_BRACKET, Token::SearchCases::CaseSensitive))
					{
						hasOpenBracket = false;
						UngetToken(openBracketToken);
					}

					bool firstStatementParsed = false;
					while (true)
					{
						Token token;
						if (!GetToken(token))
							return ParseResults::Failed;

						if (token.Matches(SEMICOLON, Token::SearchCases::CaseSensitive))
						{
							StatementHolder->AddStatement(ParseSemicolonStatement(token));

							continue;
						}

						if (!hasOpenBracket && firstStatementParsed)
						{
							UngetToken(token);
							break;
						}

						if (token.Matches(CLOSE_BRACKET, Token::SearchCases::CaseSensitive))
							break;

						Statement* bodyStm = nullptr;

						if (m_KeywordParsers.Contains(token.GetIdentifier()))
							bodyStm = (*m_KeywordParsers[token.GetIdentifier()])(token);
						else
						{
							bodyStm = ParseVariableStatement(token, EndConditions::Semicolon);

							if (bodyStm == nullptr)
								bodyStm = ParseExpression(token, EndConditions::Semicolon);
						}

						if (bodyStm == nullptr)
							return ParseResults::Failed;
						StatementHolder->AddStatement(bodyStm);

						firstStatementParsed = true;
					}

					return ParseResults::Approved;
				}

				Statement* ShaderParser::ParseVariableStatement(Token& DeclarationToken, EndConditions ConditionMask)
				{
					ShaderDataType::Types dataType = GetDataType(DeclarationToken.GetIdentifier());

					if (dataType == ShaderDataType::Types::Unknown)
						return nullptr;

					Token nameToken;
					if (!GetToken(nameToken))
						return nullptr;

					if (nameToken.GetTokenType() != Token::Types::Identifier)
					{
						UngetToken(nameToken);
						return nullptr;
					}

					VariableStatement* stm = Allocate<VariableStatement>();
					stm->SetDataType(dataType);
					stm->SetName(nameToken.GetIdentifier());

					Token assignmentToken;
					if (!GetToken(assignmentToken))
						return nullptr;

					if (assignmentToken.Matches(EQUAL, Token::SearchCases::CaseSensitive))
					{
						Token initialToken;
						if (!GetToken(initialToken))
							return nullptr;

						Statement* initialStm = ParseExpression(initialToken, EndConditions::Semicolon);

						if (initialStm == nullptr)
							return nullptr;

						stm->SetInitialStatement(initialStm);
					}

					return stm;
				}

				Statement* ShaderParser::ParseExpression(Token& DeclarationToken, EndConditions ConditionMask)
				{
					Statement* leftHand = ParseUnaryExpression(DeclarationToken, ConditionMask);

					if (leftHand == nullptr)
						return nullptr;

					return ParseBinary(0, leftHand, ConditionMask);
				}

				Statement* ShaderParser::ParseUnaryExpression(Token& DeclarationToken, EndConditions ConditionMask)
				{
					Statement* stm = ParseUnaryExpressionPrefix(DeclarationToken, ConditionMask);

					if (stm == nullptr)
						return nullptr;

					Token token;
					if (!GetToken(token))
						return nullptr;

					if (token.Matches(INCREMENT, Token::SearchCases::CaseSensitive))
					{

					}
					else if (token.Matches(INCREMENT, Token::SearchCases::CaseSensitive))
					{

					}
					else
						UngetToken(token);

					return stm;
				}

				Statement* ShaderParser::ParseUnaryExpressionPrefix(Token& DeclarationToken, EndConditions ConditionMask)
				{
					if (DeclarationToken.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive) ||
						DeclarationToken.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
					{
						Token token;
						if (!GetToken(token))
							return nullptr;

						Statement* stm = ParseExpression(token, EndConditions::Brace);

						Token closeBraceToken;
						if (!GetToken(closeBraceToken))
							return nullptr;

						if (!closeBraceToken.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive))
							UngetToken(closeBraceToken);

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
					else if (DeclarationToken.GetTokenType() == Token::Types::Identifier)
					{
						Token openBraceToken;
						if (!GetToken(openBraceToken))
							return nullptr;

						bool isFunctionCall = openBraceToken.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive);

						UngetToken(openBraceToken);

						if (isFunctionCall)
							return ParseFunctionCallStatement(DeclarationToken);

						Statement* stm = ParseVariableAccessStatement(DeclarationToken);

						while (true)
						{
							Token opensquareBracketToken;
							if (!GetToken(opensquareBracketToken))
								return nullptr;

							if (opensquareBracketToken.Matches(OPEN_SQUARE_BRACKET, Token::SearchCases::CaseSensitive))
							{
								Token elementToekn;
								if (!GetToken(elementToekn))
									return nullptr;

								Statement* arrayAccessStm = ParseArrayElementAccessStatement(elementToekn, stm);

								if (arrayAccessStm != nullptr)
									stm = arrayAccessStm;
							}
							else
							{
								UngetToken(opensquareBracketToken);
								break;
							}
						}

						return stm;
					}
					else if (DeclarationToken.GetTokenType() == Token::Types::Constant)
					{
						return ParseConstantStatement(DeclarationToken);
					}

					return nullptr;
				}

				Statement* ShaderParser::ParseUnaryOperatorExpression(Token& DeclarationToken, EndConditions ConditionMask)
				{
					Token token;
					if (!GetToken(token))
						return nullptr;

					if (IsEndCondition(token, ConditionMask))
					{
						UngetToken(token);
						return nullptr;
					}

					UnaryOperatorStatement::Operators op = GetUnaryOperator(DeclarationToken.GetIdentifier());

					UnaryOperatorStatement* stm = Allocate<UnaryOperatorStatement>();
					stm->SetOperator(op);

					Statement* operandStm = ParseUnaryExpression(token, ConditionMask);

					if (operandStm == nullptr)
						return nullptr;

					stm->SetStatement(operandStm);

					return stm;
				}

				Statement* ShaderParser::ParseArrayExpression(Token& DeclarationToken, EndConditions ConditionMask)
				{
					ArrayStatement* stm = Allocate<ArrayStatement>();

					while (true)
					{
						Token token;
						if (!GetToken(token))
							return nullptr;

						if (token.Matches(CLOSE_BRACKET, Token::SearchCases::CaseSensitive))
							break;

						if (token.Matches(COMMA, Token::SearchCases::CaseSensitive))
							continue;

						Statement* elemStm = ParseExpression(token, EndConditions::Comma | EndConditions::Bracket);

						if (elemStm == nullptr)
							return nullptr;

						stm->AddElement(elemStm);
					}

					return stm;
				}

				Statement* ShaderParser::ParseBinary(int8 LeftHandPrecedence, Statement* LeftHandStatement, EndConditions ConditionMask)
				{
					while (true)
					{
						Token token;
						if (!GetToken(token))
							return nullptr;

						if (IsEndCondition(token, ConditionMask))
						{
							UngetToken(token);
							break;
						}

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
						if (!GetToken(rightHandToken))
							return nullptr;

						Statement* rightHandStm = ParseUnaryExpression(rightHandToken, ConditionMask);

						if (rightHandStm == nullptr)
							return nullptr;

						Token nextToken;
						if (!GetToken(nextToken))
							return nullptr;
						op = GetOperator(nextToken.GetIdentifier());
						int8 rightPrecedence = GetOperatorPrecedence(op);
						UngetToken(nextToken);

						if (precedence < rightPrecedence)
							if ((rightHandStm = ParseBinary(precedence + 1, rightHandStm, ConditionMask)) == nullptr)
								return nullptr;

						stm->SetRight(rightHandStm);
						LeftHandStatement = stm;
					}

					return LeftHandStatement;
				}

				Statement* ShaderParser::ParseConstantStatement(Token& DeclarationToken)
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

				Statement* ShaderParser::ParseVariableAccessStatement(Token& DeclarationToken)
				{
					VariableAccessStatement* stm = Allocate<VariableAccessStatement>();

					stm->SetName(DeclarationToken.GetIdentifier());

					Token token;
					if (!GetToken(token))
						return nullptr;

					return ParseMemberAccessStatement(token, stm);
				}

				Statement* ShaderParser::ParseArrayElementAccessStatement(Token& DeclarationToken, Statement* ArrayStatement)
				{
					ArrayElementAccessStatement* stm = Allocate<ArrayElementAccessStatement>();

					stm->SetArrayStatement(ArrayStatement);

					Statement* elemStm = ParseExpression(DeclarationToken, EndConditions::SquareBracket);
					if (elemStm == nullptr)
						return nullptr;

					stm->SetElementStatement(elemStm);

					Token endBracketToken;
					if (!GetToken(endBracketToken) || !endBracketToken.Matches(CLOSE_SQUARE_BRACKET, Token::SearchCases::CaseSensitive))
						return nullptr;

					return stm;
				}

				Statement* ShaderParser::ParseMemberAccessStatement(Token& DeclarationToken, Statement* LeftStatement)
				{
					if (DeclarationToken.Matches(DOT, Token::SearchCases::CaseSensitive))
					{
						MemberAccessStatement* stm = Allocate<MemberAccessStatement>();

						stm->SetLeft(LeftStatement);

						Token memberToken;
						if (!GetToken(memberToken))
							return nullptr;

						Statement* child = ParseVariableAccessStatement(memberToken);

						if (child == nullptr)
							return nullptr;

						stm->SetRight(child);

						return stm;
					}

					UngetToken(DeclarationToken);

					return LeftStatement;
				}

				Statement* ShaderParser::ParseFunctionCallStatement(Token& DeclarationToken)
				{
					FunctionCallStatement* stm = Allocate<FunctionCallStatement>();

					stm->SetFunctionName(DeclarationToken.GetIdentifier());

					Token braceToken;
					if (!GetToken(braceToken) || !braceToken.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
						return nullptr;

					while (true)
					{
						Token token;
						if (!GetToken(token))
							break;

						if (token.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive))
							break;

						if (token.Matches(COMMA, Token::SearchCases::CaseSensitive))
							continue;

						Statement* argStm = ParseExpression(token, EndConditions::Comma | EndConditions::Brace);

						if (argStm == nullptr)
							return nullptr;

						stm->AddArgument(argStm);
					}

					Token token;
					if (!GetToken(token))
						return nullptr;

					return ParseMemberAccessStatement(token, stm);
				}

				bool ShaderParser::IsEndCondition(Token Token, ShaderParser::EndConditions ConditionMask)
				{
					return
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::PreprocessorElse) && Token.Matches(PREPROCESSOR_ELSE, Token::SearchCases::CaseSensitive)) ||
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::PreprocessorEndIf) && Token.Matches(PREPROCESSOR_ENDIF, Token::SearchCases::CaseSensitive)) ||
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Semicolon) && Token.Matches(SEMICOLON, Token::SearchCases::CaseSensitive)) ||
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Brace) && Token.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive)) ||
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Comma) && Token.Matches(COMMA, Token::SearchCases::CaseSensitive)) ||
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Bracket) && (Token.Matches(OPEN_BRACKET, Token::SearchCases::CaseSensitive) || Token.Matches(CLOSE_BRACKET, Token::SearchCases::CaseSensitive))) ||
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::SquareBracket) && (Token.Matches(OPEN_SQUARE_BRACKET, Token::SearchCases::CaseSensitive) || Token.Matches(CLOSE_SQUARE_BRACKET, Token::SearchCases::CaseSensitive)));
				}

				ShaderDataType::Types ShaderParser::GetDataType(const String& Name)
				{
					static bool initialized = false;
					static Map<String, ShaderDataType::Types> dataTypesName;

					if (!initialized)
					{
						initialized = true;

						dataTypesName["void"] = ShaderDataType::Types::Void;
						dataTypesName["bool"] = ShaderDataType::Types::Bool;
						dataTypesName["float"] = ShaderDataType::Types::Float;
						dataTypesName["float2"] = ShaderDataType::Types::Float2;
						dataTypesName["float3"] = ShaderDataType::Types::Float3;
						dataTypesName["float4"] = ShaderDataType::Types::Float4;
						dataTypesName["matrix4"] = ShaderDataType::Types::Matrix4;
						dataTypesName["texture2D"] = ShaderDataType::Types::Texture2D;
					}

					if (dataTypesName.Contains(Name))
						return dataTypesName[Name];

					return ShaderDataType::Types::Unknown;
				}
			}
		}
	}
}