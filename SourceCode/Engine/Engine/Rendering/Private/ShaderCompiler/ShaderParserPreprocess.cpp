// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderCompiler\ShaderParserPreprocess.h>
#include <Common\BitwiseUtils.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Utility::Lexer;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				using namespace Syntax;

				cstr PREPROCESSOR_INCLUDE = "include";
				cstr PREPROCESSOR_DEFINE = "define";
				cstr PREPROCESSOR_UNDEF = "undef";
				cstr PREPROCESSOR_IFDEF = "ifdef";
				cstr PREPROCESSOR_IFNDEF = "ifndef";
				cstr PREPROCESSOR_ELSE = "else";
				cstr PREPROCESSOR_ENDIF = "endif";

				bool IsDefined(const ShaderInfo::DefineList& Defines, const String& Define)
				{
					for (auto& define : Defines)
					{
						if (define.Name == Define)
							return true;
					}

					return false;
				}

				ShaderParserPreprocess::ShaderParserPreprocess(const String& Text) :
					Tokenizer(Text)
				{
				}

				bool ShaderParserPreprocess::Process(Parameters& Parameters)
				{
					Tokenizer::Parse();

					return Process(Parameters, EndConditions::None);
				}

				bool ShaderParserPreprocess::Process(Parameters& Parameters, EndConditions ConditionMask)
				{
#define ADD_TO_RESULT(Value) Parameters.Result += ' ' + Value

					while (true)
					{
						Token token;
						if (!GetToken(token))
							return true;

						if (IsEndCondition(token, ConditionMask))
						{
							UngetToken(token);
							return true;
						}
						else if (token.Matches(SHARP, Token::SearchCases::CaseSensitive))
						{
							Token preprocessorCommandToken;
							if (!GetToken(preprocessorCommandToken))
								return true;

							if (IsEndCondition(preprocessorCommandToken, ConditionMask))
							{
								UngetToken(token);
								return true;
							}
							else
								UngetToken(preprocessorCommandToken);
						}

						ParseResults result = ParseResults::Failed;

						if ((result = ParsePreprocessor(token, Parameters)) == ParseResults::Approved)
							continue;
						else if (result == ParseResults::Failed)
							return false;

						switch (token.GetTokenType())
						{
						case Token::Types::Identifier:
						case Token::Types::Symbol:
							ADD_TO_RESULT(token.GetIdentifier());
							break;

						case Token::Types::Constant:
						{
							if (token.GetIdentifier() == "true")
								ADD_TO_RESULT(token.GetIdentifier());
							else if (token.GetIdentifier() == "false")
								ADD_TO_RESULT(token.GetIdentifier());
							else if (token.GetIdentifier().Contains("."))
								ADD_TO_RESULT(StringUtility::ToString<char8>(token.GetConstantFloat32()));
							else
								ADD_TO_RESULT(StringUtility::ToString<char8>(token.GetConstantInt32()));
						}
						break;
						}
					}

#undef ADD_TO_RESULT

					return true;
				}

				ShaderParserPreprocess::ParseResults ShaderParserPreprocess::ParsePreprocessor(Token& DeclarationToken, Parameters& Parameters)
				{
					if (!DeclarationToken.Matches(SHARP, Token::SearchCases::CaseSensitive))
						return ParseResults::Rejected;

					Token preprocessorToken;
					if (!GetToken(preprocessorToken))
						return ParseResults::Failed;

					if (preprocessorToken.Matches(PREPROCESSOR_INCLUDE, Token::SearchCases::CaseSensitive))
					{
						Token openBracketToken;
						if (!GetToken(openBracketToken) || !openBracketToken.Matches(OPEN_ANGLE_BRACKET, Token::SearchCases::CaseSensitive))
							return ParseResults::Failed;

						String fileName;
						while (true)
						{
							Token token;
							if (!GetToken(token))
								return ParseResults::Failed;

							if (token.Matches(CLOSE_ANGLE_BRACKET, Token::SearchCases::CaseSensitive))
								break;

							fileName += token.GetIdentifier();
						}

						String source;
						if (!Parameters.IncludeFunction(fileName, source))
							return ParseResults::Failed;

						ShaderParserPreprocess parser(source);
						parser.Process(Parameters);

						return ParseResults::Approved;
					}
					else if (preprocessorToken.Matches(PREPROCESSOR_DEFINE, Token::SearchCases::CaseSensitive))
					{
						Token nameToken;
						if (!GetToken(nameToken))
							return ParseResults::Failed;

						bool isDuplicate = false;
						for (const auto& define : Parameters.Defines)
							if (define.Name == nameToken.GetIdentifier())
							{
								isDuplicate = true;
								break;
							}

						if (!isDuplicate)
							Parameters.Defines.Add({ nameToken.GetIdentifier() });

						return ParseResults::Approved;
					}
					else if (preprocessorToken.Matches(PREPROCESSOR_UNDEF, Token::SearchCases::CaseSensitive))
					{
						Token nameToken;
						if (!GetToken(nameToken))
							return ParseResults::Failed;

						for (uint32 i = 0; i < Parameters.Defines.GetSize(); ++i)
							if (Parameters.Defines[i].Name == nameToken.GetIdentifier())
								Parameters.Defines.RemoveAt(i--);

						return ParseResults::Approved;
					}
					else if (preprocessorToken.Matches(PREPROCESSOR_IFDEF, Token::SearchCases::CaseSensitive) || preprocessorToken.Matches(PREPROCESSOR_IFNDEF, Token::SearchCases::CaseSensitive))
					{
						Token nameToken;
						if (!GetToken(nameToken))
							return ParseResults::Failed;

						bool shouldRemoveBlock = (IsDefined(Parameters.Defines, nameToken.GetIdentifier()) == preprocessorToken.Matches(PREPROCESSOR_IFNDEF, Token::SearchCases::CaseSensitive));

						if (ParsePreprocessorBlock(Parameters, shouldRemoveBlock) == ParseResults::Failed)
							return ParseResults::Failed;

						Token sharpToken;
						if (!GetToken(sharpToken))
							return ParseResults::Failed;

						if (!sharpToken.Matches(SHARP, Token::SearchCases::CaseSensitive))
							return ParseResults::Failed;

						Token preprocessorToken;
						if (!GetToken(preprocessorToken))
							return ParseResults::Failed;

						if (preprocessorToken.Matches(PREPROCESSOR_ELSE, Token::SearchCases::CaseSensitive))
							if (ParsePreprocessorBlock(Parameters, !shouldRemoveBlock) == ParseResults::Failed)
								return ParseResults::Failed;

						return ParseResults::Approved;
					}

					return ParseResults::Failed;
				}

				ShaderParserPreprocess::ParseResults ShaderParserPreprocess::ParsePreprocessorBlock(Parameters& Parameters, bool ShouldRemove)
				{
					if (ShouldRemove)
					{
						Token prevToken;

						int32 openBlockCount = 1;
						while (true)
						{
							Token token;
							if (!GetToken(token))
								return ParseResults::Failed;

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

						return ParseResults::Approved;
					}

					Process(Parameters, EndConditions::PreprocessorElse | EndConditions::PreprocessorEndIf);

					Token sharpToken;
					if (!GetToken(sharpToken))
						return ParseResults::Failed;

					Token preprocessorToken;
					if (!GetToken(preprocessorToken))
						return ParseResults::Failed;

					if (preprocessorToken.Matches(PREPROCESSOR_ELSE, Token::SearchCases::CaseSensitive))
					{
						UngetToken(sharpToken);
						return ParseResults::Approved;
					}

					if (preprocessorToken.Matches(PREPROCESSOR_ENDIF, Token::SearchCases::CaseSensitive))
						return ParseResults::Approved;

					return ParseResults::Failed;
				}

				bool ShaderParserPreprocess::IsEndCondition(Token Token, ShaderParserPreprocess::EndConditions ConditionMask)
				{
					return
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::PreprocessorElse) && Token.Matches(PREPROCESSOR_ELSE, Token::SearchCases::CaseSensitive)) ||
						(BitwiseUtils::IsEnabled(ConditionMask, EndConditions::PreprocessorEndIf) && Token.Matches(PREPROCESSOR_ENDIF, Token::SearchCases::CaseSensitive));
				}
			}
		}
	}
}