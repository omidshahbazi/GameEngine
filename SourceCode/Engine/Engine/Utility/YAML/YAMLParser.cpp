// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Utility\YAML\YAMLParser.h>
#include <Utility\YAML\YAMLArray.h>
#include <Utility\YAML\YAMLCodeParser.h>
#include <Containers/\StringUtility.h>
#include <exception>

namespace Engine
{
	using namespace Containers;

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

				if (value == "true")
					Data = true;
				else if (value == "false")
					Data = AnyDataType(false);
				else if (value.Contains('.'))
					Data = StringUtility::ToFloat64(value);
				else if (CharacterUtility::IsDigit(value.GetValue()))
					Data = StringUtility::ToInt64(value);
				else if (type == Token::Types::Constant)
					Data = Token.GetConstantString();
			}

			void ParseArray(YAMLCodeParser::TokenList::Iterator& TokensIterator, YAMLArray& Array);

			void ParseObject(YAMLCodeParser::TokenList& Tokens, YAMLObject& Object)
			{
				auto tokenIT = Tokens.GetBegin();

				while (tokenIT != Tokens.GetEnd())
				{
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

					++tokenIT;
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