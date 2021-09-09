// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PREPROCESSOR_PARSER_H
#define PREPROCESSOR_PARSER_H

#include <RenderCommon\Enums.h>
#include <ProgramParser\AbstractSyntaxTree\SyntaxItemHolder.h>
#include <Lexer\Tokenizer.h>
#include <Containers\Strings.h>
#include <Containers\Map.h>
#include <functional>
#include <memory>

namespace Engine
{
	using namespace MemoryManagement;
	using namespace Lexer;
	using namespace Containers;

	namespace ProgramParser
	{
		class PROGRAMPARSER_API PreprocessorParser : private Tokenizer
		{
		private:
			enum class EndConditions
			{
				None = 1 << 0,
				PreprocessorElse = 1 << 1,
				PreprocessorEndIf = 1 << 2
			};

		public:
			typedef std::function<bool(const String&, String& Source)> IncludeFunction;
			typedef Vector<String> DefineList;

			struct Parameters
			{
			public:
				IncludeFunction IncludeFunction;
				DefineList Defines;
				String Result;
			};

		public:
			PreprocessorParser(const String& Text);

			void Process(Parameters& Parameters);

		private:
			void Process(Parameters& Parameters, EndConditions ConditionMask);

			bool ParsePreprocessor(Token& DeclarationToken, Parameters& Parameters);

			void ParsePreprocessorBlock(Parameters& Parameters, bool ShouldRemove);

			bool IsEndCondition(Token Token, EndConditions ConditionMask);
		};
	}
}

#endif