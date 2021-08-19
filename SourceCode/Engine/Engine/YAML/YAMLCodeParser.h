// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef YAML_CODE_PARSER_H
#define YAML_CODE_PARSER_H

#include <Lexer\Tokenizer.h>
#include <Containers\List.h>

namespace Engine
{
	using namespace Containers;
	using namespace Lexer;

	namespace YAML
	{
		enum class SymbolParseOptions
		{
			Normal = 0,
			CloseTemplateBracket
		};

		class YAML_API YAMLCodeParser : public Tokenizer
		{
		public:
			typedef List<Token> TokenList;

		public:
			YAMLCodeParser(const String& Text) :
				Tokenizer(Text)
			{
			}

			virtual void Parse(TokenList& Tokens);
		};
	}
}
#endif
