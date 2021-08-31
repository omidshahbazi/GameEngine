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
			LexerException(const String& What, uint32 LineNumber, uint32 ColumnNumber) :
				Exception(Categories::Utilities, What, "", LineNumber, ""),
				m_ColumnNumber(ColumnNumber)
			{
			}

			uint32 GetColumnNumber(void) const
			{
				return m_ColumnNumber;
			}

			virtual String ToString(void) const override
			{
				return GetWhat() + " at (" + StringUtility::ToString<char8>(GetLineNumber()) + ", " + StringUtility::ToString<char8>(m_ColumnNumber) + ")";
			}

		private:
			uint32 m_ColumnNumber;
		};

#define THROW_LEXER_EXCEPTION(What) throw LexerException(What, m_CurrentLineIndex, m_CurrentColumnIndex)
	}
}
#endif
