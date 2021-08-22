// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Lexer\Tokenizer.h>
#include <Common\CharacterUtility.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace Lexer
	{
		Tokenizer::Tokenizer(const String& Text, ErrorFunction OnError) :
			m_CurrentIndex(0),
			m_PrevIndex(0),
			m_CurrentLineIndex(0),
			m_PrevLineIndex(0),
			m_Text(Text),
			m_OnError(OnError)
		{
		}

		void Tokenizer::Parse(void)
		{
			m_CurrentIndex = 0;
			m_PrevIndex = 0;
			m_CurrentLineIndex = 0;
			m_PrevLineIndex = 0;
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
				Token.SetName(Token.GetName());

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

		void Tokenizer::UngetToken(Token& Token)
		{
			m_CurrentIndex = Token.GetStartIndex();
			m_CurrentLineIndex = Token.GetLineIndex();
		}

		char8 Tokenizer::GetChar(bool Literal)
		{
			m_PrevIndex = m_CurrentIndex;
			m_PrevLineIndex = m_CurrentLineIndex;

			char8 c;
			do
			{
				if (m_CurrentIndex == m_Text.GetLength())
					return '\0';

				c = m_Text[m_CurrentIndex++];

				if (c == NEWLINE)
					m_CurrentLineIndex++;
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

		bool Tokenizer::RequireToken(Token& Token)
		{
			if (GetToken(Token))
				return true;

			RaisError("Token required");

			return false;
		}

		bool Tokenizer::RequireSymbol(const String& Match, const String& Tag)
		{
			if (MatchSymbol(Match))
				return true;

			RaisError("Missing '" + Match + "' in " + Tag);

			return false;
		}

		bool Tokenizer::MatchSymbol(const String& Match)
		{
			Token token;

			if (GetToken(token))
				if (token.GetTokenType() == Token::Types::Symbol && token.Matches(Match, Token::SearchCases::CaseSensitive))
					return true;
				else
					UngetToken(token);

			return false;
		}

		bool Tokenizer::RequireIdentifier(const String& Match, const String& Tag)
		{
			if (MatchIdentifier(Match))
				return true;

			RaisError("Missing '" + Match + "' in " + Tag);

			return false;
		}

		bool Tokenizer::MatchIdentifier(const String& Match)
		{
			Token token;

			if (GetToken(token))
				if (token.GetTokenType() == Token::Types::Identifier && token.Matches(Match, Token::SearchCases::IgnoreCase))
					return true;
				else
					UngetToken(token);

			return false;
		}

		void Tokenizer::RaisError(const String& Message)
		{
			if (m_OnError != nullptr)
				m_OnError(Message, m_CurrentLineIndex);
		}
	}
}