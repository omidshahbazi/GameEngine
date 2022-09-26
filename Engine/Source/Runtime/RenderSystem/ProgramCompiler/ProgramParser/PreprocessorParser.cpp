// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ProgramParser\PreprocessorParser.h>
#include <ProgramParser\ProgramParserException.h>
#include <Common\BitwiseUtils.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Lexer;

	namespace ProgramParser
	{
		using namespace AbstractSyntaxTree;

		cstr PREPROCESSOR_INCLUDE = "include";
		cstr PREPROCESSOR_DEFINE = "define";
		cstr PREPROCESSOR_UNDEF = "undef";
		cstr PREPROCESSOR_IFDEF = "ifdef";
		cstr PREPROCESSOR_IFNDEF = "ifndef";
		cstr PREPROCESSOR_ELSE = "else";
		cstr PREPROCESSOR_ENDIF = "endif";

		bool IsDefined(const PreprocessorParser::DefineList& Defines, const String& Define)
		{
			for (auto& define : Defines)
			{
				if (define == Define)
					return true;
			}

			return false;
		}

		PreprocessorParser::PreprocessorParser(const String& Text) :
			Tokenizer(Text)
		{
		}

		void PreprocessorParser::Process(Parameters& Parameters)
		{
			Tokenizer::Reset();

			Process(Parameters, EndConditions::None);
		}

		void PreprocessorParser::Process(Parameters& Parameters, EndConditions ConditionMask)
		{
#define ADD_TO_RESULT(Value) Parameters.Result += String(" ") + String(Value)

			uint32 lastLineIndex = 0;
			while (true)
			{
				Token token;
				if (!GetToken(token))
					return;

				if (IsEndCondition(token, ConditionMask))
				{
					return;
				}
				else if (token.Matches(SHARP, Token::SearchCases::CaseSensitive))
				{
					Token preprocessorCommandToken;
					if (!GetToken(preprocessorCommandToken))
						return;

					if (IsEndCondition(preprocessorCommandToken, ConditionMask))
					{
						return;
					}
					else
						UngetToken(preprocessorCommandToken);
				}

				if (lastLineIndex != token.GetLineIndex())
				{
					uint32 diff = token.GetLineIndex() - lastLineIndex;
					for (uint32 i = 0; i < diff; ++i)
						ADD_TO_RESULT("\n");
					lastLineIndex = token.GetLineIndex();
				}

				if (ParsePreprocessor(token, Parameters))
					continue;

				switch (token.GetType())
				{
				case Token::Types::Identifier:
				case Token::Types::Symbol:
					ADD_TO_RESULT(token.GetName());
					break;

				case Token::Types::ConstantBool:
					ADD_TO_RESULT(token.GetName());
					break;

				case Token::Types::ConstantInt32:
					ADD_TO_RESULT(StringUtility::ToString<char8>(token.GetConstantInt32()));
					break;

				case Token::Types::ConstantInt64:
					ADD_TO_RESULT(StringUtility::ToString<char8>(token.GetConstantInt64()));
					break;

				case Token::Types::ConstantFloat32:
					ADD_TO_RESULT(StringUtility::ToString<char8>(token.GetConstantFloat32()));
					break;

				case Token::Types::ConstantFloat64:
					ADD_TO_RESULT(StringUtility::ToString<char8>(token.GetConstantFloat64()));
					break;

				default:
					THROW_NOT_IMPLEMENTED_EXCEPTION(Categories::ProgramCompiler);
					break;
				}
			}

#undef ADD_TO_RESULT
		}

		bool PreprocessorParser::ParsePreprocessor(Token& DeclarationToken, Parameters& Parameters)
		{
			if (!DeclarationToken.Matches(SHARP, Token::SearchCases::CaseSensitive))
				return false;

			if (MatchIdentifier(PREPROCESSOR_INCLUDE))
			{
				Token openBracketToken;
				RequireSymbol(OPEN_ANGLE_BRACKET, "Preprocess include directive");

				String fileName;
				while (true)
				{
					Token token;
					RequireToken(token, "parse preprocessor include");

					if (token.Matches(CLOSE_ANGLE_BRACKET, Token::SearchCases::CaseSensitive))
						break;

					fileName += token.GetName();
				}

				String source;
				if (!Parameters.IncludeFunction(fileName, source))
					THROW_PROGRAM_PARSER_EXCEPTION("Couldn't find the include file {" + fileName + "}", openBracketToken);

				PreprocessorParser parser(source);
				parser.Process(Parameters);

				return true;
			}

			if (MatchIdentifier(PREPROCESSOR_DEFINE))
			{
				Token nameToken;
				RequireToken(nameToken, "parse preprocessor definition");

				bool isDuplicate = false;
				for (const auto& define : Parameters.Defines)
					if (define == nameToken.GetName())
					{
						isDuplicate = true;
						break;
					}

				if (!isDuplicate)
					Parameters.Defines.Add({ nameToken.GetName() });

				return true;
			}

			if (MatchIdentifier(PREPROCESSOR_UNDEF))
			{
				Token nameToken;
				RequireToken(nameToken, "parse preprocessor undef");

				for (uint32 i = 0; i < Parameters.Defines.GetSize(); ++i)
					if (Parameters.Defines[i] == nameToken.GetName())
						Parameters.Defines.RemoveAt(i--);

				return true;
			}

			bool isNotDef = false;
			if (MatchIdentifier(PREPROCESSOR_IFDEF) || (isNotDef = MatchIdentifier(PREPROCESSOR_IFNDEF)))
			{
				Token nameToken;
				RequireToken(nameToken, "parse preprocessor ifdef/ifndef directive");

				bool shouldRemoveBlock = (IsDefined(Parameters.Defines, nameToken.GetName()) == isNotDef);

				ParsePreprocessorBlock(Parameters, shouldRemoveBlock);

				Token sharpToken;
				RequireSymbol(SHARP, "parse preprocess ifdef/ifndef directive");

				Token preprocessorToken;
				RequireToken(preprocessorToken, "parse preprocess ifdef/ifndef directive");

				if (preprocessorToken.Matches(PREPROCESSOR_ELSE, Token::SearchCases::CaseSensitive))
					ParsePreprocessorBlock(Parameters, !shouldRemoveBlock);

				return true;
			}

			THROW_PROGRAM_PARSER_EXCEPTION("Unexpected token", DeclarationToken);
		}

		void PreprocessorParser::ParsePreprocessorBlock(Parameters& Parameters, bool ShouldRemove)
		{
			if (ShouldRemove)
			{
				Token prevToken;

				int32 openBlockCount = 1;
				while (true)
				{
					Token token;
					RequireToken(token, "parse preprocess block");

					if (token.Matches(PREPROCESSOR_IFDEF, Token::SearchCases::CaseSensitive) || token.Matches(PREPROCESSOR_IFNDEF, Token::SearchCases::CaseSensitive))
					{
						++openBlockCount;
					}
					else if (token.Matches(PREPROCESSOR_ELSE, Token::SearchCases::CaseSensitive))
					{
						if (--openBlockCount == 0)
						{
							UngetToken(prevToken);

							break;
						}
					}
					else if (token.Matches(PREPROCESSOR_ENDIF, Token::SearchCases::CaseSensitive))
					{
						if (--openBlockCount == 0)
							break;
					}

					prevToken = token;
				}
			}

			Process(Parameters, EndConditions::PreprocessorElse | EndConditions::PreprocessorEndIf);

			Token sharpToken;
			RequireToken(sharpToken, "parse preprocess block");

			Token preprocessorToken;
			RequireToken(preprocessorToken, "parse preprocess block");

			if (preprocessorToken.Matches(PREPROCESSOR_ELSE, Token::SearchCases::CaseSensitive))
			{
				UngetToken(sharpToken);
				return;
			}

			if (preprocessorToken.Matches(PREPROCESSOR_ENDIF, Token::SearchCases::CaseSensitive))
				return;

			THROW_PROGRAM_PARSER_EXCEPTION("Unexpected token", preprocessorToken);
		}

		bool PreprocessorParser::IsEndCondition(Token Token, EndConditions ConditionMask)
		{
			bool isTheEnd =
				(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::PreprocessorElse) && Token.Matches(PREPROCESSOR_ELSE, Token::SearchCases::CaseSensitive)) ||
				(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::PreprocessorEndIf) && Token.Matches(PREPROCESSOR_ENDIF, Token::SearchCases::CaseSensitive));

			if (!isTheEnd)
				return false;

			UngetToken(Token);

			return true;
		}
	}
}