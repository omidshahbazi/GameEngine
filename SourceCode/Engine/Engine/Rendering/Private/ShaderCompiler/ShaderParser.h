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
				class FunctionType;
				class VariableType;
				class ParameterType;

				const String VERTEX_MAIN("vertexmain");
				const String FRAGMENT_MAIN("fragmentmain");

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
					typedef Vector<VariableType*> VariableTypeList;
					typedef Vector<FunctionType*> FunctionTypeList;

				public:
					ShaderParser(const String &Text) :
						Tokenizer(Text)
					{
					}

					void Parse(VariableTypeList &Variables, FunctionTypeList &Functions);

				private:
					CompileResults CompileVariable(Token &DeclarationToken, VariableTypeList &Variables);

					CompileResults CompileFunction(Token &DeclarationToken, FunctionTypeList &Functions);

					CompileResults CompileVariable(Token &DeclarationToken, VariableType *Variable);

					CompileResults CompileParameter(Token &DeclarationToken, ParameterType *Parameter);
				};
			}
		}
	}
}

#endif