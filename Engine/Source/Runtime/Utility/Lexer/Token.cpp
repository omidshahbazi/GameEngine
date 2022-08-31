// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Lexer\Token.h>

namespace Engine
{
	namespace Lexer
	{
		Token::Token(void) :
			m_Type(Types::None),
			m_StartIndex(0),
			m_LineIndex(0),
			m_ColumnIndex(0)
		{
			PlatformMemory::Set(&m_String, 0, 1);
		}

		Token::Token(const Token& Token)
		{
			*this = Token;
		}

		Token::~Token(void)
		{
			Destruct(&m_String);
		}

		bool Token::Matches(const String& Name) const
		{
			return (m_Type == Types::Identifier && m_Name == Name);
		}

		bool Token::Matches(const String& Value, SearchCases SearchCase) const
		{
			return ((m_Type == Types::Identifier || m_Type == Types::Symbol) && ((SearchCase == SearchCases::CaseSensitive) ? m_Name == Value : m_Name.ToLower() == Value.ToLower()));
		}

		Token& Token::operator=(const Token& Token)
		{
			m_Type = Token.m_Type;
			m_Name = Token.m_Name;

			m_Float64 = Token.m_Float64;
			m_String = Token.m_String;

			m_StartIndex = Token.m_StartIndex;
			m_LineIndex = Token.m_LineIndex;
			m_ColumnIndex = Token.m_ColumnIndex;

			return *this;
		}
	}
}