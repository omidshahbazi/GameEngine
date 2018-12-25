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
				const String FUNCTION_CALL_BEGIN_PARAMETERS = "__FUNCTION_CALL__";
				const String INCREMENT("++");
				const String DECREMENT("--");

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

					Statement *conditionStm = ParseExpression(conditionToken, EndConditions::Brace);
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

					Statement *exprStm = ParseExpression(token, EndConditions::Semicolon);

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
							bodyStm = ParseExpression(token, EndConditions::Brace);

						if (bodyStm == nullptr)
							return ParseResults::Failed;
						StatementHolder->AddStatement(bodyStm);

						firstStatementFinished = true;
					}

					return ParseResults::Approved;
				}

				Statement * ShaderParser::ParseExpression(Token & DeclarationToken, EndConditions ConditionMask)
				{
					Statement *leftHand = ParseUnaryExpression(DeclarationToken, ConditionMask);

					if (leftHand == nullptr)
						return nullptr;

					return ParseBinary(0, leftHand, ConditionMask);
				}

				Statement * ShaderParser::ParseUnaryExpression(Token & DeclarationToken, EndConditions ConditionMask)
				{
					Statement *stm = ParseUnaryExpressionPrefix(DeclarationToken, ConditionMask);

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

				Statement * ShaderParser::ParseUnaryExpressionPrefix(Token & DeclarationToken, EndConditions ConditionMask)
				{
					if (DeclarationToken.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive))
					{
						Token token;
						if (!GetToken(token))
							return nullptr;

						return ParseExpression(token, EndConditions::Brace);
					}
					if (DeclarationToken.Matches(EXLAMATION, Token::SearchCases::CaseSensitive))
					{
						Token token;
						if (!GetToken(token))
							return nullptr;

						return ParseUnaryLogicalNotExpression(token, ConditionMask);
					}
					if (DeclarationToken.Matches(TILDE, Token::SearchCases::CaseSensitive))
					{
						Token token;
						if (!GetToken(token))
							return nullptr;

						return ParseUnaryBitwiseNotExpression(token, ConditionMask);
					}
					//if (DeclarationToken.Matches(PLUS, Token::SearchCases::CaseSensitive))
					//{
					//	Token token;
					//	if (!GetToken(token))
					//		return nullptr;

					//	return GC_NEW(astExpression) astUnaryPlusExpression(parseUnary(condition));
					//}
					//if (DeclarationToken.Matches(MINES, Token::SearchCases::CaseSensitive))
					//{
					//	if (!next()) return 0; // skip '-'
					//	return GC_NEW(astExpression) astUnaryMinusExpression(parseUnary(condition));
					//}
					//if (DeclarationToken.Matches(INCREMENT, Token::SearchCases::CaseSensitive))
					//{
					//	if (!next()) return 0; // skip '++'
					//	return GC_NEW(astExpression) astPrefixIncrementExpression(parseUnary(condition));
					//}
					//if (DeclarationToken.Matches(DECREMENT, Token::SearchCases::CaseSensitive))
					//{
					//	if (!next()) return 0; // skip '--'
					//	return GC_NEW(astExpression) astPrefixDecrementExpression(parseUnary(condition));
					//}
					else if (DeclarationToken.GetTokenType() == Token::Types::Identifier)
					{
						Token openBraceToken;
						if (!GetToken(openBraceToken))
							return nullptr;

						bool isFunctionCall = openBraceToken.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive);

						UngetToken(openBraceToken);

						if (isFunctionCall)
							return ParseFunctionCallStatement(DeclarationToken);

						return ParseMemberAccessStatement(DeclarationToken);
					}
					else if (DeclarationToken.GetTokenType() == Token::Types::Constant)
					{
						return ParseConstantStatement(DeclarationToken);
					}

					//fatal("syntax error during unary prefix");
					return nullptr;
				}

				Statement * ShaderParser::ParseUnaryLogicalNotExpression(Token & DeclarationToken, EndConditions ConditionMask)
				{
					return nullptr;
				}

				Statement * ShaderParser::ParseUnaryBitwiseNotExpression(Token & DeclarationToken, EndConditions ConditionMask)
				{
					return nullptr;
				}

				Statement * ShaderParser::ParseBinary(int8 LeftHandPrecedence, Statement *LeftHandStatement, EndConditions ConditionMask)
				{
					while (true)
					{
						Token token;
						if (!GetToken(token))
							return nullptr;

						if (IsEndCondition(token, ConditionMask))
							break;

						OperatorStatement::Operators op = GetOperator(token.GetIdentifier());
						int8 precedence = GetOperatorPrecedence(op);

						if (precedence < LeftHandPrecedence)
							break;

						OperatorStatement *stm = Allocate<OperatorStatement>();

						Token rightHandToken;
						if (!GetToken(rightHandToken))
							return nullptr;

						Statement *rightHandStm = ParseUnaryExpression(rightHandToken, ConditionMask);


					}

					return LeftHandStatement;
				}

				Statement * ShaderParser::ParseConstantStatement(Token & DeclarationToken)
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

				Statement * ShaderParser::ParseMemberAccessStatement(Token &DeclarationToken)
				{
					MemberAccessStatement *stm = Allocate<MemberAccessStatement>();

					stm->SetName(DeclarationToken.GetIdentifier());

					Token token;
					if (!GetToken(token))
						return nullptr;

					if (token.Matches(DOT, Token::SearchCases::CaseSensitive))
					{
						Token memberToken;
						if (!GetToken(memberToken))
							return nullptr;

						Statement *child = ParseMemberAccessStatement(memberToken);

						if (child == nullptr)
							return nullptr;

						stm->SetMember(child);
					}
					else
						UngetToken(token);

					return stm;
				}

				Statement * ShaderParser::ParseFunctionCallStatement(Token &DeclarationToken)
				{
					FunctionCallStatement *stm = Allocate<FunctionCallStatement>();

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

						Statement *argStm = ParseExpression(token, EndConditions::Comma | EndConditions::Brace);

						if (argStm == nullptr)
							return nullptr;

						stm->InsertArgumentStatement(0, argStm);
					}

					return stm;
				}

				bool ShaderParser::IsEndCondition(Token Token, ShaderParser::EndConditions ConditionMask)
				{
					return
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Semicolon) && Token.Matches(SEMICOLON, Token::SearchCases::CaseSensitive)) ||
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Brace) && Token.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive)) ||
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::Comma) && Token.Matches(COMMA, Token::SearchCases::CaseSensitive));
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