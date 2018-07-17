// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Utility\Lexer\Lexer.h>

namespace Engine
{
	namespace Utility
	{
		namespace Lexer
		{
			Lexer::Lexer(const String &Value) :
				m_Value(Value),
				m_Index(0),
				m_Column(0),
				m_Line(1)
			{
			}

			Token Lexer::ReadNextToken(void)
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
					else if (IsSign(c))
						return Token(Token::Types::Sign, c, m_Column, m_Line);

					value += c;
					while (true)
					{
						c = GetNextChar();

						if ((type == Token::Types::String && IsQuote(c)))
						{
							value += ReadNextChar();
							return Token(type, value, m_Column, m_Line);
						}

						if (IsWhitespace(c))
							return Token(type, value, m_Column, m_Line);

						if (IsNewLine(c))
							return Token(type, value, m_Column, m_Line);

						if (IsSign(c))
							return Token(type, value, m_Column, m_Line);

						value += ReadNextChar();
					}
				}
			}

			char8 Lexer::GetNextChar(void)
			{
				return m_Value[m_Index];
			}

			char8 Lexer::ReadNextChar(void)
			{
				return m_Value[m_Index++];
			}

			bool Lexer::IsWhitespace(char8 C)
			{
				return (C == ' ' || C == '\t');
			}

			bool Lexer::IsNewLine(char8 C)
			{
				return (C == '\n' || C == '\r');
			}

			bool Lexer::IsLetter(char8 C)
			{
				return (C >= 'A' && C <= 'Z') || (C >= 'a' && C <= 'z');
			}

			bool Lexer::IsDigit(char8 C)
			{
				return (C >= '0' && C <= '9');
			}

			bool Lexer::IsSign(char8 C)
			{
				return !(IsWhitespace(C) || IsNewLine(C) || IsLetter(C) || IsDigit(C) || IsQuote(C));
			}

			bool Lexer::IsQuote(char8 C)
			{
				return (C == '\'' || C == '"');
			}
		}
	}
}