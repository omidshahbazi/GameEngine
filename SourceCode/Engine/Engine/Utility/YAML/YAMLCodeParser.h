// Copyright 2012-2015 ?????????????. All Rights Reserved.
#ifndef YAML_CODE_PARSER_H
#define YAML_CODE_PARSER_H

#include <Utility\Lexer\Tokenizer.h>
#include <Containers\List.h>

namespace Engine
{
	using namespace Containers;
	using namespace Utility::Lexer;

	namespace Utility
	{
		namespace YAML
		{
			enum class SymbolParseOptions
			{
				Normal = 0,
				CloseTemplateBracket
			};

			class UTILITY_API YAMLCodeParser : public Tokenizer
			{
			public:
				typedef List<Token> TokenList;

			public:
				YAMLCodeParser(const String &Text) :
					Tokenizer(Text)
				{
				}

				virtual void Parse(TokenList &Types);
			};
		}
	}
}
#endif
