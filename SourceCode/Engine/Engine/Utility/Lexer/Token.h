// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef TOKEN_H
#define TOKEN_H

#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Utility
	{
		namespace Lexer
		{
			class UTILITY_API Token
			{
			public:
				enum class Types
				{
					Literal = 0,
					String,
					Digit,
					Sign,
					Whitespace,
					End
				};

			public:
				Token(Types Type, const String &Value, uint16 Column, uint16 Line) :
					m_Type(Type),
					m_Value(Value),
					m_Column(Column),
					m_Line(Line)
				{
				}

				Types GetType(void) const
				{
					return m_Type;
				}

				const String &GetValue(void) const
				{
					return m_Value;
				}

			private:
				Types m_Type;
				String m_Value;
				uint16 m_Column;
				uint16 m_Line;
			};
		}
	}
}

#endif