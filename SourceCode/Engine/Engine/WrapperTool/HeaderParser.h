// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef HEADER_PARSER_H
#define HEADER_PARSER_H
#include <Utility\Lexer\Tokenizer.h>
#include <Containers\Stack.h>

namespace Engine
{
	using namespace Utility::Lexer;
	using namespace Containers;

	namespace WrapperTool
	{
		class WRAPPERTOOL_API HeaderParser : public Tokenizer
		{
		public:
			HeaderParser(const String& Text) :
				Tokenizer(Text)
			{
			}

			//virtual void Parse(TypesList& Types);

		protected:
			//virtual bool HeaderParser::CompileDeclaration(TypesList& Types, Token& DelarationToken);

			virtual void AddBlockLevel(void)
			{
				m_BlockLevel++;
			}

			virtual void AddNamespace(void);

			String GetNamespaces(void) const;

		private:
			uint16 m_BlockLevel;
		};
	}
}
#endif