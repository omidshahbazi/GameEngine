// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Utility\YAML\YAMLParser.h>
#include <Utility\YAML\YAMLArray.h>
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

			void ParseValue(YAMLData &Data, const Token &Token)
			{
				Token::Types type = Token.GetType();
				const String &value = Token.GetValue();

				if (type == Token::Types::String)
					Data = value;
				else if (value == "true")
					Data = true;
				else if (value == "false")
					Data = false;
				else if (value.Contains('.'))
					Data = std::atof(value.GetValue());
				else if (CharacterUtility::IsDigit(value.GetValue()))
					Data = std::atoll(value.GetValue());
			}

			void ParseArray(Tokenizer &Tokenizer, YAMLArray &Array);

			void ParseObject(Tokenizer &Tokenizer, YAMLObject &Object)
			{
				while (true)
				{
					Token keyToken = Tokenizer.ReadNextToken();

					if (keyToken.GetType() == Token::Types::End)
						break;

					Token colonToken = Tokenizer.ReadNextToken();
					if (!CheckToken(colonToken, Token::Types::Sign, ":"))
						throw std::exception("':' expected");

					Token valueToken = Tokenizer.ReadNextToken();

					if (valueToken.GetType() == Token::Types::Whitespace)
					{
						valueToken = Tokenizer.ReadNextToken();

						if (CheckToken(valueToken, Token::Types::Sign, '-'))
						{
							YAMLArray *arr = new YAMLArray();
							Object[keyToken.GetValue()] = arr;
							ParseArray(Tokenizer, *arr);
						}
						else
						{
							YAMLObject *obj = new YAMLObject();
							Object[keyToken.GetValue()] = obj;
							ParseObject(Tokenizer, *obj);
						}
					}

					ParseValue(Object[keyToken.GetValue()], valueToken);
				}
			}

			void ParseArray(Tokenizer &Tokenizer, YAMLArray &Array)
			{
				while (true)
				{
					Token valueToken = Tokenizer.ReadNextToken();

					if (valueToken.GetType() == Token::Types::End)
						break;

					Token nextToken = Tokenizer.ReadNextToken();
					if (!CheckToken(nextToken, Token::Types::Whitespace, '\t'))
						break;

					nextToken = Tokenizer.ReadNextToken();
					if (CheckToken(nextToken, Token::Types::Sign, '-'))
					{
						Array.Add("");
						ParseValue(Array[Array.GetSize() - 1], valueToken);
					}
					else
						break;
				}
			}

			void YAMLParser::Parse(const String &Value, YAMLObject &Object)
			{
				Tokenizer tokenizer(Value);

				ParseObject(tokenizer, Object);
			}
		}
	}
}