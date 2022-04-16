// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef PROGRAM_PARSER_EXCEPTION_H
#define PROGRAM_PARSER_EXCEPTION_H

#include <Lexer\LexerException.h>

namespace Engine
{
	using namespace Containers;
	using namespace Lexer;

	namespace ProgramParser
	{
		class PROGRAMPARSER_API ProgramParserException : public LexerException
		{
		public:
			ProgramParserException(const String& What, const String& Info, uint32 LineNumber, uint32 ColumnNumber) :
				LexerException(What, LineNumber, ColumnNumber)
			{
				SetCategoryFlags(Categories::ProgramCompiler);
				SetInfo(Info);
			}

			virtual String ToString(void) const override
			{
				return GetWhat() + " {" + GetInfo() + "} at (" + StringUtility::ToString<char8>(GetLineNumber()) + ", " + StringUtility::ToString<char8>(GetColumnNumber()) + ")";
			}
		};

#define THROW_PROGRAM_PARSER_EXCEPTION(What, Token) throw ProgramParserException(What, Token.GetIdentifier(), Token.GetLineIndex() + 1, Token.GetColumnIndex() + 1)
	}
}
#endif