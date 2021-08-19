// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_PARSER_PREPROCESS_H
#define PROGRAM_PARSER_PREPROCESS_H

#include <Rendering\ProgramDataTypes.h>
#include <Rendering\ProgramInfo.h>
#include <Rendering\Private\ProgramCompiler\Syntax\SyntaxItemHolder.h>
#include <MemoryManagement\Allocator\AllocatorBase.h>
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

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				class ProgramParserPreprocess : private Tokenizer
				{
				private:
					enum class ParseResults
					{
						Approved = 0,
						Rejected,
						Failed
					};

					enum class EndConditions
					{
						None = 1 << 0,
						PreprocessorElse = 1 << 1,
						PreprocessorEndIf = 1 << 2
					};

				public:
					typedef std::function<bool(const String&, String& Source)> IncludeFunction;

					struct Parameters
					{
					public:
						IncludeFunction IncludeFunction;
						ProgramInfo::DefineList Defines;
						String Result;
					};

				public:
					ProgramParserPreprocess(const String& Text);

					bool Process(Parameters& Parameters);

				private:
					bool Process(Parameters& Parameters, EndConditions ConditionMask);

					ParseResults ParsePreprocessor(Token& DeclarationToken, Parameters& Parameters);

					ParseResults ParsePreprocessorBlock(Parameters& Parameters, bool ShouldRemove);

					bool IsEndCondition(Token Token, EndConditions ConditionMask);
				};
			}
		}
	}
}

#endif