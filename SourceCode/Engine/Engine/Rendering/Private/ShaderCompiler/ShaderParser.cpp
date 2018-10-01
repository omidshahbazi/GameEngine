// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderCompiler\ShaderParser.h>
#include <Rendering\Private\ShaderCompiler\StructType.h>
#include <Rendering\Private\ShaderCompiler\VariableType.h>
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

				const String STRUCT(STRINGIZE(struct));

				DataTypes GetDataType(Token &DeclarationToken)
				{
					auto &type = DeclarationToken.GetIdentifier();

					if (type == "float")
						return DataTypes::Float;
					if (type == "float2")
						return DataTypes::Float2;
					if (type == "float3")
						return DataTypes::Float3;
					if (type == "float4")
						return DataTypes::Float4;
					if (type == "float4x4")
						return DataTypes::Float4X4;

					return DataTypes::Unknown;
				}

				void ShaderParser::Parse(void)
				{
					Tokenizer::Parse();

					do
					{
						Token token;
						if (!GetToken(token))
							return;

						CompileStruct(token);

					} while (true);
				}

				ShaderParser::CompileResults ShaderParser::CompileStruct(Token &DeclarationToken)
				{
					StructType *structType = Allocate(StructType);
					Construct(structType);

					if (!DeclarationToken.Matches(STRUCT, Token::SearchCases::CaseSensitive))
						return CompileResults::Rejected;

					Token nameToken;
					if (!GetToken(nameToken))
						return CompileResults::Failed;

					if (nameToken.GetTokenType() != Token::Types::Identifier)
						return CompileResults::Failed;

					structType->SetName(nameToken.GetIdentifier());

					Token token;
					if (!GetToken(token))
						return CompileResults::Failed;
					if (!token.Matches(OPEN_BRACKET, Token::SearchCases::CaseSensitive))
						return CompileResults::Failed;

					while (true)
					{
						Token memberToken;
						if (!GetToken(memberToken))
							return CompileResults::Failed;

						if (memberToken.Matches(CLOSE_BRACKET, Token::SearchCases::CaseSensitive))
						{
							Token semiColonToken;
							if (!GetToken(semiColonToken))
								return CompileResults::Failed;

							if (memberToken.Matches(SEMI_COLON, Token::SearchCases::CaseSensitive))
								return CompileResults::Approved;
							else
								return CompileResults::Failed;
						}

						VariableType *variableType = Allocate(VariableType);
						Construct(variableType);

						structType->AddVariable(variableType);

						if (CompileVariable(memberToken, variableType) == CompileResults::Failed)
							return CompileResults::Failed;
					}
				}

				ShaderParser::CompileResults ShaderParser::CompileVariable(Token &DeclarationToken, VariableType *Variable)
				{
					GetDataType
				}
			}
		}
	}
}