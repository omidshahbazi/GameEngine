// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef CODE_PAGE_PARSER_H
#define CODE_PAGE_PARSER_H

#include <Lexer\Tokenizer.h>
#include <ReflectionGenerator\Specifiers.h>
#include <Reflection\Type.h>
#include <Reflection\DataType.h>

namespace Engine
{
	using namespace Containers;
	using namespace Reflection;
	using namespace Lexer;

	namespace ReflectionGenerator
	{
		const cstr CONST(STRINGIZE(const));
		const cstr CLASS(_CRT_STRINGIZE(class));
		const cstr STRUCT(STRINGIZE(struct));
		const cstr NAMESPACE(STRINGIZE(namespace));
		const cstr REFLECTION_OBJECT_TEXT(STRINGIZE(REFLECTION_OBJECT));
		const cstr REFLECTION_STRUCT_TEXT(STRINGIZE(REFLECTION_STRUCT));
		const cstr REFLECTION_ENUM_TEXT(STRINGIZE(REFLECTION_ENUM));
		const cstr REFLECTION_FUNCTION_TEXT(STRINGIZE(REFLECTION_FUNCTION));
		const cstr REFLECTION_PROPERTY_TEXT(STRINGIZE(REFLECTION_PROPERTY));

		class REFLECTIONGENERATOR_API CodePageParser : public Tokenizer
		{
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

			virtual bool MatchSymbol(const String& Match, SymbolParseOptions ParseTemplateCloseBracket = SymbolParseOptions::Normal);
			virtual bool RequireSymbol(const String& Match, const String& Tag, SymbolParseOptions ParseTemplateCloseBracket = SymbolParseOptions::Normal);

			virtual void ReadSpecifiers(Specifiers* Specifiers, const String& TypeName);

			virtual bool GetDataType(DataType& DataType);

			static ValueTypes ParseValueType(const String& Value);
		};
	}
}
#endif
