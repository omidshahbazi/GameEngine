// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Utility\YAML\YAMLCodeParser.h>

namespace Engine
{
	using namespace Common;

	namespace Utility
	{
		namespace YAML
		{
			void YAMLCodeParser::Parse(TokenList& Tokens, const String& Bullshit)
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
}