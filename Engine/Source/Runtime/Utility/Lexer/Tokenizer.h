// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <Containers\Strings.h>
#include <Lexer\Token.h>
#include <Lexer\Constants.h>
#include <functional>

namespace Engine
{
	using namespace Containers;

	namespace Lexer
	{
		class LEXER_API Tokenizer
		{
		public:
			Tokenizer(const String& Text);
			virtual ~Tokenizer(void)
			{
			}

			virtual void Parse(void);

		protected:
			virtual bool GetToken(Token& Token);
			virtual void UngetToken(const Token& Token);

			virtual char8 GetChar(bool Literal = false);
			virtual void UngetChar(void)
			{
				m_CurrentIndex = m_PrevIndex;
				m_CurrentLineIndex = m_PrevLineIndex;
				m_CurrentColumnIndex = m_PrevColumnIndex;
			}

			virtual char8 GetLeadingChar(void);

			virtual char8 PeekChar(void) const
			{
				return (m_CurrentIndex < m_Text.GetLength() ? m_Text[m_CurrentIndex] : '\0');
			}

			virtual bool RequireToken(Token& Token);

			virtual bool RequireSymbol(const String& Match, const String& Tag);
			virtual bool MatchSymbol(const String& Match);

			virtual bool RequireIdentifier(const String& Match, const String& Tag);
			virtual bool MatchIdentifier(const String& Match);

			INLINE bool IsDigit(char8 c) const
			{
				return (c >= CHAR_0 && c <= CHAR_9);
			}

			INLINE bool IsEOL(char8 c) const
			{
				return (c == RETURN || c == NEWLINE || c == '\0');
			}

			INLINE bool IsWhitespace(char8 c) const
			{
				return (c == SPACE || c == TAB || c == RETURN || c == NEWLINE);
			}

			INLINE bool IsAlphabetic(char8 c) const
			{
				return ((c >= UPPER_A && c <= UPPER_Z) || (c >= LOWER_A && c <= LOWER_Z) || c == UNDERLINE);
			}

			INLINE bool IsAlphanumeric(char8 c) const
			{
				return (IsAlphabetic(c) || IsDigit(c));
			}

		protected:
			String m_Text;

			uint32 m_CurrentIndex;
			uint32 m_PrevIndex;

			uint32 m_CurrentLineIndex;
			uint32 m_PrevLineIndex;

			uint32 m_CurrentColumnIndex;
			uint32 m_PrevColumnIndex;
		};
	}
}
#endif
