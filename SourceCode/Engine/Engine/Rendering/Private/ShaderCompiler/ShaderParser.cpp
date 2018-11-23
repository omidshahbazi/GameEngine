// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderCompiler\ShaderParser.h>
#include <Rendering\Private\ShaderCompiler\FunctionType.h>
#include <Rendering\Private\ShaderCompiler\VariableType.h>
#include <Rendering\Private\ShaderCompiler\ParameterType.h>
#include <Rendering\Private\ShaderCompiler\AssignmentStatement.h>
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
				template<typename T>
				INLINE T *Allocate(void)
				{
					T *value = ReinterpretCast(T*, AllocateMemory(&Allocators::RenderingSystemAllocator, sizeof(T)));
					Construct(value);
					return value;
				}

#define Deallocate(Address) DeallocateMemory(&Allocators::RenderingSystemAllocator, Address)

				const String EQUAL = STRINGIZE(= );
				const String IF = STRINGIZE(if);
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

				ShaderParser::ShaderParser(const String & Text) :
					Tokenizer(Text)
				{
					m_KwywordParsers[IF] = std::make_shared<KeywordParseFunction>([&](Token &Token) { return ParseIfStatement(Token); });
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
					Construct(variableType);

					ParseResults result = ParseVariable(DeclarationToken, variableType);
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
					Construct(functionType);
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
						Construct(parameterType);

						functionType->AddParamaeter(parameterType);

						if (ParseParameter(parameterToken, parameterType) == ParseResults::Failed)
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

					Token openBracketToken;
					if (!GetToken(openBracketToken) || !openBracketToken.Matches(OPEN_BRACKET, Token::SearchCases::CaseSensitive))
						return ParseResults::Failed;

					return ParseFunction(functionType);
				}

				ShaderParser::ParseResults ShaderParser::ParseVariable(Token &DeclarationToken, VariableType *Variable)
				{
					bool isConst = DeclarationToken.Matches(CONST, Token::SearchCases::CaseSensitive);
					Variable->SetIsConstant(isConst);

					Token dataTypeToken;
					if (isConst && !GetToken(dataTypeToken))
						return ParseResults::Failed;

					DataTypes dataType = GetDataType((isConst ? dataTypeToken : DeclarationToken).GetIdentifier());
					if (dataType == DataTypes::Unknown)
						return ParseResults::Failed;

					Variable->SetDataType(dataType);

					Token nameToken;
					if (!GetToken(nameToken) || nameToken.GetTokenType() != Token::Types::Identifier)
						return ParseResults::Failed;

					Variable->SetName(nameToken.GetIdentifier());

					while (true)
					{
						Token token;
						if (!GetToken(token))
							return ParseResults::Failed;

						if (token.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
						{
							UngetToken(token);
							UngetToken(nameToken);
							return ParseResults::Rejected;
						}

						if (token.Matches(SEMICOLON, Token::SearchCases::CaseSensitive))
							return ParseResults::Approved;

						if (token.Matches(COLON, Token::SearchCases::CaseSensitive))
						{
							Token registerToken;
							if (!GetToken(registerToken))
								return ParseResults::Failed;

							Variable->SetRegister(registerToken.GetIdentifier());
						}
					}
				}

				ShaderParser::ParseResults ShaderParser::ParseParameter(Token &DeclarationToken, ParameterType *Parameter)
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

				ShaderParser::ParseResults ShaderParser::ParseFunction(FunctionType *Function)
				{
					while (true)
					{
						Token token;
						if (!GetToken(token))
							return ParseResults::Failed;

						if (token.Matches(CLOSE_BRACKET, Token::SearchCases::CaseSensitive))
							return ParseResults::Approved;

						Statement *stm = nullptr;

						if (m_KwywordParsers.Contains(token.GetIdentifier()))
							stm = (*m_KwywordParsers[token.GetIdentifier()])(token);
						else
							stm = ParseStatement(token);

						if (stm == nullptr)
							return ParseResults::Failed;

						Function->AddStatement(stm);
					}
				}

				Statement *ShaderParser::ParseIfStatement(Token &DeclarationToken)
				{
					return nullptr;
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
					return nullptr;
				}

				Statement *ShaderParser::ParseDiscardStatement(Token &DeclarationToken)
				{
					return nullptr;
				}

				Statement *ShaderParser::ParseSemicolonStatement(Token &DeclarationToken)
				{
					return Allocate<SemicolonStatement>();
				}

				Statement *ShaderParser::ParseFunctionCallStatement(Token & DeclarationToken)
				{
					FunctionCallStatement *stm = Allocate<FunctionCallStatement>();

					stm->SetFunctionName(DeclarationToken.GetIdentifier());

					while (true)
					{
						Token token;
						if (!GetToken(token))
							return nullptr;

						if (token.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive))
							break;

						if (token.Matches(COMMA, Token::SearchCases::CaseSensitive))
							continue;

						Statement *argStm = ParseStatement(token);

						if (argStm == nullptr)
							return nullptr;

						stm->AddArgumentStatement(argStm);
					}

					return stm;
				}

				Statement *ShaderParser::ParseMemberAccessStatement(Token & DeclarationToken)
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

						Statement *memberStm = ParseMemberAccessStatement(memberToken);
						if (memberStm == nullptr)
							return nullptr;

						stm->SetMember(memberStm);
					}

					if (token.Matches(EQUAL, Token::SearchCases::CaseSensitive) ||
						token.Matches(SEMICOLON, Token::SearchCases::CaseSensitive))
						UngetToken(token);

					return stm;
				}

				Statement *ShaderParser::ParseConstantStatement(Token & DeclarationToken)
				{
					ConstantStatement *stm = Allocate<ConstantStatement>();
					stm->SetValue(StringUtility::ToFloat32(DeclarationToken.GetIdentifier()));
					return stm;
				}

				Statement * ShaderParser::ParseAssignmentStatement(Token & DeclarationToken)
				{
					bool equalSignFound = false;

					while (true)
					{
						Token token;
						if (!GetToken(token))
							return nullptr;

						if (token.Matches(EQUAL, Token::SearchCases::CaseSensitive))
						{
							equalSignFound = true;
							break;
						}
						else if (token.Matches(SEMICOLON, Token::SearchCases::CaseSensitive))
							break;
					}

					if (!equalSignFound)
					{
						UngetToken(DeclarationToken);
						return nullptr;
					}

					Token rightToken;
					if (!GetToken(rightToken))
						return nullptr;

					Statement *rightStm = ParseStatement(rightToken);

					if (rightStm == nullptr)
						return nullptr;

					Token endToken;
					if (!GetToken(endToken))
						return nullptr;

					UngetToken(DeclarationToken);

					Token leftToken;
					if (!GetToken(leftToken))
						return nullptr;

					Statement *leftStm = ParseMemberAccessStatement(leftToken);
					if (leftStm == nullptr)
						return nullptr;

					AssignmentStatement *stm = Allocate<AssignmentStatement>();
					stm->SetLeft(leftStm);
					stm->SetRight(rightStm);

					UngetToken(endToken);

					return stm;
				}

				Statement * ShaderParser::ParseVariableStatement(Token & DeclarationToken)
				{
					UngetToken(DeclarationToken);

					Token declaration;
					if (!GetToken(declaration))
						return nullptr;

					DataTypes dataType = GetDataType(declaration.GetIdentifier());

					VariableStatement *stm = Allocate<VariableStatement>();

					stm->SetDataType(dataType);

					Token nameToken;
					if (!GetToken(nameToken))
						return nullptr;
					stm->SetName(nameToken.GetIdentifier());

					Token assignmentToken;
					if (!GetToken(assignmentToken))
						return nullptr;

					if (assignmentToken.Matches(EQUAL, Token::SearchCases::CaseSensitive))
					{
						Token initialToken;
						if (!GetToken(initialToken))
							return nullptr;

						Statement *initialStm = ParseStatement(initialToken);

						if (initialStm == nullptr)
							return nullptr;

						stm->SetInitialStatement(initialStm);
					}

					return stm;
				}

				Statement *ShaderParser::ParseStatement(Token &DeclarationToken)
				{
					DataTypes dataType = GetDataType(DeclarationToken.GetIdentifier());

					if (dataType == DataTypes::Unknown)
					{
						if (DeclarationToken.GetTokenType() == Token::Types::Constant)
							return ParseConstantStatement(DeclarationToken);

						Token token;
						if (!GetToken(token))
							return nullptr;

						if (token.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
							return ParseFunctionCallStatement(DeclarationToken);

						UngetToken(token);
						Statement *stm = ParseAssignmentStatement(DeclarationToken);
						if (stm != nullptr)
							return stm;

						return ParseMemberAccessStatement(DeclarationToken);
					}
					else
					{
						Token nameToken;
						if (!GetToken(nameToken))
							return nullptr;

						if (nameToken.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
							return ParseFunctionCallStatement(DeclarationToken);

						if (nameToken.GetTokenType() != Token::Types::Identifier)
							return nullptr;

						return ParseVariableStatement(DeclarationToken);
					}
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