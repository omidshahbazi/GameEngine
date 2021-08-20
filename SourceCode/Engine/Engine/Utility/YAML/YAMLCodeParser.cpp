// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <YAML\YAMLCodeParser.h>

namespace Engine
{
	using namespace Common;

	namespace YAML
	{
		void YAMLCodeParser::Parse(TokenList& Tokens)
		{
			Tokenizer::Parse();

			while (true)
			{
				Token token;
				if (!GetToken(token))
					break;

				Tokens.Add(token);
			}
		}
	}
}