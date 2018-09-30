// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderCompiler\ShaderParser.h>

namespace Engine
{
	using namespace Utility::Lexer;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				const String STRUCT(STRINGIZE(struct));

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
					if (!DeclarationToken.Matches(STRUCT, Token::SearchCases::CaseSensitive))
						return CompileResults::Rejected;

					Token nameToken;
					if (!GetToken(nameToken))
						return CompileResults::Failed;

					if (nameToken.GetTokenType() != Token::Types::Identifier)
						return CompileResults::Failed;

					//set name

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


					}
				}
			}
		}
	}
}