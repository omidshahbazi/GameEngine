// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderCompiler\ShaderParser.h>
#include <Rendering\Private\ShaderCompiler\FunctionType.h>
#include <Rendering\Private\ShaderCompiler\VariableType.h>
#include <Rendering\Private\ShaderCompiler\ParameterType.h>
#include <Rendering\Private\Allocators.h>

namespace Engine
{
	using namespace Utility::Lexer;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
#define Allocate(Type) ReinterpretCast(Type*, AllocateMemory(&Allocators::RenderingSystemAllocator, sizeof(Type)))
#define Deallocate(Address) DeallocateMemory(&Allocators::RenderingSystemAllocator, Address)

				DataTypes GetDataType(Token &DeclarationToken)
				{
					auto &type = DeclarationToken.GetIdentifier();

					if (type == "void")
						return DataTypes::Void;
					if (type == "float")
						return DataTypes::Float;
					if (type == "float2")
						return DataTypes::Float2;
					if (type == "float3")
						return DataTypes::Float3;
					if (type == "float4")
						return DataTypes::Float4;
					if (type == "matrix4")
						return DataTypes::Matrix4;

					return DataTypes::Unknown;
				}

				void ShaderParser::Parse(VariableTypeList &Variables, FunctionTypeList &Functions)
				{
					Tokenizer::Parse();

					while (true)
					{
						Token token;
						if (!GetToken(token))
							return;

						CompileResults result = CompileResults::Failed;

						if ((result = CompileVariable(token, Variables)) == CompileResults::Approved)
							continue;
						else if (result == CompileResults::Failed)
							break;

						if ((result = CompileFunction(token, Functions)) == CompileResults::Approved)
							continue;
						else if (result == CompileResults::Failed)
							break;

					}
				}

				ShaderParser::CompileResults ShaderParser::CompileVariable(Token &DeclarationToken, VariableTypeList &Variables)
				{
					if (DeclarationToken.GetTokenType() != Token::Types::Identifier)
					{
						UngetToken(DeclarationToken);
						return CompileResults::Rejected;
					}

					VariableType *variableType = Allocate(VariableType);
					Construct(variableType);

					CompileResults result = CompileVariable(DeclarationToken, variableType);
					if (result == CompileResults::Approved)
						Variables.Add(variableType);
					else
						Deallocate(variableType);

					return result;
				}

				ShaderParser::CompileResults ShaderParser::CompileFunction(Token &DeclarationToken, FunctionTypeList &Functions)
				{
					Token nameToken;
					if (!GetToken(nameToken))
						return CompileResults::Failed;

					if (nameToken.GetTokenType() != Token::Types::Identifier)
					{
						UngetToken(nameToken);
						return CompileResults::Rejected;
					}

					Token openBraceToken;
					if (!GetToken(openBraceToken))
						return CompileResults::Failed;

					if (!openBraceToken.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
					{
						UngetToken(nameToken);
						return CompileResults::Rejected;
					}

					FunctionType *functionType = Allocate(FunctionType);
					Construct(functionType);
					Functions.Add(functionType);

					DataTypes dataType = GetDataType(DeclarationToken);
					if (dataType == DataTypes::Unknown)
						return CompileResults::Failed;

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
							return CompileResults::Failed;

						if (parameterToken.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive))
							break;

						ParameterType *parameterType = Allocate(ParameterType);
						Construct(parameterType);

						functionType->AddParamaeter(parameterType);

						if (CompileParameter(parameterToken, parameterType) == CompileResults::Failed)
							return CompileResults::Failed;
					}

					Token doubleColonToken;
					if (!GetToken(doubleColonToken))
						return CompileResults::Failed;
					if (doubleColonToken.Matches(COLON, Token::SearchCases::CaseSensitive))
					{
						Token registerToken;
						if (!GetToken(registerToken))
							return CompileResults::Failed;

						functionType->SetRegister(registerToken.GetIdentifier());
					}
					else
						UngetToken(doubleColonToken);

					Token openBracketToken;
					if (!GetToken(openBracketToken) || !openBracketToken.Matches(OPEN_BRACKET, Token::SearchCases::CaseSensitive))
						return CompileResults::Failed;

					while (true)
					{
						Token token;
						if (!GetToken(token))
							return CompileResults::Failed;

						if (token.Matches(CLOSE_BRACKET, Token::SearchCases::CaseSensitive))
							return CompileResults::Approved;


					}
				}

				ShaderParser::CompileResults ShaderParser::CompileVariable(Token &DeclarationToken, VariableType *Variable)
				{
					DataTypes dataType = GetDataType(DeclarationToken);
					if (dataType == DataTypes::Unknown)
						return CompileResults::Failed;

					Variable->SetDataType(dataType);

					Token nameToken;
					if (!GetToken(nameToken) || nameToken.GetTokenType() != Token::Types::Identifier)
						return CompileResults::Failed;

					Variable->SetName(nameToken.GetIdentifier());

					while (true)
					{
						Token token;
						if (!GetToken(token))
							return CompileResults::Failed;

						if (token.Matches(OPEN_BRACE, Token::SearchCases::CaseSensitive))
						{
							UngetToken(token);
							UngetToken(nameToken);
							return CompileResults::Rejected;
						}

						if (token.Matches(SEMI_COLON, Token::SearchCases::CaseSensitive))
							return CompileResults::Approved;

						if (token.Matches(COLON, Token::SearchCases::CaseSensitive))
						{
							Token registerToken;
							if (!GetToken(registerToken))
								return CompileResults::Failed;

							Variable->SetRegister(registerToken.GetIdentifier());
						}
					}
				}

				ShaderParser::CompileResults ShaderParser::CompileParameter(Token &DeclarationToken, ParameterType *Parameter)
				{
					DataTypes dataType = GetDataType(DeclarationToken);
					if (dataType == DataTypes::Unknown)
						return CompileResults::Failed;

					Parameter->SetDataType(dataType);

					Token nameToken;
					if (!GetToken(nameToken) || nameToken.GetTokenType() != Token::Types::Identifier)
						return CompileResults::Failed;

					Parameter->SetName(nameToken.GetIdentifier());

					while (true)
					{
						Token token;
						if (!GetToken(token))
							return CompileResults::Failed;

						if (token.Matches(COMMA, Token::SearchCases::CaseSensitive))
							return CompileResults::Approved;

						if (token.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive))
						{
							UngetToken(token);
							return CompileResults::Approved;
						}

						if (token.Matches(COLON, Token::SearchCases::CaseSensitive))
						{
							Token registerToken;
							if (!GetToken(registerToken))
								return CompileResults::Failed;

							Parameter->SetRegister(registerToken.GetIdentifier());
						}
					}
				}
			}
		}
	}
}