// Copyright 2012-2015 ?????????????. All Rights Reserved.
#include <Utility\Lexer\Token.h>

namespace Engine
{
	namespace Utility
	{
		namespace Lexer
		{
			//http://stackoverflow.com/questions/12251545/how-do-i-implement-a-lexer-given-that-i-have-already-implemented-a-basic-regular

			Token::Token(void) :
				m_Type(Types::None)
			{
			}

			Token::Token(const Token & Token)
			{
				*this = Token;
			}

			bool Token::Matches(const String &Name) const
			{
				return (m_Type == Types::Identifier && m_Name == Name);
			}

			bool Token::Matches(const String &Value, SearchCases SearchCase) const
			{
				return ((m_Type == Types::Identifier || m_Type == Types::Symbol) && ((SearchCase == SearchCases::CaseSensitive) ? m_Identifier == Value : m_Identifier.ToLower() == Value.ToLower()));
			}

			void Token::SetConstantBool(bool Value)
			{
				m_Bool = Value;
				m_Type = Types::Constant;
			}

			void Token::SetConstantInt32(int32 Value)
			{
				m_Int32 = Value;
				m_Type = Types::Constant;
			}

			void Token::SetConstantFloat32(float32 Value)
			{
				m_Float32 = Value;
				m_Type = Types::Constant;
			}

			void Token::SetConstantFloat64(float64 Value)
			{
				m_Float64 = Value;
				m_Type = Types::Constant;
			}

			void Token::SetConstantString(const String &Value)
			{
				m_String = Value;
				m_Type = Types::Constant;
			}
			Token &Token::operator=(const Token & Token)
			{
				m_Type = Token.m_Type;
				m_Name = Token.m_Name;
				m_Identifier = Token.m_Identifier;

				m_StartIndex = Token.m_StartIndex;
				m_LineIndex = Token.m_LineIndex;

				m_Float64 = Token.m_Float64;

				return *this;
			}
		}
	}
}