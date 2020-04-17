// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Utility\YAML\YAMLParser.h>
#include <Utility\YAML\YAMLArray.h>
#include <Utility\YAML\YAMLCodeParser.h>
#include <exception>

namespace Engine
{
	namespace Utility
	{
		using namespace Lexer;

		namespace YAML
		{
			bool CheckToken(const Token& Token, Token::Types Type, const String& Value)
			{
				return (Token.GetTokenType() == Type && Token.GetIdentifier() == Value);
			}

			void ParseValue(YAMLData& Data, const Token& Token)
			{
				Token::Types type = Token.GetTokenType();
				const String& value = Token.GetIdentifier();

				if (type == Token::Types::Identifier)
					Data = value;
				else if (value == "true")
					Data = true;
				else if (value == "false")
					Data = AnyDataType(false);
				else if (value.Contains('.'))
					Data = std::atof(value.GetValue());
				else if (CharacterUtility::IsDigit(value.GetValue()))
					Data = std::atoll(value.GetValue());
			}

			void ParseArray(YAMLCodeParser::TokenList::Iterator& TokensIterator, YAMLArray& Array);

			void ParseObject(YAMLCodeParser::TokenList& Tokens, YAMLObject& Object)
			{
				while (true)
				{
					auto tokenIT = Tokens.GetBegin();

					if (tokenIT == Tokens.GetEnd())
						break;

					Token& keyToken = *tokenIT;

					++tokenIT;
					Token& colonToken = *tokenIT;
					if (!CheckToken(colonToken, Token::Types::Symbol, ":"))
						throw std::exception("':' expected");

					++tokenIT;
					Token& valueToken = *tokenIT;

					//if (valueToken.GetTokenType() == Token::Types::Whitespace)
					//{
					//	++tokenIT;
					//	valueToken = *tokenIT;

					//	if (CheckToken(valueToken, Token::Types::Symbol, '-'))
					//	{
					//		YAMLArray *arr = new YAMLArray();
					//		Object[keyToken.GetIdentifier()] = arr;
					//		ParseArray(tokenIT, *arr);
					//	}
					//	else
					//	{
					//		YAMLObject *obj = new YAMLObject();
					//		Object[keyToken.GetIdentifier()] = obj;
					//		ParseObject(Tokens, *obj);
					//	}
					//}

					ParseValue(Object[keyToken.GetIdentifier()], valueToken);
				}
			}

			void ParseArray(YAMLCodeParser::TokenList::Iterator& TokensIterator, YAMLArray& Array)
			{
				while (true)
				{
					Token& valueToken = *TokensIterator;

					++TokensIterator;
					Token& nextToken = *TokensIterator;
					//if (!CheckToken(nextToken, Token::Types::Whitespace, '\t'))
					//	break;

					//++tokenIT;
					//nextToken = Tokenizer.ReadNextToken();
					if (CheckToken(nextToken, Token::Types::Symbol, '-'))
					{
						Array.Add("");
						ParseValue(Array[Array.GetSize() - 1], valueToken);
					}
					else
						break;
				}
			}

			void YAMLParser::Parse(const String& Value, YAMLObject& Object)
			{
				YAMLCodeParser parser(Value);

				YAMLCodeParser::TokenList tokens;
				parser.Parse(tokens);

				ParseObject(tokens, Object);
			}
		}
	}
}