// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef SHADER_PARSER_H
#define SHADER_PARSER_H

#include <Containers\Strings.h>
#include <Utility\Lexer\Tokenizer.h>

namespace Engine
{
	using namespace Utility::Lexer;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class VariableType;

				class ShaderParser : private Tokenizer
				{
				private:
					enum class CompileResults
					{
						Approved = 0,
						Rejected,
						Failed
					};

				public:
					ShaderParser(const String &Text) :
						Tokenizer(Text)
					{
					}

					void Parse(void) override;

				private:
					CompileResults CompileStruct(Token &DeclarationToken);

					CompileResults CompileVariable(Token &DeclarationToken, VariableType *Variable);
				};
			}
		}
	}
}

#endif