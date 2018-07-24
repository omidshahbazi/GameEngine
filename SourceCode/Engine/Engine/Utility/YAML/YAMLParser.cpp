// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Utility\YAML\YAMLParser.h>
#include <Utility\Lexer\Tokenizer.h>
#include <exception>

namespace Engine
{
	namespace Utility
	{
		using namespace Lexer;

		namespace YAML
		{
			bool CheckToken(const Token &Token, Token::Types Type, const String &Value)
			{
				return (Token.GetType() == Type && Token.GetValue() == Value);
			}

			void YAMLParser::Parse(const String &Value, YAMLObject &Object)
			{
				Tokenizer tokenizer(Value);

				while (true)
				{
					Token keyToken = tokenizer.ReadNextToken();

					if (keyToken.GetType() == Token::Types::End)
						break;

					Token colonToken = tokenizer.ReadNextToken();
					if (!CheckToken(colonToken, Token::Types::Sign, ":"))
						throw std::exception("':' expected");

					Token valueToken = tokenizer.ReadNextToken();

					if (valueToken.GetType() == Token::Types::Whitespace)
					{
						// new object or array
					}

					Object[keyToken.GetValue()] = valueToken.GetValue();
				}
			}
		}
	}
}