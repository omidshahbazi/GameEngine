// Copyright 2012-2015 ?????????????. All Rights Reserved.
#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace ReflectionTool
	{
		const char8 NEWLINE('\n');
		const char8 RETURN('\r');
		const char8 TAB('\t');
		const char8 SPACE(' ');
		const char8 SLASH('/');
		const char8 BACK_SLASH('\\');
		const char8 STAR('*');
		const char8 AND('&');
		const char8 UNDERLINE('_');
		const char8 LOWER_A('a');
		const char8 LOWER_X('x');
		const char8 LOWER_Z('z');
		const char8 UPPER_A('A');
		const char8 UPPER_F('F');
		const char8 UPPER_X('X');
		const char8 UPPER_Z('Z');
		const char8 DOUBLE_QUOTATION('"');
		const char8 CHAR_0('0');
		const char8 CHAR_9('9');
		const char8 PLUS('+');
		const char8 MINES('-');
		const char8 EQUAL('=');
		const char8 DOT('.');
		const char8 COMMA(',');
		const char8 COLON(':');
		const char8 SEMI_COLON(';');
		const char8 SHARP('#');
		const char8 OPEN_BRACE('(');
		const char8 CLOSE_BRACE(')');
		const char8 OPEN_BRACKET('{');
		const char8 CLOSE_BRACKET('}');

		const String CONST(STRINGIZE(const));

		const String CLASS(_CRT_STRINGIZE(class));
		const String STRUCT(STRINGIZE(struct));
		const String REFLECTION_OBJECT_TEXT(STRINGIZE(REFLECTION_OBJECT));
		const String REFLECTION_ENUM_TEXT(STRINGIZE(REFLECTION_ENUM));
		const String REFLECTION_FUNCTION_TEXT(STRINGIZE(REFLECTION_FUNCTION));
		const String REFLECTION_PROPERTY_TEXT(STRINGIZE(REFLECTION_PROPERTY));
	}
}
#endif
