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

				FillDebugInfo(Token);

				if (CharacterUtility::IsAlphabetic(c))
					return FillIdentifier(c, NoConst, Token);
				else if (CharacterUtility::IsDigit(c) || ((c == PLUS || c == MINES) && CharacterUtility::IsDigit(p)))
					return FillIntegralConstant(c, NoConst, Token);
				else if (c == DOUBLE_QUOTATION)
					return FillStringConstant(c, NoConst, Token);
				else
					return FillSymbol(c, NoConst, ParseTemplateCloseBracket == SymbolParseOptions::CloseTemplateBracket, Token);
			}

			void CodePageParser::RequireToken(Token& Token, const String& Tag, bool NoConst)
			{
				if (GetToken(Token, NoConst))
					return;

				ThrowRequiredException(Tag);
			}

			bool CodePageParser::MatchSymbol(const String& Match, SymbolParseOptions ParseTemplateCloseBracket)
			{
				Token token;
				if (!GetToken(token, false, ParseTemplateCloseBracket))
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
					RequireIdentifierToken(specifier, TypeName + " declaration specifier");

					Specifiers->AddSpecifier(specifier.GetName());
				}

				RequireSymbol(SEMICOLON, TypeName + " declaration specifier");
			}

			void CodePageParser::ParseParameters(MetaFunction* Function)
			{
				bool isFirstOne = true;
				while (true)
				{
					MetaDataType paramDataType;
					if (!ParseDataType(paramDataType))
						continue;

					if (!(paramDataType.GetValueType() == ValueTypes::Void && paramDataType.GetPassType() != DataType::PassesTypes::Pointer))
					{
						Token paramName;
						RequireIdentifierToken(paramName, Function->GetName() + " declaration specifier");

						MetaParameter parameter;
						parameter.SetDataType(paramDataType);
						parameter.SetName(paramName.GetName());

						Function->AddParameter(parameter);
					}

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

					if (MatchSymbol(OPEN_ANGLE_BRACKET))
					{
						DataType.SetIsTemplate(true);

						while (true)
						{
							MetaDataType templateParameterDataType;
							if (!ParseDataType(templateParameterDataType))
								return false;

							DataType.AddTemplateParameter(templateParameterDataType);

							if (MatchSymbol(COMMA))
								continue;

							if (MatchSymbol(CLOSE_ANGLE_BRACKET))
								break;
						}
					}

					ValueTypes valueType = ParseValueType(token.GetName());

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
						if (MatchSymbol(OPEN_BRACE) ||
							MatchSymbol(CLOSE_BRACE) ||
							MatchSymbol(SEMICOLON) ||
							MatchSymbol(COMMA) ||
							token.Matches(CLOSE_ANGLE_BRACKET, Token::SearchCases::CaseSensitive) ||
							!DataType.GetIsEmpty())
						{
							UngetToken(token);

							break;
						}

						DataType.SetExtraValueType(token.GetName());
					}
				}

				return !DataType.GetIsEmpty();
			}

			ValueTypes CodePageParser::ParseValueType(const String& Value)
			{
				ValueTypes type = ValueTypes::None;

				if (Value == "void")
					type = ValueTypes::Void;

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

			AccessSpecifiers CodePageParser::ParseAccessSpecifier(Token& Token)
			{
				if (Token.GetName() == "private")
					return AccessSpecifiers::Private;
				else if (Token.GetName() == "protected")
					return AccessSpecifiers::Protected;
				else if (Token.GetName() == "public")
					return AccessSpecifiers::Public;

				return AccessSpecifiers::None;
			}
		}
	}
}
