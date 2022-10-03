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

			virtual void Reset(void);

		protected:
			virtual bool GetToken(Token& Token);
			virtual void UngetToken(const Token& Token);

			virtual void FillDebugInfo(Token& Token);
			virtual bool FillIdentifier(char8 c, bool NoConst, Token& Token);
			virtual bool FillIntegralConstant(char8 c, bool NoConst, Token& Token);
			virtual bool FillStringConstant(char8 c, bool NoConst, Token& Token);
			virtual bool FillSymbol(char8 c, bool NoConst, bool DetectCloseTemplateBracket, Token& Token);

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

			virtual void RequireToken(Token& Token, const String& Tag);

			virtual bool MatchIdentifierToken(Token& Token);
			virtual void RequireIdentifierToken(Token& Token, const String& Tag);

			virtual bool MatchSymbolToken(Token& Token);
			virtual void RequireSymbolToken(Token& Token, const String& Tag);

			virtual bool MatchConstantToken(Token& Token);
			virtual void RequireConstantToken(Token& Token, const String& Tag);

			virtual bool MatchIdentifier(const String& Match);
			virtual void RequireIdentifier(const String& Match, const String& Tag);

			virtual bool MatchSymbol(const String& Match);
			virtual void RequireSymbol(const String& Match, const String& Tag);

			virtual void ThrowRequiredException(const String& Tag);
			virtual void ThrowMissingException(const String& Match, const String& Tag);

			INLINE uint32 GetPrevIndex(void) const
			{
				return m_PrevIndex;
			}

			INLINE uint32 GetCurrentIndex(void) const
			{
				return m_CurrentIndex;
			}

			INLINE uint32 GetPrevLineIndex(void) const
			{
				return m_PrevLineIndex;
			}

			INLINE uint32 GetCurrentLineIndex(void) const
			{
				return m_CurrentLineIndex;
			}

			INLINE uint32 GetPrevColumnIndex(void) const
			{
				return m_PrevColumnIndex;
			}

			INLINE uint32 GetCurrentColumnIndex(void) const
			{
				return m_CurrentColumnIndex;
			}

		private:
			String m_Text;

			uint32 m_PrevIndex;
			uint32 m_CurrentIndex;

			uint32 m_PrevLineIndex;
			uint32 m_CurrentLineIndex;

			uint32 m_PrevColumnIndex;
			uint32 m_CurrentColumnIndex;
		};
	}
}
#endif
