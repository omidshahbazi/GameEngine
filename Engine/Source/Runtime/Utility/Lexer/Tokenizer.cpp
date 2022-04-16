// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Lexer\Tokenizer.h>
#include <Lexer\LexerException.h>
#include <Common\CharacterUtility.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Lexer
	{
		Tokenizer::Tokenizer(const String& Text) :
			m_CurrentIndex(0),
			m_PrevIndex(0),
			m_CurrentLineIndex(0),
			m_PrevLineIndex(0),
			m_CurrentColumnIndex(0),
			m_PrevColumnIndex(0),
			m_Text(Text)
		{
		}

		void Tokenizer::Parse(void)
		{
			m_CurrentIndex = 0;
			m_PrevIndex = 0;
			m_CurrentLineIndex = 0;
			m_PrevLineIndex = 0;
			m_CurrentColumnIndex = 0;
			m_PrevColumnIndex = 0;
		}

		bool Tokenizer::GetToken(Token& Token)
		{
			char8 c = GetLeadingChar();
			char8 p = PeekChar();

			if (c == '\0')
			{
				UngetChar();
				return false;
			}

			Token.SetStartIndex(m_PrevIndex);
			Token.SetLineIndex(m_PrevLineIndex);
			Token.SetColumnIndex(m_PrevColumnIndex);
			Token.SetIdentifier("");

			if (IsAlphabetic(c))
			{
				do
				{
					Token.GetIdentifier() += c;
					c = GetChar();
				} while (IsAlphanumeric(c));

				UngetChar();

				Token.SetType(Token::Types::Identifier);
				//Token.SetName(Token.GetName());

				if (Token.Matches("true", Token::SearchCases::CaseSensitive))
				{
					Token.SetConstantBool(true);
					return true;
				}
				else if (Token.Matches("false", Token::SearchCases::CaseSensitive))
				{
					Token.SetConstantBool(false);
					return true;
				}

				return true;
			}
			else if (IsDigit(c) || ((c == PLUS || c == MINES) && IsDigit(p)))
			{
				bool isFloat = false;
				bool isHex = false;

				do
				{
					if (c == DOT)
						isFloat = true;

					if (c == UPPER_X || c == LOWER_X)
						isHex = true;

					Token.GetIdentifier() += c;

					c = CharacterUtility::ToUpper(GetChar());

				} while (IsDigit(c) || (!isFloat && c == DOT) || (!isHex && c == UPPER_X) || (isHex && c >= UPPER_A && c <= UPPER_F));

				if (isFloat || c != UPPER_F)
					UngetChar();

				if (isFloat)
					Token.SetConstantFloat32(StringUtility::ToFloat32(Token.GetIdentifier()));
				else if (isHex)
					Token.SetConstantInt32(StringUtility::ToInt32(Token.GetIdentifier()));
				else
					Token.SetConstantInt32(StringUtility::ToInt32(Token.GetIdentifier()));

				return true;
			}
			else if (c == DOUBLE_QUOTATION)
			{
				String temp;
				c = GetChar(true);
				while (c != DOUBLE_QUOTATION && !IsEOL(c))
				{
					if (c == BACK_SLASH)
					{
						c = GetChar(true);

						if (IsEOL(c))
							break;
						else if (c == 'n')
							c = '\n';
					}

					temp += c;

					c = GetChar(true);
				}

				Token.SetConstantString(temp);

				return true;
			}
			else
			{
				Token.GetIdentifier() += c;

#define PAIR(cc, dd) (c == cc && d == dd)

				char8 d = GetChar();

				if (PAIR('<', '<') ||
					PAIR('>', '>') ||
					PAIR('=', '=') ||
					PAIR('!', '=') ||
					PAIR('<', '=') ||
					PAIR('>', '=') ||
					PAIR('+', '+') ||
					PAIR('-', '-') ||
					PAIR('|', '|') ||
					PAIR('^', '^') ||
					PAIR('&', '&') ||
					PAIR('+', '=') ||
					PAIR('-', '=') ||
					PAIR('*', '=') ||
					PAIR('/', '=') ||
					PAIR('~', '=') ||
					PAIR(':', ':') ||
					PAIR('*', '*'))
				{
					Token.GetIdentifier() += d;

					if (c == '>' && d == '>')
					{
						if (GetChar() == '>')
							Token.GetIdentifier() += '>';
						else
							UngetChar();
					}
				}
				else
					UngetChar();

#undef PAIR

				Token.SetType(Token::Types::Symbol);
				Token.SetName(Token.GetIdentifier());

				return true;
			}
		}

		void Tokenizer::UngetToken(const Token& Token)
		{
			m_CurrentIndex = Token.GetStartIndex();
			m_CurrentLineIndex = Token.GetLineIndex();
			m_CurrentColumnIndex = Token.GetColumnIndex();
		}

		char8 Tokenizer::GetChar(bool Literal)
		{
			m_PrevIndex = m_CurrentIndex;
			m_PrevLineIndex = m_CurrentLineIndex;
			m_PrevColumnIndex = m_CurrentColumnIndex;

			char8 c;
			do
			{
				if (m_CurrentIndex == m_Text.GetLength())
					return '\0';

				c = m_Text[m_CurrentIndex++];
				++m_CurrentColumnIndex;

				if (c == NEWLINE)
				{
					m_CurrentLineIndex++;
					m_CurrentColumnIndex = 0;
				}
				else if (!Literal)
				{
					const char8 nextChar = PeekChar();
					if (c == SLASH && nextChar == STAR)
					{
						m_CurrentIndex++;
						continue;
					}
					else if (c == STAR && nextChar == SLASH)
					{
						m_CurrentIndex++;
						continue;
					}
				}

				return c;

			} while (true);
		}

		char8 Tokenizer::GetLeadingChar(void)
		{
			char8 trailingCommentNewline = '\0';

			while (true)
			{
				bool multipleNewline = false;
				char8 c;

				do
				{
					c = GetChar();

					if (c == trailingCommentNewline)
						multipleNewline = true;

				} while (IsWhitespace(c));

				if (!(c == SLASH && PeekChar() == SLASH))
					return c;

				if (multipleNewline)
				{

				}

				do
				{
					c = GetChar(true);

					if (c == '\0')
						return c;

				} while (!IsEOL(c));

				trailingCommentNewline = c;

				do
				{
					c = GetChar();

					if (c == '\0')
						return c;

					if (c == trailingCommentNewline || !IsEOL(c))
					{
						UngetChar();
						break;
					}
				} while (true);
			}

			return '\0';
		}

		void Tokenizer::RequireToken(Token& Token)
		{
			if (GetToken(Token))
				return;

			THROW_LEXER_EXCEPTION("Token required");
		}

		void Tokenizer::RequireIdentifierToken(Token& Token)
		{
			if (MatchIdentifierToken(Token))
				return;

			THROW_LEXER_EXCEPTION("Missing an identifier");
		}

		bool Tokenizer::MatchIdentifierToken(Token& Token)
		{
			RequireToken(Token);

			if (Token.GetTokenType() != Token::Types::Identifier)
			{
				UngetToken(Token);
				return false;
			}

			return true; 
		}

		void Tokenizer::RequireSymbol(const String& Match, const String& Tag)
		{
			if (MatchSymbol(Match))
				return;

			THROW_LEXER_EXCEPTION("Missing symbol '" + Match + "' in " + Tag);
		}

		bool Tokenizer::MatchSymbol(const String& Match)
		{
			Token token;
			if (!GetToken(token))
				return false;

			if (token.GetTokenType() == Token::Types::Symbol && token.Matches(Match, Token::SearchCases::CaseSensitive))
				return true;

			UngetToken(token);

			return false;
		}

		void Tokenizer::RequireIdentifier(const String& Match, const String& Tag)
		{
			if (MatchIdentifier(Match))
				return;

			THROW_LEXER_EXCEPTION("Missing identifier '" + Match + "' in " + Tag);
		}

		bool Tokenizer::MatchIdentifier(const String& Match)
		{
			Token token;
			if (!GetToken(token))
				return false;

			if (token.GetTokenType() == Token::Types::Identifier && token.Matches(Match, Token::SearchCases::IgnoreCase))
				return true;

			UngetToken(token);

			return false;
		}
	}
}