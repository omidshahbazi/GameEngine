// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ReflectionGenerator\Private\CodePageParser.h>
#include <ReflectionGenerator\ReflectionGeneratorException.h>
#include <ReflectionGenerator\Private\MetaParameter.h>
#include <Common\CharacterUtility.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Common;
	using namespace Reflection;
	using namespace Containers;

	namespace ReflectionGenerator
	{
		namespace Private
		{
			void CodePageParser::Parse(TypeList& Types)
			{
				Reset();
			}

			bool CodePageParser::GetToken(Token& Token, bool NoConst, SymbolParseOptions ParseTemplateCloseBracket)
			{
				char8 c = GetLeadingChar();
				char8 p = PeekChar();

				if (c == '\0')
				{
					UngetChar();
					return false;
				}

				Token.SetStartIndex(GetPrevIndex());
				Token.SetLineIndex(GetPrevLineIndex());

				String name = Token.GetName();

				if (CharacterUtility::IsAlphabetic(c))
				{
					do
					{
						name += c;
						c = GetChar();
					} while (CharacterUtility::IsAlphanumeric(c));

					UngetChar();

					Token.SetIdentifier(name);

					if (!NoConst)
					{
						if (Token.Matches("true"))
						{
							Token.SetConstantBool(true);
							return true;
						}
						else if (Token.Matches("false"))
						{
							Token.SetConstantBool(false);
							return true;
						}
					}

					return true;
				}
				else if (CharacterUtility::IsDigit(c) || ((c == PLUS || c == MINES) && CharacterUtility::IsDigit(p)))
				{
					bool isFloat = false;
					bool isHex = false;

					do
					{
						if (c == DOT)
							isFloat = true;

						if (c == UPPER_X || c == LOWER_X)
							isHex = true;

						name += c;

						c = CharacterUtility::ToUpper(GetChar());

					} while (CharacterUtility::IsDigit(c) || (!isFloat && c == DOT) || (!isHex && c == UPPER_X) || (isHex && c >= UPPER_A && c <= UPPER_F));

					if (isFloat || c != UPPER_F)
						UngetChar();

					if (isFloat)
						Token.SetConstantFloat32(StringUtility::ToFloat32(name));
					else if (isHex)
						Token.SetConstantInt32(StringUtility::ToInt32(name));
					else
						Token.SetConstantInt32(StringUtility::ToInt32(name));

					return true;
				}
				else if (c == DOUBLE_QUOTATION)
				{
					String temp;
					c = GetChar(true);
					while (c != DOUBLE_QUOTATION && !CharacterUtility::IsEOL(c))
					{
						if (c == BACK_SLASH)
						{
							c = GetChar(true);

							if (CharacterUtility::IsEOL(c))
								break;
							else if (c == 'n')
								c = '\n';
						}

						temp += c;

						c = GetChar(true);
					}

					Token.SetConstantString(temp);

					return true;
				}
				else
				{
					name += c;

#define PAIR(cc, dd) (c == cc && d == dd)

					char8 d = GetChar();

					if (PAIR('<', '<') ||
						(PAIR('>', '>') && ParseTemplateCloseBracket != SymbolParseOptions::CloseTemplateBracket) ||
						PAIR('=', '=') ||
						PAIR('!', '=') ||
						PAIR('<', '=') ||
						PAIR('>', '=') ||
						PAIR('+', '+') ||
						PAIR('-', '-') ||
						PAIR('|', '|') ||
						PAIR('^', '^') ||
						PAIR('&', '&') ||
						PAIR('+', '=') ||
						PAIR('-', '=') ||
						PAIR('*', '=') ||
						PAIR('/', '=') ||
						PAIR('~', '=') ||
						PAIR(':', ':') ||
						PAIR('*', '*'))
					{
						name += d;

						if (c == '>' && d == '>')
						{
							if (GetChar() == '>')
								name += '>';
							else
								UngetChar();
						}
					}
					else
						UngetChar();

#undef PAIR

					Token.SetSymbol(name);

					return true;
				}
			}

			bool CodePageParser::MatchSymbol(const String& Match, SymbolParseOptions ParseTemplateCloseBracket)
			{
				Token token;
				if (!GetToken(token, true, ParseTemplateCloseBracket))
					return false;

				if (token.GetType() == Token::Types::Symbol && token.Matches(Match, Token::SearchCases::CaseSensitive))
					return true;

				UngetToken(token);

				return false;
			}

			bool CodePageParser::RequireSymbol(const String& Match, const String& Tag, SymbolParseOptions ParseTemplateCloseBracket)
			{
				if (MatchSymbol(Match, ParseTemplateCloseBracket))
					return true;

				ThrowMissingException(Match, Tag);

				return false;
			}

			void CodePageParser::ParseSpecifiers(Specifiers* Specifiers, const String& TypeName)
			{
				RequireSymbol(OPEN_BRACE, TypeName + " declaration specifier");

				bool isFirstOne = true;
				while (!MatchSymbol(CLOSE_BRACE))
				{
					if (!isFirstOne)
						RequireSymbol(COMMA, TypeName + " declaration specifier");
					isFirstOne = false;

					Token specifier;
					RequireIdentifierToken(specifier);

					Specifiers->AddSpecifier(specifier.GetName());
				}
			}

			void CodePageParser::ParseParameters(MetaFunction* Function)
			{
				bool isFirstOne = true;
				while (true)
				{
					MetaDataType paramDataType;
					if (!ParseDataType(paramDataType))
						continue;

					Token paramName;
					RequireIdentifierToken(paramName);

					MetaParameter parameter;
					parameter.SetDataType(paramDataType);
					parameter.SetName(paramName.GetName());

					Function->AddParameter(parameter);

					while (true)
					{
						if (MatchSymbol(COMMA))
							break;

						if (MatchSymbol(CLOSE_BRACE))
							return;

						Token token;
						GetToken(token);
					}
				}
			}

			bool CodePageParser::ParseDataType(MetaDataType& DataType)
			{
				while (true)
				{
					Token token;
					GetToken(token);

					ValueTypes valueType = ParseValueType(token.GetName());

					if (token.Matches(CLOSE_BRACE, Token::SearchCases::CaseSensitive))
						return false;

					if (valueType != ValueTypes::None)
						DataType.SetValueType(valueType);
					else if (token.Matches(STAR, Token::SearchCases::CaseSensitive))
					{
						DataType.SetPassType(DataType::PassesTypes::Pointer);
						break;
					}
					else if (token.Matches(AND, Token::SearchCases::CaseSensitive))
					{
						DataType.SetPassType(DataType::PassesTypes::Reference);
						break;
					}
					else if (token.Matches(CONST, Token::SearchCases::CaseSensitive))
					{
						if (DataType.GetValueType() == ValueTypes::None)
							DataType.SetIsConst(true);
						else
							DataType.SetIsConstValue(true);
					}
					else
					{
						if (MatchSymbol(OPEN_BRACE) || MatchSymbol(CLOSE_BRACE) || MatchSymbol(SEMICOLON) || MatchSymbol(COMMA))
							break;
						else
							DataType.SetExtraValueType(token.GetName());
					}
				}

				return true;
			}

			ValueTypes CodePageParser::ParseValueType(const String& Value)
			{
				ValueTypes type = ValueTypes::None;

				if (Value == "void")
					type = ValueTypes::VoidPointer;

				if (Value == "bool")
					type = ValueTypes::Bool;

				else if (Value == "uint8" || Value == "unsinged char")
					type = ValueTypes::UInt8;
				else if (Value == "uint16" || Value == "unsinged short")
					type = ValueTypes::UInt16;
				else if (Value == "uint32" || Value == "unsinged int")
					type = ValueTypes::UInt32;
				else if (Value == "uint64" || Value == "unsinged long")
					type = ValueTypes::UInt64;

				else if (Value == "int8" || Value == "char")
					type = ValueTypes::Int8;
				else if (Value == "int16" || Value == "short")
					type = ValueTypes::Int16;
				else if (Value == "int32" || Value == "int")
					type = ValueTypes::Int32;
				else if (Value == "int64" || Value == "long")
					type = ValueTypes::Int64;

				else if (Value == "float32" || Value == "float")
					type = ValueTypes::Float32;
				else if (Value == "float64" || Value == "double")
					type = ValueTypes::Float64;

				else if (Value == "String")
					type = ValueTypes::String;
				//else if (Value == "Vector2")
				//	type = ValueTypes::Vector2;
				//else if (Value == "Vector3")
				//	type = ValueTypes::Vector3;
				//else if (Value == "Matrix4")
				//	type = ValueTypes::Matrix4;

				return type;
			}
		}
	}
}
