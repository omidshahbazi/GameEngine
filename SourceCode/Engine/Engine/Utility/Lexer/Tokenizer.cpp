// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Utility\Lexer\Tokenizer.h>

namespace Engine
{
	namespace Utility
	{
		namespace Lexer
		{
			Tokenizer::Tokenizer(const String &Value) :
				m_Value(Value),
				m_Index(0),
				m_Column(0),
				m_Line(1)
			{
			}

			Token Tokenizer::ReadNextToken(void)
			{
				String value;
				Token::Types type = Token::Types::Digit;

				while (true)
				{
					if (m_Index == m_Value.GetLength())
						return Token(Token::Types::End, "", m_Column, m_Line);

					char8 c = ReadNextChar();

					++m_Column;

					if (IsWhitespace(c))
						continue;

					if (IsNewLine(c))
					{
						m_Column = 0;
						++m_Line;
						continue;
					}

					if (IsLetter(c))
						type = Token::Types::Literal;
					else if (IsQuote(c))
						type = Token::Types::String;
					else if (c == '\t')
						return Token(Token::Types::Whitespace, c, m_Column, m_Line);
					else if (IsSign(c))
						return Token(Token::Types::Sign, c, m_Column, m_Line);

					value += c;
					while (true)
					{
						c = GetNextChar();

						if (type == Token::Types::String)
						{
							if (IsQuote(c))
							{
								ReadNextChar();
								value = value.SubString(1);
								return Token(type, value, m_Column, m_Line);
							}
						}
						else
						{
							if (IsWhitespace(c))
								return Token(type, value, m_Column, m_Line);

							if (IsNewLine(c))
								return Token(type, value, m_Column, m_Line);

							if (IsSign(c))
								return Token(type, value, m_Column, m_Line);
						}

						value += ReadNextChar();
					}
				}
			}

			char8 Tokenizer::GetNextChar(void)
			{
				return m_Value[m_Index];
			}

			char8 Tokenizer::ReadNextChar(void)
			{
				return m_Value[m_Index++];
			}

			bool Tokenizer::IsWhitespace(char8 C)
			{
				return (C == ' ');
			}

			bool Tokenizer::IsNewLine(char8 C)
			{
				return StringUtility::IsNewLine(C);
			}

			bool Tokenizer::IsLetter(char8 C)
			{
				return StringUtility::IsLetter(C);
			}

			bool Tokenizer::IsDigit(char8 C)
			{
				return StringUtility::IsDigit(C);
			}

			bool Tokenizer::IsSign(char8 C)
			{
				return !(IsWhitespace(C) || IsNewLine(C) || IsLetter(C) || IsDigit(C) || IsQuote(C));
			}

			bool Tokenizer::IsQuote(char8 C)
			{
				return (C == '\'' || C == '"');
			}
		}
	}
}