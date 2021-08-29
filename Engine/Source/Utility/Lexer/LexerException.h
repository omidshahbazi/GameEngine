// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef LEXER_EXCEPTION_H
#define LEXER_EXCEPTION_H

#include <Containers\Exception.h>

namespace Engine
{
	using namespace Containers;

	namespace Lexer
	{
		class LexerException : public Exception
		{
		public:
			LexerException(const String& What, uint32 LineNumber) :
				Exception(Categories::Utilities, What, "", LineNumber, "")
			{
			}
		};

#define THROW_LEXER_EXCEPTION(What) throw LexerException(What, m_CurrentLineIndex)
	}
}
#endif
