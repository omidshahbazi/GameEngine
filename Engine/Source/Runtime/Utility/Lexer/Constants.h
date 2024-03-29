// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace Lexer
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
		const cstr DOUBLE_COLON("::");
		const char8 COLON(':');
		const char8 SEMICOLON(';');
		const char8 SHARP('#');
		const char8 OPEN_BRACE('(');
		const char8 CLOSE_BRACE(')');
		const char8 OPEN_BRACKET('{');
		const char8 CLOSE_BRACKET('}');
		const char8 OPEN_SQUARE_BRACKET('[');
		const char8 CLOSE_SQUARE_BRACKET(']');
		const char8 OPEN_ANGLE_BRACKET('<');
		const char8 CLOSE_ANGLE_BRACKET('>');
		const char8 EXLAMATION('!');
		const char8 TILDE('~');
	}
}
#endif
