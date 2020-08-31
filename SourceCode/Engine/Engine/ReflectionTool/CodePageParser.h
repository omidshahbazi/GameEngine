// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef CODE_PAGE_PARSER_H
#define CODE_PAGE_PARSER_H

#include <Utility\Lexer\Tokenizer.h>
#include <ReflectionTool\Specifiers.h>
#include <Reflection\Type.h>
#include <Reflection\DataType.h>

namespace Engine
{
	using namespace Containers;
	using namespace Reflection;
	using namespace Utility::Lexer;

	namespace ReflectionTool
	{
		enum class SymbolParseOptions
		{
			Normal = 0,
			CloseTemplateBracket
		};

		const str CONST(STRINGIZE(const));
		const str CLASS(_CRT_STRINGIZE(class));
		const str STRUCT(STRINGIZE(struct));
		const str NAMESPACE(STRINGIZE(namespace));
		const str REFLECTION_OBJECT_TEXT(STRINGIZE(REFLECTION_OBJECT));
		const str REFLECTION_STRUCT_TEXT(STRINGIZE(REFLECTION_STRUCT));
		const str REFLECTION_ENUM_TEXT(STRINGIZE(REFLECTION_ENUM));
		const str REFLECTION_FUNCTION_TEXT(STRINGIZE(REFLECTION_FUNCTION));
		const str REFLECTION_PROPERTY_TEXT(STRINGIZE(REFLECTION_PROPERTY));

		class REFLECTIONTOOL_API CodePageParser : public Tokenizer
		{
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

			virtual bool RequireSymbol(const String& Match, const String& Tag, SymbolParseOptions ParseTemplateCloseBracket = SymbolParseOptions::Normal);
			virtual bool MatchSymbol(const String& Match, SymbolParseOptions ParseTemplateCloseBracket = SymbolParseOptions::Normal);

			virtual bool MatchSemiColon(void);

			virtual void ReadSpecifiers(Specifiers* Specifiers, const String& TypeName);

			virtual bool GetDataType(DataType& DataType);

			static ValueTypes ParseValueType(const String& Value);
		};
	}
}
#endif
