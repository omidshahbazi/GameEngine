// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ProgramCompiler\ProgramParser.h>
#include <Rendering\Private\ProgramCompiler\Syntax\StructType.h>
#include <Rendering\Private\ProgramCompiler\Syntax\FunctionType.h>
#include <Rendering\Private\ProgramCompiler\Syntax\ParameterType.h>
#include <Rendering\Private\ProgramCompiler\Syntax\IfStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\ElseStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\SwitchStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\CaseStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\ForStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\DoStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\WhileStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\ContinueStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\BreakStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\ReturnStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\DiscardStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\OperatorStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\UnaryOperatorStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\VariableStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\FunctionCallStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\ConstantStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\VariableAccessStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\ArrayElementAccessStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\MemberAccessStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\SemicolonStatement.h>
#include <Rendering\Private\ProgramCompiler\Syntax\ArrayStatement.h>
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
			namespace ProgramCompiler
			{
				//LOTODO: Need to have a documentation about shading language
				//This is an standard program
				//"struct INPUT_DATA { float3 pos : POSITION; float3 col : UV; };"
				//	"struct DATA { matrix4 _MVP;  matrix4 _View; float time; };"
				//	"DATA data;"
				//	"float4 VertexMain(INPUT_DATA InputData)"
				//	"{"
				//	"	return data._MVP * data._View * float4(InputData.pos, 1);"
				//	"}"
				//	"float4 FragmentMain(INPUT_DATA InputData)"
				//	"{"
				//	"	return float4(InputData.col, data.time);"
				//	"}";

				using namespace Syntax;

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
				//cstr CONST = "const";
				cstr INCREMENT("++");
				cstr DECREMENT("--");

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

				ProgramParser::ProgramParser(AllocatorBase* Allocator, const String& Text, ErrorFunction OnError) :
					Tokenizer(Text, OnError),
					m_Allocator(Allocator),
					m_Parameters(nullptr),
					m_Structs(m_Allocator),
					m_Variables(m_Allocator)
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

				bool ProgramParser::Parse(Parameters& Parameters)
				{
					Tokenizer::Parse();

					return Parse(Parameters, EndConditions::None);
				}

				bool ProgramParser::Parse(Parameters& Parameters, EndConditions ConditionMask)
				{
					m_Parameters = &Parameters;

					while (true)
					{
						Token token;
						if (!GetToken(token))
							break;

						if (IsEndCondition(token, ConditionMask))
						{
							UngetToken(token);
							return true;
						}
						else if (token.Matches(SHARP, Token::SearchCases::CaseSensitive))
						{
							Token preprocessorCommandToken;
							if (!GetToken(preprocessorCommandToken))
								return true;

							if (IsEndCondition(preprocessorCommandToken, ConditionMask))
							{
								UngetToken(token);
								return true;
							}
							else
								UngetToken(preprocessorCommandToken);
						}

						ParseResults result = ParseResults::Failed;

						if ((result = ParseStruct(token)) == ParseResults::Approved)
							continue;
						else if (result == ParseResults::Failed)
							return false;

						if ((result = ParseVariable(token)) == ParseResults::Approved)
							continue;
						else if (result == ParseResults::Failed)
							return false;

						if ((result = ParseFunction(token)) == ParseResults::Approved)
							continue;
						else if (result == ParseResults::Failed)
							return false;
					}

					Assert(m_Structs.GetSize() == 0, "Structs didn't get evacuated from stack");

					m_Parameters = nullptr;

					return true;
				}

				ProgramParser::ParseResults ProgramParser::ParseStruct(Token& DeclarationToken)
				{
					if (DeclarationToken.GetTokenType() != Token::Types::Identifier)
					{
						UngetToken(DeclarationToken);
						return ParseResults::Rejected;
					}

					if (!DeclarationToken.Matches(STRUCT, Token::SearchCases::CaseSensitive))
						return ParseResults::Rejected;

					ParseResults result = ParseResults::Approved;

					StructType* structType = Allocate<StructType>(m_Allocator);

					Token nameToken;
					if (!RequireToken(nameToken))
					{
						result = ParseResults::Failed;
						goto FinishUp;
					}

					structType->SetName(nameToken.GetIdentifier());

					if (!RequireSymbol(OPEN_BRACKET, "struct"))
					{
						result = ParseResults::Failed;
						goto FinishUp;
					}

					m_Structs.Push(structType);

					while (true)
					{
						Token variableToken;
						if (!RequireToken(variableToken))
							return ParseResults::Failed;

						if (variableToken.Matches(CLOSE_BRACKET, Token::SearchCases::CaseSensitive))
							break;

						if (ParseVariable(variableToken) == ParseResults::Failed)
						{
							result = ParseResults::Failed;
							goto FinishUp;
						}
					}

					m_Structs.Pop();

					if (!RequireSymbol(SEMICOLON, "struct"))
					{
						result = ParseResults::Failed;
						goto FinishUp;
					}

				FinishUp:
					if (result == ParseResults::Approved)
						m_Parameters->Structs.Add(structType);
					else
						Deallocate(structType);

					return result;
				}

				ProgramParser::ParseResults ProgramParser::ParseVariable(Token& DeclarationToken)
				{
					if (DeclarationToken.GetTokenType() != Token::Types::Identifier)
					{
						UngetToken(DeclarationToken);
						return ParseResults::Rejected;
					}

					VariableType* variableType = Allocate<VariableType>();

					ParseResults result = ParseResults::Approved;

					Token nameToken;

					DataType dataType = GetDataType(DeclarationToken.GetIdentifier());
					if (dataType.IsUnrecognized())
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
						if (!RequireToken(token))
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
							if (!RequireToken(registerToken))
							{
								result = ParseResults::Failed;
								goto FinishUp;
							}

							variableType->SetRegister(registerToken.GetIdentifier());
						}
					}

				FinishUp:
					if (result == ParseResults::Approved)
					{
						if (m_Structs.GetSize() == 0)
						{
							if (variableType->GetDataType().IsBuiltIn())
								return ParseResults::Failed;

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

				ProgramParser::ParseResults ProgramParser::ParseFunction(Token& DeclarationToken)
				{
					m_Variables.Clear();

					DataType dataType = GetDataType(DeclarationToken.GetIdentifier());
					if (dataType.IsUnrecognized())
						return ParseResults::Failed;

					uint8 elementCount = 1;

					while (true)
					{
						Token token;
						if (!RequireToken(token))
							return ParseResults::Failed;

						if (token.GetTokenType() == Token::Types::Identifier)
						{
							UngetToken(token);
							break;
						}

						if (token.Matches(OPEN_SQUARE_BRACKET, Token::SearchCases::CaseSensitive))
						{
							Token elementCountToken;
							if (!RequireToken(elementCountToken))
								return ParseResults::Failed;

							elementCount = elementCountToken.GetConstantInt32();
						}
					}

					Token nameToken;
					if (!RequireToken(nameToken))
						return ParseResults::Failed;

					if (nameToken.GetTokenType() != Token::Types::Identifier)
					{
						UngetToken(nameToken);
						return ParseResults::Rejected;
					}

					Token openBraceToken;
					if (!RequireToken(openBraceToken))
						return ParseResults::Failed;

					if (!openBraceToken.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
					{
						UngetToken(nameToken);
						return ParseResults::Rejected;
					}

					FunctionType* functionType = Allocate<FunctionType>(m_Allocator);
					m_Parameters->Functions.Add(functionType);

					functionType->SetReturnDataType({ dataType.GetType(), elementCount });

					const String& name = nameToken.GetIdentifier();

					if (name.ToLower() == VERTEX_ENTRY_POINT_NAME)
						functionType->SetType(FunctionType::Types::VertexMain);
					else if (name.ToLower() == FRAGMENT_ENTRY_POINT_NAME)
						functionType->SetType(FunctionType::Types::FragmentMain);

					functionType->SetName(name);

					while (true)
					{
						Token parameterToken;
						if (!RequireToken(parameterToken))
							return ParseResults::Failed;

						if (parameterToken.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive))
							break;

						ParameterType* parameterType = Allocate<ParameterType>();

						functionType->AddParamaeter(parameterType);

						if (ParseFunctionParameter(parameterToken, parameterType) == ParseResults::Failed)
						{
							Deallocate(functionType);

							return ParseResults::Failed;
						}
					}

					Token colonToken;
					if (!RequireToken(colonToken))
					{
						Deallocate(functionType);

						return ParseResults::Failed;
					}

					if (colonToken.Matches(COLON, Token::SearchCases::CaseSensitive))
					{
						Token registerToken;
						if (!RequireToken(registerToken))
						{
							Deallocate(functionType);

							return ParseResults::Failed;
						}

						functionType->SetRegister(registerToken.GetIdentifier());
					}
					else
						UngetToken(colonToken);

					return ParseScopedStatements(functionType);
				}

				ProgramParser::ParseResults ProgramParser::ParseFunctionParameter(Token& DeclarationToken, ParameterType* Parameter)
				{
					DataType dataType = GetDataType(DeclarationToken.GetIdentifier());
					if (dataType.IsUnrecognized())
						return ParseResults::Failed;

					Parameter->SetDataType(dataType);

					Token nameToken;
					if (!RequireToken(nameToken) || nameToken.GetTokenType() != Token::Types::Identifier)
						return ParseResults::Failed;

					Parameter->SetName(nameToken.GetIdentifier());

					m_Variables[Parameter->GetName()] = dataType;

					while (true)
					{
						Token token;
						if (!RequireToken(token))
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
							if (!RequireToken(registerToken))
								return ParseResults::Failed;

							Parameter->SetRegister(registerToken.GetIdentifier());
						}
					}
				}

				Statement* ProgramParser::ParseIfStatement(Token& DeclarationToken)
				{
					Token token;
					if (!RequireToken(token))
						return nullptr;

					if (!token.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
						return nullptr;

					Statement* conditionStm = ParseExpression(token, EndConditions::Brace | EndConditions::Bracket);
					if (conditionStm == nullptr)
						return nullptr;

					IfStatement* stm = Allocate<IfStatement>(m_Allocator);
					stm->SetCondition(conditionStm);

					ParseScopedStatements(stm);

					Token elseToken;
					if (!RequireToken(elseToken))
					{
						Deallocate(stm);

						return nullptr;
					}

					if (elseToken.Matches(ELSE, Token::SearchCases::CaseSensitive))
					{
						Statement* elseStm = ParseElseStatement(elseToken);

						if (elseStm == nullptr)
						{
							Deallocate(stm);

							return nullptr;
						}

						stm->SetElse(elseStm);
					}
					else
						UngetToken(elseToken);

					return stm;
				}

				Statement* ProgramParser::ParseElseStatement(Token& DeclarationToken)
				{
					ElseStatement* stm = Allocate<ElseStatement>(m_Allocator);

					if (ParseScopedStatements(stm) != ParseResults::Approved)
					{
						Deallocate(stm);

						return nullptr;
					}

					return stm;
				}

				Statement* ProgramParser::ParseSwitchStatement(Token& DeclarationToken)
				{
					return nullptr;
				}

				Statement* ProgramParser::ParseCaseStatement(Token& DeclarationToken)
				{
					return nullptr;
				}

				Statement* ProgramParser::ParseForStatement(Token& DeclarationToken)
				{
					return nullptr;
				}

				Statement* ProgramParser::ParseDoStatement(Token& DeclarationToken)
				{
					return nullptr;
				}

				Statement* ProgramParser::ParseWhileStatement(Token& DeclarationToken)
				{
					return nullptr;
				}

				Statement* ProgramParser::ParseContinueStatement(Token& DeclarationToken)
				{
					return nullptr;
				}

				Statement* ProgramParser::ParseBreakStatement(Token& DeclarationToken)
				{
					return nullptr;
				}

				Statement* ProgramParser::ParseReturnStatement(Token& DeclarationToken)
				{
					ReturnStatement* stm = Allocate<ReturnStatement>();

					Token token;
					if (!RequireToken(token))
					{
						Deallocate(stm);

						return nullptr;
					}

					Statement* exprStm = ParseExpression(token, EndConditions::Semicolon);

					if (exprStm == nullptr)
					{
						Deallocate(stm);

						return nullptr;
					}

					stm->SetStatement(exprStm);

					return stm;
				}

				Statement* ProgramParser::ParseDiscardStatement(Token& DeclarationToken)
				{
					return Allocate<DiscardStatement>();
				}

				Statement* ProgramParser::ParseSemicolonStatement(Token& DeclarationToken)
				{
					return Allocate<SemicolonStatement>();
				}

				ProgramParser::ParseResults ProgramParser::ParseScopedStatements(StatementItemHolder* StatementItemHolder)
				{
					Token openBracketToken;
					if (!RequireToken(openBracketToken))
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
						if (!RequireToken(token))
							return ParseResults::Failed;

						if (token.Matches(SEMICOLON, Token::SearchCases::CaseSensitive))
						{
							StatementItemHolder->AddItem(ParseSemicolonStatement(token));

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
						{
							RaisError("Unexpected token");

							return ParseResults::Failed;
						}

						StatementItemHolder->AddItem(bodyStm);

						firstStatementParsed = true;
					}

					return ParseResults::Approved;
				}

				Statement* ProgramParser::ParseVariableStatement(Token& DeclarationToken, EndConditions ConditionMask)
				{
					DataType dataType = GetDataType(DeclarationToken.GetIdentifier());

					if (dataType.IsUnrecognized())
						return nullptr;

					Token nameToken;
					if (!RequireToken(nameToken))
						return nullptr;

					if (nameToken.GetTokenType() != Token::Types::Identifier)
					{
						UngetToken(nameToken);
						return nullptr;
					}

					VariableStatement* stm = Allocate<VariableStatement>();
					stm->SetDataType(dataType);
					stm->SetName(nameToken.GetIdentifier());

					m_Variables[stm->GetName()] = dataType;

					Token assignmentToken;
					if (!RequireToken(assignmentToken))
					{
						Deallocate(stm);

						return nullptr;
					}

					if (assignmentToken.Matches(EQUAL, Token::SearchCases::CaseSensitive))
					{
						Token initialToken;
						if (!RequireToken(initialToken))
						{
							Deallocate(stm);

							return nullptr;
						}

						Statement* initialStm = ParseExpression(initialToken, EndConditions::Semicolon);

						if (initialStm == nullptr)
						{
							Deallocate(stm);

							return nullptr;
						}

						stm->SetInitialStatement(initialStm);
					}

					return stm;
				}

				Statement* ProgramParser::ParseExpression(Token& DeclarationToken, EndConditions ConditionMask)
				{
					Statement* leftHand = ParseUnaryExpression(DeclarationToken, ConditionMask);

					if (leftHand == nullptr)
						return nullptr;

					return ParseBinary(0, leftHand, ConditionMask);
				}

				Statement* ProgramParser::ParseUnaryExpression(Token& DeclarationToken, EndConditions ConditionMask)
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

				Statement* ProgramParser::ParseUnaryExpressionPrefix(Token& DeclarationToken, EndConditions ConditionMask)
				{
					if (DeclarationToken.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive) ||
						DeclarationToken.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
					{
						Token token;
						if (!RequireToken(token))
							return nullptr;

						Statement* stm = ParseExpression(token, EndConditions::Brace);

						Token closeBraceToken;
						if (!RequireToken(closeBraceToken))
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
						if (!RequireToken(openBraceToken))
							return nullptr;

						bool isFunctionCall = openBraceToken.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive);

						UngetToken(openBraceToken);

						if (isFunctionCall)
							return ParseFunctionCallStatement(DeclarationToken);

						Statement* stm = ParseVariableAccessStatement(DeclarationToken);

						while (true)
						{
							Token opensquareBracketToken;
							if (!RequireToken(opensquareBracketToken))
								return nullptr;

							if (opensquareBracketToken.Matches(OPEN_SQUARE_BRACKET, Token::SearchCases::CaseSensitive))
							{
								Token elementToekn;
								if (!RequireToken(elementToekn))
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

				Statement* ProgramParser::ParseUnaryOperatorExpression(Token& DeclarationToken, EndConditions ConditionMask)
				{
					Token token;
					if (!RequireToken(token))
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
					{
						Deallocate(stm);

						return nullptr;
					}

					stm->SetStatement(operandStm);

					return stm;
				}

				Statement* ProgramParser::ParseArrayExpression(Token& DeclarationToken, EndConditions ConditionMask)
				{
					ArrayStatement* stm = Allocate<ArrayStatement>(m_Allocator);

					while (true)
					{
						Token token;
						if (!RequireToken(token))
						{
							Deallocate(stm);

							return nullptr;
						}

						if (token.Matches(CLOSE_BRACKET, Token::SearchCases::CaseSensitive))
							break;

						if (token.Matches(COMMA, Token::SearchCases::CaseSensitive))
							continue;

						Statement* elemStm = ParseExpression(token, EndConditions::Comma | EndConditions::Bracket);

						if (elemStm == nullptr)
						{
							Deallocate(stm);

							return nullptr;
						}

						stm->AddElement(elemStm);
					}

					return stm;
				}

				Statement* ProgramParser::ParseBinary(int8 LeftHandPrecedence, Statement* LeftHandStatement, EndConditions ConditionMask)
				{
					while (true)
					{
						Token token;
						if (!RequireToken(token))
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
						if (!RequireToken(rightHandToken))
						{
							Deallocate(stm);

							return nullptr;
						}

						Statement* rightHandStm = ParseUnaryExpression(rightHandToken, ConditionMask);

						if (rightHandStm == nullptr)
						{
							Deallocate(stm);

							return nullptr;
						}

						Token nextToken;
						if (!RequireToken(nextToken))
						{
							Deallocate(stm);

							return nullptr;
						}

						op = GetOperator(nextToken.GetIdentifier());
						int8 rightPrecedence = GetOperatorPrecedence(op);
						UngetToken(nextToken);

						if (precedence < rightPrecedence)
							if ((rightHandStm = ParseBinary(precedence + 1, rightHandStm, ConditionMask)) == nullptr)
							{
								Deallocate(stm);

								return nullptr;
							}

						stm->SetRight(rightHandStm);
						LeftHandStatement = stm;
					}

					return LeftHandStatement;
				}

				Statement* ProgramParser::ParseConstantStatement(Token& DeclarationToken)
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

				Statement* ProgramParser::ParseVariableAccessStatement(Token& DeclarationToken)
				{
					VariableAccessStatement* stm = Allocate<VariableAccessStatement>();

					stm->SetName(DeclarationToken.GetIdentifier());
					stm->SetVariableType(FindVariableType(stm->GetName()).GetType());

					Token token;
					if (!RequireToken(token))
					{
						Deallocate(stm);

						return nullptr;
					}

					return ParseMemberAccessStatement(token, stm);
				}

				Statement* ProgramParser::ParseArrayElementAccessStatement(Token& DeclarationToken, Statement* ArrayStatement)
				{
					ArrayElementAccessStatement* stm = Allocate<ArrayElementAccessStatement>();

					stm->SetArrayStatement(ArrayStatement);

					Statement* elemStm = ParseExpression(DeclarationToken, EndConditions::SquareBracket);
					if (elemStm == nullptr)
					{
						Deallocate(stm);

						return nullptr;
					}

					stm->SetElementStatement(elemStm);

					Token endBracketToken;
					if (!RequireToken(endBracketToken) || !endBracketToken.Matches(CLOSE_SQUARE_BRACKET, Token::SearchCases::CaseSensitive))
					{
						Deallocate(stm);

						return nullptr;
					}

					return stm;
				}

				Statement* ProgramParser::ParseMemberAccessStatement(Token& DeclarationToken, Statement* LeftStatement)
				{
					if (DeclarationToken.Matches(DOT, Token::SearchCases::CaseSensitive))
					{
						MemberAccessStatement* stm = Allocate<MemberAccessStatement>();

						stm->SetLeft(LeftStatement);

						Token memberToken;
						if (!RequireToken(memberToken))
						{
							Deallocate(stm);

							return nullptr;
						}

						Statement* child = ParseVariableAccessStatement(memberToken);

						if (child == nullptr)
						{
							Deallocate(stm);

							return nullptr;
						}

						stm->SetRight(child);

						return stm;
					}

					UngetToken(DeclarationToken);

					return LeftStatement;
				}

				Statement* ProgramParser::ParseFunctionCallStatement(Token& DeclarationToken)
				{
					FunctionCallStatement* stm = Allocate<FunctionCallStatement>(m_Allocator);

					stm->SetFunctionName(DeclarationToken.GetIdentifier());

					if (!RequireSymbol(OPEN_BRACE, "function call"))
					{
						Deallocate(stm);

						return nullptr;
					}

					while (true)
					{
						Token token;
						if (!RequireToken(token))
							break;

						if (token.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive))
							break;

						if (token.Matches(COMMA, Token::SearchCases::CaseSensitive))
							continue;

						Statement* argStm = ParseExpression(token, EndConditions::Comma | EndConditions::Brace);

						if (argStm == nullptr)
						{
							Deallocate(stm);

							return nullptr;
						}

						stm->AddArgument(argStm);
					}

					Token token;
					if (!RequireToken(token))
					{
						Deallocate(stm);

						return nullptr;
					}

					return ParseMemberAccessStatement(token, stm);
				}

				bool ProgramParser::IsEndCondition(Token Token, EndConditions ConditionMask)
				{
					return
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Semicolon) && Token.Matches(SEMICOLON, Token::SearchCases::CaseSensitive)) ||
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Brace) && Token.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive)) ||
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Comma) && Token.Matches(COMMA, Token::SearchCases::CaseSensitive)) ||
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Bracket) && (Token.Matches(OPEN_BRACKET, Token::SearchCases::CaseSensitive) || Token.Matches(CLOSE_BRACKET, Token::SearchCases::CaseSensitive))) ||
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::SquareBracket) && (Token.Matches(OPEN_SQUARE_BRACKET, Token::SearchCases::CaseSensitive) || Token.Matches(CLOSE_SQUARE_BRACKET, Token::SearchCases::CaseSensitive)));
				}

				DataType ProgramParser::FindVariableType(const String& Name) const
				{
					for (auto& structType : m_Parameters->Structs)
					{
						auto& variables = structType->GetItems();

						int32 index = variables.Find([&Name](const VariableType* Item) { return (Item->GetName() == Name); });
						if (index != -1)
							return variables[index]->GetDataType();
					}

					if (!m_Variables.Contains(Name))
						return {};

					return m_Variables[Name];
				}

				DataType ProgramParser::GetDataType(const String& Name)
				{
					ProgramDataTypes primitiveType = GetPrimitiveDataType(Name);
					if (primitiveType != ProgramDataTypes::Unknown)
						return primitiveType;

					for (auto& structType : m_Parameters->Structs)
					{
						if (structType->GetName() == Name)
							return Name;
					}

					return {};
				}

				ProgramDataTypes ProgramParser::GetPrimitiveDataType(const String& Name)
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
	}
}