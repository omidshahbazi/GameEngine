// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef CODE_PAGE_PARSER_H
#define CODE_PAGE_PARSER_H

#include <Lexer\Tokenizer.h>
#include <ReflectionGenerator\Private\MetaDataType.h>
#include <ReflectionGenerator\Private\MetaFunction.h>

namespace Engine
{
	using namespace Containers;
	using namespace Reflection;
	using namespace Lexer;

	namespace ReflectionGenerator
	{
		namespace Private
		{
			const cstr NAMESPACE(STRINGIZE(namespace));
			const cstr ENUM(_CRT_STRINGIZE(enum));
			const cstr CLASS(_CRT_STRINGIZE(class));
			const cstr STRUCT(STRINGIZE(struct));
			const cstr STATIC(STRINGIZE(static));
			const cstr CONST(STRINGIZE(const));
			const cstr REFLECTION_OBJECT_TEXT(STRINGIZE(REFLECTION_OBJECT));
			const cstr REFLECTION_ENUM_TEXT(STRINGIZE(REFLECTION_ENUM));
			const cstr REFLECTION_FUNCTION_TEXT(STRINGIZE(REFLECTION_FUNCTION));
			const cstr REFLECTION_PROPERTY_TEXT(STRINGIZE(REFLECTION_PROPERTY));
			const cstr REFLECTION_ABSTRACT_TEXT(STRINGIZE(REFLECTION_ABSTRACT));

			class REFLECTIONGENERATOR_API CodePageParser : public Tokenizer
			{
			public:
			protected:
				enum class SymbolParseOptions
				{
					Normal = 0,
					CloseTemplateBracket
				};

			public:
				CodePageParser(const String& Text) :
					Tokenizer(Text)
				{
				}
				virtual ~CodePageParser(void)
				{
				}

				virtual void Parse(TypeList& Types);

			protected:
				virtual bool GetToken(Token& Token, bool NoConst = false, SymbolParseOptions ParseTemplateCloseBracket = SymbolParseOptions::Normal);

				virtual void RequireToken(Token& Token, const String& Tag, bool NoConst = false);

				virtual bool MatchSymbol(const String& Match, SymbolParseOptions ParseTemplateCloseBracket = SymbolParseOptions::Normal);
				virtual bool RequireSymbol(const String& Match, const String& Tag, SymbolParseOptions ParseTemplateCloseBracket = SymbolParseOptions::Normal);

				virtual void ParseSpecifiers(Specifiers* Specifiers, const String& TypeName);

				virtual void ParseParameters(MetaFunction* Function);

				virtual bool ParseDataType(MetaDataType& DataType);

				static ValueTypes ParseValueType(const String& Value);

				virtual AccessSpecifiers ParseAccessSpecifier(Token& Token);
			};
		}
	}
}
#endif
