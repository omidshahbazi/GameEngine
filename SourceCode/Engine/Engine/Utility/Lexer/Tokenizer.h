// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <Utility\Lexer\Token.h>

namespace Engine
{
	using namespace Containers;

	namespace Utility
	{
		namespace Lexer
		{
			class UTILITY_API Tokenizer
			{
			public:
				Tokenizer(const String &Value);

				Token ReadNextToken(void);

			private:
				char8 GetNextChar(void);
				char8 ReadNextChar(void);

				bool IsWhitespace(char8 C);
				bool IsNewLine(char8 C);
				bool IsLetter(char8 C);
				bool IsDigit(char8 C);
				bool IsSign(char8 C);
				bool IsQuote(char8 C);

			private:
				String m_Value;
				uint32 m_Index;
				uint32 m_Column;
				uint32 m_Line;
			};
		}
	}
}

#endif