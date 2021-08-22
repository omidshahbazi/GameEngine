// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ReflectionTool\CodePageParser.h>
#include <Common\CharacterUtility.h>
#include <Debugging\CoreDebug.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Common;
	using namespace Debugging;
	using namespace Reflection;
	using namespace Containers;

	namespace ReflectionTool
	{
		void CodePageParser::Parse(TypeList& Types)
		{
			m_CurrentIndex = 0;
			m_PrevIndex = 0;
			m_CurrentLineIndex = 0;
			m_PrevLineIndex = 0;
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

			Token.SetStartIndex(m_PrevIndex);
			Token.SetLineIndex(m_PrevLineIndex);

			if (IsAlphabetic(c))
			{
				do
				{
					Token.GetIdentifier() += c;
					c = GetChar();
				} while (IsAlphanumeric(c));

				UngetChar();

				Token.SetType(Token::Types::Identifier);
				Token.SetName(Token.GetName());

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
			else if (!NoConst && (IsDigit(c) || ((c == PLUS || c == MINES) && IsDigit(p))))
			{
				bool isFloat = false;
				bool isHex = false;

				do
				{
					if (c == DOT)
						isFloat = true;

					if (c == UPPER_X || c == LOWER_X)
						isHex = true;

					Token.GetIdentifier() += c;

					c = CharacterUtility::ToUpper(GetChar());

				} while (IsDigit(c) || (!isFloat && c == DOT) || (!isHex && c == UPPER_X) || (isHex && c >= UPPER_A && c <= UPPER_F));

				if (isFloat || c != UPPER_F)
					UngetChar();

				if (isFloat)
					Token.SetConstantFloat32(StringUtility::ToFloat32(Token.GetIdentifier()));
				else if (isHex)
					Token.SetConstantInt32(StringUtility::ToInt32(Token.GetIdentifier()));
				else
					Token.SetConstantInt32(StringUtility::ToInt32(Token.GetIdentifier()));

				return true;
			}
			else if (c == DOUBLE_QUOTATION)
			{
				String temp;
				c = GetChar(true);
				while (c != DOUBLE_QUOTATION && !IsEOL(c))
				{
					if (c == BACK_SLASH)
					{
						c = GetChar(true);

						if (IsEOL(c))
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
				Token.GetIdentifier() += c;

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
					Token.GetIdentifier() += d;

					if (c == '>' && d == '>')
					{
						if (GetChar() == '>')
							Token.GetIdentifier() += '>';
						else
							UngetChar();
					}
				}
				else
					UngetChar();

#undef PAIR

				Token.SetType(Token::Types::Symbol);

				Token.SetName(Token.GetIdentifier());

				return true;
			}
		}

		bool CodePageParser::RequireSymbol(const String& Match, const String& Tag, SymbolParseOptions ParseTemplateCloseBracket)
		{
			if (MatchSymbol(Match, ParseTemplateCloseBracket))
				return true;

			CoreDebugLogError(Categories::ReflectionTool, (TEXT("Missing '") + Match + "' in " + Tag).GetValue());

			return false;
		}

		bool CodePageParser::MatchSymbol(const String& Match, SymbolParseOptions ParseTemplateCloseBracket)
		{
			Token token;

			if (GetToken(token, true, ParseTemplateCloseBracket))
				if (token.GetTokenType() == Token::Types::Symbol && token.Matches(Match, Token::SearchCases::CaseSensitive))
					return true;
				else
					UngetToken(token);

			return false;
		}

		bool CodePageParser::MatchSemiColon(void)
		{
			if (MatchSymbol(SEMICOLON))
				return true;

			Token token;
			if (GetToken(token))
				CoreDebugLogError(Categories::ReflectionTool, (TEXT("Missing ';' before '") + token.GetIdentifier() + "'").GetValue());
			else
				CoreDebugLogError(Categories::ReflectionTool, "Missing ';'");

			return false;
		}

		void CodePageParser::ReadSpecifiers(Specifiers* Specifiers, const String& TypeName)
		{
			uint16 specifiersCount = 0;
			String error = TypeName + " declaration specifier";

			RequireSymbol(OPEN_BRACE, error);

			while (!MatchSymbol(CLOSE_BRACE))
			{
				if (specifiersCount > 0)
					RequireSymbol(COMMA, error);

				specifiersCount++;

				Token specifier;
				if (!GetToken(specifier))
					CoreDebugLogError(Categories::ReflectionTool, (TEXT("Expected ") + error).GetValue());

				Specifiers->AddSpecifier(specifier.GetIdentifier());
			}
		}

		bool CodePageParser::GetDataType(DataType& DataType)
		{
			while (true)
			{
				Token token;
				GetToken(token);

				ValueTypes valueType = ParseValueType(token.GetIdentifier());
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
					if (MatchSymbol(OPEN_BRACE) || MatchSymbol(CLOSE_BRACE) || MatchSymbol(SEMICOLON) || MatchSymbol(COMMA))
					{
						UngetToken(token);
						break;
					}
					else
						DataType.SetExtraValueType(token.GetIdentifier());
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
