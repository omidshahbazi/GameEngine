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
			m_Text(Text),
			m_PrevIndex(0),
			m_CurrentIndex(0),
			m_PrevLineIndex(0),
			m_CurrentLineIndex(0),
			m_PrevColumnIndex(0),
			m_CurrentColumnIndex(0)
		{
		}

		void Tokenizer::Reset(void)
		{
			m_PrevIndex = 0;
			m_CurrentIndex = 0;
			m_PrevLineIndex = 0;
			m_CurrentLineIndex = 0;
			m_PrevColumnIndex = 0;
			m_CurrentColumnIndex = 0;
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

			String name = Token.GetName();

			if (CharacterUtility::IsAlphabetic(c))
			{
				do
				{
					name += c;
					c = GetChar();
				} while (CharacterUtility::IsAlphanumeric(c));

				UngetChar();

				Token.SetIdentifier(name);

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
			else if (CharacterUtility::IsDigit(c) || ((c == PLUS || c == MINES) && CharacterUtility::IsDigit(p)))
			{
				bool isFloat = false;
				bool isHex = false;

				do
				{
					if (c == DOT)
						isFloat = true;

					if (c == UPPER_X || c == LOWER_X)
						isHex = true;

					name += c;

					c = CharacterUtility::ToUpper(GetChar());

				} while (CharacterUtility::IsDigit(c) || (!isFloat && c == DOT) || (!isHex && c == UPPER_X) || (isHex && c >= UPPER_A && c <= UPPER_F));

				if (isFloat || c != UPPER_F)
					UngetChar();

				if (isFloat)
					Token.SetConstantFloat32(StringUtility::ToFloat32(name));
				else if (isHex)
					Token.SetConstantInt32(StringUtility::ToInt32(name));
				else
					Token.SetConstantInt32(StringUtility::ToInt32(name));

				return true;
			}
			else if (c == DOUBLE_QUOTATION)
			{
				String temp;
				c = GetChar(true);
				while (c != DOUBLE_QUOTATION && !CharacterUtility::IsEOL(c))
				{
					if (c == BACK_SLASH)
					{
						c = GetChar(true);

						if (CharacterUtility::IsEOL(c))
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
				name += c;

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
					name += d;

					if (c == '>' && d == '>')
					{
						if (GetChar() == '>')
							name += '>';
						else
							UngetChar();
					}
				}
				else
					UngetChar();

#undef PAIR

				Token.SetSymbol(name);

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

				} while (CharacterUtility::IsWhitespace(c));

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

				} while (!CharacterUtility::IsEOL(c));

				trailingCommentNewline = c;

				do
				{
					c = GetChar();

					if (c == '\0')
						return c;

					if (c == trailingCommentNewline || !CharacterUtility::IsEOL(c))
					{
						UngetChar();
						break;
					}
				} while (true);
			}

			return '\0';
		}

		void Tokenizer::RequireToken(Token& Token, const String& Tag)
		{
			if (GetToken(Token))
				return;

			ThrowRequiredException(Tag);
		}

		bool Tokenizer::MatchIdentifierToken(Token& Token)
		{
			RequireToken(Token, "identifier token parsing");

			if (Token.GetType() != Token::Types::Identifier)
			{
				UngetToken(Token);
				return false;
			}

			return true;
		}

		void Tokenizer::RequireIdentifierToken(Token& Token, const String& Tag)
		{
			if (MatchIdentifierToken(Token))
				return;

			ThrowRequiredException(Tag);
		}

		bool Tokenizer::MatchIdentifier(const String& Match)
		{
			Token token;
			if (!GetToken(token))
				return false;

			if (token.GetType() == Token::Types::Identifier && token.Matches(Match, Token::SearchCases::IgnoreCase))
				return true;

			UngetToken(token);

			return false;
		}

		void Tokenizer::RequireIdentifier(const String& Match, const String& Tag)
		{
			if (MatchIdentifier(Match))
				return;

			ThrowMissingException(Match, Tag);
		}

		bool Tokenizer::MatchSymbol(const String& Match)
		{
			Token token;
			if (!GetToken(token))
				return false;

			if (token.GetType() == Token::Types::Symbol && token.Matches(Match, Token::SearchCases::CaseSensitive))
				return true;

			UngetToken(token);

			return false;
		}

		void Tokenizer::RequireSymbol(const String& Match, const String& Tag)
		{
			if (MatchSymbol(Match))
				return;

			ThrowMissingException(Match, Tag);
		}

		void Tokenizer::ThrowRequiredException(const String& Tag)
		{
			Token token;
			if (GetToken(token))
				THROW_LEXER_EXCEPTION("Missing token before '" + token.GetName() + "' " + Tag);
			else
				THROW_LEXER_EXCEPTION("Missing token " + Tag);
		}

		void Tokenizer::ThrowMissingException(const String& Match, const String& Tag)
		{
			Token token;
			if (GetToken(token))
				THROW_LEXER_EXCEPTION("Missing '" + Match + "' before '" + token.GetName() + "' " + Tag);
			else
				THROW_LEXER_EXCEPTION("Missing '" + Match + "' " + Tag);
		}
	}
}