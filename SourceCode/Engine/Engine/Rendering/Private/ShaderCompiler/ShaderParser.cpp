// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderCompiler\ShaderParser.h>
#include <Rendering\Private\ShaderCompiler\FunctionType.h>
#include <Rendering\Private\ShaderCompiler\VariableType.h>
#include <Rendering\Private\ShaderCompiler\ParameterType.h>
#include <Rendering\Private\ShaderCompiler\IfStatement.h>
#include <Rendering\Private\ShaderCompiler\ElseStatement.h>
#include <Rendering\Private\ShaderCompiler\SwitchStatement.h>
#include <Rendering\Private\ShaderCompiler\CaseStatement.h>
#include <Rendering\Private\ShaderCompiler\ForStatement.h>
#include <Rendering\Private\ShaderCompiler\DoStatement.h>
#include <Rendering\Private\ShaderCompiler\WhileStatement.h>
#include <Rendering\Private\ShaderCompiler\ContinueStatement.h>
#include <Rendering\Private\ShaderCompiler\BreakStatement.h>
#include <Rendering\Private\ShaderCompiler\ReturnStatement.h>
#include <Rendering\Private\ShaderCompiler\DiscardStatement.h>
#include <Rendering\Private\ShaderCompiler\OperatorStatement.h>
#include <Rendering\Private\ShaderCompiler\VariableStatement.h>
#include <Rendering\Private\ShaderCompiler\FunctionCallStatement.h>
#include <Rendering\Private\ShaderCompiler\ConstantStatement.h>
#include <Rendering\Private\ShaderCompiler\MemberAccessStatement.h>
#include <Rendering\Private\ShaderCompiler\SemicolonStatement.h>
#include <Rendering\Private\Allocators.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Containers;
	using namespace Utility::Lexer;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
#define Deallocate(Address) DeallocateMemory(&Allocators::RenderingSystemAllocator, Address)

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
				const String FUNCTION_CALL = "__FUNCTION_CALL__";

				template<typename T>
				INLINE T *Allocate(void)
				{
					T *value = ReinterpretCast(T*, AllocateMemory(&Allocators::RenderingSystemAllocator, sizeof(T)));
					Construct(value);
					return value;
				}

				OperatorStatement::Operators GetOperator(const String &Symbol)
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

				int8 GetOperatorPrecedence(OperatorStatement::Operators Operator)
				{
					switch (Operator)
					{
					case OperatorStatement::Operators::Multipication:
					case OperatorStatement::Operators::Division:
						return 1;

					case OperatorStatement::Operators::Remainder:
						return 2;

					case OperatorStatement::Operators::Addition:
					case OperatorStatement::Operators::Subtraction:
						return 3;

					case OperatorStatement::Operators::Assignment:
					case OperatorStatement::Operators::AdditionAssignment:
					case OperatorStatement::Operators::SubtractionAssignment:
					case OperatorStatement::Operators::MultipicationAssignment:
					case OperatorStatement::Operators::DivisionAssignment:
						return 4;

					case OperatorStatement::Operators::EqualCheck:
					case OperatorStatement::Operators::NotEqualCheck:
					case OperatorStatement::Operators::LessCheck:
					case OperatorStatement::Operators::LessEqualCheck:;
					case OperatorStatement::Operators::GreaterCheck:
					case OperatorStatement::Operators::GreaterEqualCheck:
					case OperatorStatement::Operators::LogicalAnd:
					case OperatorStatement::Operators::LogicalOr:
						return 5;
					}

					return -1;
				}

				ShaderParser::ShaderParser(const String & Text) :
					Tokenizer(Text)
				{
					m_KwywordParsers[IF] = std::make_shared<KeywordParseFunction>([&](Token &Token) { return ParseIfStatement(Token); });
					m_KwywordParsers[ELSE] = std::make_shared<KeywordParseFunction>([&](Token &Token) { return ParseElseStatement(Token); });
					m_KwywordParsers[SWITCH] = std::make_shared<KeywordParseFunction>([&](Token &Token) { return ParseSwitchStatement(Token); });
					m_KwywordParsers[CASE] = std::make_shared<KeywordParseFunction>([&](Token &Token) { return ParseCaseStatement(Token); });
					m_KwywordParsers[DEFAULT] = m_KwywordParsers[CASE];
					m_KwywordParsers[FOR] = std::make_shared<KeywordParseFunction>([&](Token &Token) { return ParseForStatement(Token); });
					m_KwywordParsers[DO] = std::make_shared<KeywordParseFunction>([&](Token &Token) { return ParseDoStatement(Token); });
					m_KwywordParsers[WHILE] = std::make_shared<KeywordParseFunction>([&](Token &Token) { return ParseWhileStatement(Token); });
					m_KwywordParsers[CONTINUE] = std::make_shared<KeywordParseFunction>([&](Token &Token) { return ParseContinueStatement(Token); });
					m_KwywordParsers[BREAK] = std::make_shared<KeywordParseFunction>([&](Token &Token) { return ParseBreakStatement(Token); });
					m_KwywordParsers[RETURN] = std::make_shared<KeywordParseFunction>([&](Token &Token) { return ParseReturnStatement(Token); });
					m_KwywordParsers[DISCARD] = std::make_shared<KeywordParseFunction>([&](Token &Token) { return ParseDiscardStatement(Token); });
					m_KwywordParsers[SEMICOLON] = std::make_shared<KeywordParseFunction>([&](Token &Token) { return ParseSemicolonStatement(Token); });
				}

				void ShaderParser::Parse(VariableTypeList &Variables, FunctionTypeList &Functions)
				{
					Tokenizer::Parse();

					while (true)
					{
						Token token;
						if (!GetToken(token))
							return;

						ParseResults result = ParseResults::Failed;

						if ((result = ParseVariable(token, Variables)) == ParseResults::Approved)
							continue;
						else if (result == ParseResults::Failed)
							break;

						if ((result = ParseFunction(token, Functions)) == ParseResults::Approved)
							continue;
						else if (result == ParseResults::Failed)
							break;
					}
				}

				ShaderParser::ParseResults ShaderParser::ParseVariable(Token &DeclarationToken, VariableTypeList &Variables)
				{
					if (DeclarationToken.GetTokenType() != Token::Types::Identifier)
					{
						UngetToken(DeclarationToken);
						return ParseResults::Rejected;
					}

					VariableType *variableType = Allocate<VariableType>();

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

					DataTypes dataType = GetDataType((isConst ? dataTypeToken : DeclarationToken).GetIdentifier());
					if (dataType == DataTypes::Unknown)
					{
						result = ParseResults::Failed;
						goto FinishUp;
					}

					variableType->SetDataType(dataType);

					if (!GetToken(nameToken) || nameToken.GetTokenType() != Token::Types::Identifier)
					{
						result = ParseResults::Failed;
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
						Variables.Add(variableType);
					else
						Deallocate(variableType);

					return result;
				}

				ShaderParser::ParseResults ShaderParser::ParseFunction(Token &DeclarationToken, FunctionTypeList &Functions)
				{
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

					FunctionType *functionType = Allocate<FunctionType>();
					Functions.Add(functionType);

					DataTypes dataType = GetDataType(DeclarationToken.GetIdentifier());
					if (dataType == DataTypes::Unknown)
						return ParseResults::Failed;

					functionType->SetReturnDataType(dataType);

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

						ParameterType *parameterType = Allocate<ParameterType>();

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

				ShaderParser::ParseResults ShaderParser::ParseFunctionParameter(Token &DeclarationToken, ParameterType *Parameter)
				{
					DataTypes dataType = GetDataType(DeclarationToken.GetIdentifier());
					if (dataType == DataTypes::Unknown)
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

				Statement *ShaderParser::ParseIfStatement(Token &DeclarationToken)
				{
					Token token;
					if (!GetToken(token))
						return nullptr;

					if (!token.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
						return nullptr;

					Token conditionToken;
					if (!GetToken(conditionToken))
						return nullptr;

					Statement *conditionStm = ParseExpression(conditionToken);
					if (conditionStm == nullptr)
						return nullptr;

					IfStatement *stm = Allocate<IfStatement>();
					stm->SetCondition(conditionStm);

					ParseScopedStatements(stm);

					Token elseToken;
					if (!GetToken(elseToken))
						return nullptr;

					if (elseToken.Matches(ELSE, Token::SearchCases::CaseSensitive))
					{
						Statement *elseStm = ParseElseStatement(elseToken);

						if (elseStm == nullptr)
							return nullptr;

						stm->SetElse(elseStm);
					}
					else
						UngetToken(elseToken);

					return stm;
				}

				Statement * ShaderParser::ParseElseStatement(Token & DeclarationToken)
				{
					ElseStatement *stm = Allocate<ElseStatement>();

					if (ParseScopedStatements(stm) != ParseResults::Approved)
						return nullptr;

					return stm;
				}

				Statement *ShaderParser::ParseSwitchStatement(Token &DeclarationToken)
				{
					return nullptr;
				}

				Statement *ShaderParser::ParseCaseStatement(Token &DeclarationToken)
				{
					return nullptr;
				}

				Statement *ShaderParser::ParseForStatement(Token &DeclarationToken)
				{
					return nullptr;
				}

				Statement *ShaderParser::ParseDoStatement(Token &DeclarationToken)
				{
					return nullptr;
				}

				Statement *ShaderParser::ParseWhileStatement(Token &DeclarationToken)
				{
					return nullptr;
				}

				Statement *ShaderParser::ParseContinueStatement(Token &DeclarationToken)
				{
					return nullptr;
				}

				Statement *ShaderParser::ParseBreakStatement(Token &DeclarationToken)
				{
					return nullptr;
				}

				Statement *ShaderParser::ParseReturnStatement(Token &DeclarationToken)
				{
					ReturnStatement *stm = Allocate<ReturnStatement>();

					Token token;
					if (!GetToken(token))
						return nullptr;

					Statement *exprStm = ParseExpression(token);

					if (exprStm == nullptr)
						return nullptr;

					stm->SetStatement(exprStm);

					return stm;
				}

				Statement *ShaderParser::ParseDiscardStatement(Token &DeclarationToken)
				{
					return Allocate<DiscardStatement>();
				}

				Statement *ShaderParser::ParseSemicolonStatement(Token &DeclarationToken)
				{
					return Allocate<SemicolonStatement>();
				}

				ShaderParser::ParseResults ShaderParser::ParseScopedStatements(StatementsHolder *StatementHolder)
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

					bool firstStatementFinished = false;
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

						if (token.Matches(CLOSE_BRACKET, Token::SearchCases::CaseSensitive))
							break;

						if (!hasOpenBracket && firstStatementFinished)
						{
							UngetToken(token);
							break;
						}

						Statement *bodyStm = nullptr;

						if (m_KwywordParsers.Contains(token.GetIdentifier()))
							bodyStm = (*m_KwywordParsers[token.GetIdentifier()])(token);
						else
							bodyStm = ParseExpression(token);

						if (bodyStm == nullptr)
							return ParseResults::Failed;
						StatementHolder->AddStatement(bodyStm);

						firstStatementFinished = true;
					}

					return ParseResults::Approved;
				}

				Statement * ShaderParser::ParseExpression(Token & DeclarationToken)
				{
					int32 openBraceCount = 0;

					UngetToken(DeclarationToken);

					TokenStack operativeTokens;
					TokenStack outputTokens;

					while (true)
					{
						Token token;
						if (!GetToken(token))
							break;

						Token::Types tokenType = token.GetTokenType();

						if (token.Matches(SEMICOLON, Token::SearchCases::CaseSensitive))
						{
							UngetToken(token);
							break;
						}
						else if (token.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive) && openBraceCount-- == 0)
							break;

						if (tokenType == Token::Types::Constant)
							outputTokens.Push(token);
						else if (tokenType == Token::Types::Identifier || token.Matches(DOT, Token::SearchCases::CaseSensitive))
						{
							Token nextToken;
							if (!GetToken(nextToken))
								break;

							if (nextToken.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
							{
								++openBraceCount;

								Token functionCallSign;
								functionCallSign.SetIdentifier(FUNCTION_CALL);
								functionCallSign.SetType(Token::Types::Identifier);
								operativeTokens.Push(functionCallSign);

								operativeTokens.Push(token);
								operativeTokens.Push(nextToken);

								Token closeBrace;
								closeBrace.SetIdentifier(CLOSE_BRACE);
								closeBrace.SetType(Token::Types::Symbol);
								outputTokens.Push(closeBrace);
							}
							else
							{
								outputTokens.Push(token);

								UngetToken(nextToken);
							}
						}
						else if (token.Matches(COMMA, Token::SearchCases::CaseSensitive))
						{
							while (operativeTokens.GetSize() != 0)
							{
								Token &opToken = operativeTokens.Fetch();

								if (opToken.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
									break;

								outputTokens.Push(opToken);

								operativeTokens.Pop();
							}

							outputTokens.Push(token);
						}
						else if (token.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
						{
							++openBraceCount;

							operativeTokens.Push(token);
						}
						else if (token.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive))
						{
							while (operativeTokens.GetSize() != 0)
							{
								Token &opToken = operativeTokens.Fetch();

								outputTokens.Push(opToken);

								operativeTokens.Pop();

								if (opToken.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
									break;
							}
						}
						else
						{
							OperatorStatement::Operators op = GetOperator(token.GetIdentifier());

							int8 opPrecedence = GetOperatorPrecedence(op);

							while (operativeTokens.GetSize() != 0)
							{
								Token &opToken = operativeTokens.Fetch();

								if (opToken.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
									break;

								int8 tokenPrecedence = GetOperatorPrecedence(GetOperator(opToken.GetIdentifier()));

								if (opToken.GetTokenType() != Token::Types::Identifier &&
									tokenPrecedence > opPrecedence)
									break;

								outputTokens.Push(opToken);

								operativeTokens.Pop();
							}

							operativeTokens.Push(token);
						}
					}

					while (operativeTokens.GetSize() != 0)
					{
						Token opToken = operativeTokens.FetchAndPop();
						outputTokens.Push(opToken);
					}

					return ParseExpressionStack(outputTokens);
				}

				Statement *ShaderParser::ParseExpressionStack(TokenStack &Stack)
				{
					TokenStack valuesStack;

					while (Stack.GetSize() != 0)
					{
						Token token = Stack.FetchAndPop();
						Token::Types tokenType = token.GetTokenType();

						if (tokenType == Token::Types::Symbol)
							return ParseOperatorStatement(token, Stack);

						if (tokenType == Token::Types::Constant)
							return ParseConstantStatement(token, Stack);

						if (tokenType == Token::Types::Identifier)
						{
							if (Stack.GetSize() != 0)
							{
								Token nextToken = Stack.Fetch();

								if (token.Matches(FUNCTION_CALL, Token::SearchCases::CaseSensitive))
								{
									Stack.Pop();
									Stack.FetchAndPop();

									return ParseFunctionCallStatement(nextToken, Stack);
								}

								DataTypes dataType = GetDataType(nextToken.GetIdentifier());
								if (dataType != DataTypes::Unknown)
									return ParseVariableStatement(token, Stack);
							}

							return ParseMemberAccessStatement(token, Stack);
						}
					}

					return nullptr;
				}

				Statement * ShaderParser::ParseOperatorStatement(Token & DeclarationToken, TokenStack & Stack)
				{
					OperatorStatement *stm = Allocate<OperatorStatement>();
					stm->SetOperator(GetOperator(DeclarationToken.GetIdentifier()));
					stm->SetRight(ParseExpressionStack(Stack));
					stm->SetLeft(ParseExpressionStack(Stack));

					return stm;
				}

				Statement * ShaderParser::ParseConstantStatement(Token & DeclarationToken, TokenStack & Stack)
				{
					ConstantStatement *stm = Allocate<ConstantStatement>();

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

				Statement * ShaderParser::ParseMemberAccessStatement(Token &DeclarationToken, TokenStack &Stack)
				{
					MemberAccessStatement *stm = ReinterpretCast(MemberAccessStatement*, ReverseMemberAccessStatement(DeclarationToken, Stack));

					MemberAccessStatement *prevStm = nullptr;
					MemberAccessStatement *nextStm = nullptr;
					while (stm != nullptr)
					{
						nextStm = ReinterpretCast(MemberAccessStatement*, stm->GetMember());

						stm->SetMember(prevStm);

						prevStm = stm;
						stm = nextStm;
					}

					return prevStm;
				}

				Statement * ShaderParser::ParseFunctionCallStatement(Token &DeclarationToken, TokenStack &Stack)
				{
					FunctionCallStatement *stm = Allocate<FunctionCallStatement>();

					stm->SetFunctionName(DeclarationToken.GetIdentifier());

					while (true)
					{
						Token paramToken = Stack.Fetch();

						if (paramToken.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive))
						{
							Stack.Pop();
							break;
						}

						if (paramToken.Matches(COMMA, Token::SearchCases::CaseSensitive))
						{
							Stack.Pop();
							continue;
						}

						Statement *argStm = ParseExpressionStack(Stack);

						if (argStm == nullptr)
							return nullptr;

						stm->InsertArgumentStatement(0, argStm);
					}

					return stm;
				}

				Statement * ShaderParser::ParseVariableStatement(Token & DeclarationToken, TokenStack & Stack)
				{
					VariableStatement *stm = Allocate<VariableStatement>();
					stm->SetName(DeclarationToken.GetIdentifier());
					stm->SetDataType(GetDataType(Stack.FetchAndPop().GetIdentifier()));

					return stm;
				}

				Statement * ShaderParser::ReverseMemberAccessStatement(Token & DeclarationToken, TokenStack & Stack)
				{
					MemberAccessStatement *stm = Allocate<MemberAccessStatement>();
					stm->SetName(DeclarationToken.GetIdentifier());

					if (Stack.GetSize() == 0)
						return stm;

					if (Stack.Fetch().Matches(DOT, Token::SearchCases::CaseSensitive))
					{
						Stack.Pop();
						Token memberToken = Stack.FetchAndPop();

						Statement *memberStm = ReverseMemberAccessStatement(memberToken, Stack);
						if (memberStm == nullptr)
							return nullptr;

						stm->SetMember(memberStm);
					}

					return stm;
				}

				DataTypes ShaderParser::GetDataType(const String &Name)
				{
					static bool initialized = false;
					static Map<String, DataTypes> dataTypesName;

					if (!initialized)
					{
						initialized = true;

						dataTypesName["void"] = DataTypes::Void;
						dataTypesName["float"] = DataTypes::Float;
						dataTypesName["float2"] = DataTypes::Float2;
						dataTypesName["float3"] = DataTypes::Float3;
						dataTypesName["float4"] = DataTypes::Float4;
						dataTypesName["matrix4"] = DataTypes::Matrix4;
					}

					if (dataTypesName.Contains(Name))
						return dataTypesName[Name];

					return DataTypes::Unknown;
				}
			}
		}
	}
}