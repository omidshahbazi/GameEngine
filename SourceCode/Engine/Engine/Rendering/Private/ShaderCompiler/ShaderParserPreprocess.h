// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SHADER_PARSER_PREPROCESS_H
#define SHADER_PARSER_PREPROCESS_H

#include <Rendering\ShaderDataType.h>
#include <Rendering\ShaderInfo.h>
#include <Rendering\Private\ShaderCompiler\Syntax\StatementsHolder.h>
#include <MemoryManagement\Allocator\AllocatorBase.h>
#include <Utility\Lexer\Tokenizer.h>
#include <Containers\Strings.h>
#include <Containers\Map.h>
#include <functional>
#include <memory>

namespace Engine
{
	using namespace MemoryManagement;
	using namespace Utility::Lexer;
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class ShaderParserPreprocess : private Tokenizer
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
						ShaderInfo::DefineList Defines;
						String Result;
					};

				public:
					ShaderParserPreprocess(const String& Text);

					void Process(Parameters& Parameters);

				private:
					void Process(Parameters& Parameters, EndConditions ConditionMask);

					ParseResults ParsePreprocessor(Token& DeclarationToken, Parameters& Parameters);

					ParseResults ParsePreprocessorBlock(Parameters& Parameters, bool ShouldRemove);

					bool IsEndCondition(Token Token, ShaderParserPreprocess::EndConditions ConditionMask);

				public:
					static ShaderDataType::Types GetDataType(const String& Name);
				};
			}
		}
	}
}

#endif