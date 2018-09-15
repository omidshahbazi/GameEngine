// Copyright 2012-2015 ?????????????. All Rights Reserved.
#include <ReflectionTool\Token.h>

namespace Engine
{
	namespace ReflectionTool
	{
		//http://stackoverflow.com/questions/12251545/how-do-i-implement-a-lexer-given-that-i-have-already-implemented-a-basic-regular


		Token::Token(void) :
			PropertyType(nullptr),
			m_Type(Types::None)
		{
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
			(PropertyType&)*this = PropertyType(nullptr, ValueTypes::Bool);

			m_Bool = Value;
			m_Type = Types::Constant;
		}

		void Token::SetConstantInt32(int32 Value)
		{
			(PropertyType&)*this = PropertyType(nullptr, ValueTypes::Int32);

			m_Int32 = Value;
			m_Type = Types::Constant;
		}

		void Token::SetConstantFloat32(float32 Value)
		{
			(PropertyType&)*this = PropertyType(nullptr, ValueTypes::Float32);

			m_Float32 = Value;
			m_Type = Types::Constant;
		}

		void Token::SetConstantFloat64(float64 Value)
		{
			(PropertyType&)*this = PropertyType(nullptr, ValueTypes::Float64);

			m_Float64 = Value;
			m_Type = Types::Constant;
		}

		void Token::SetConstantString(const String &Value)
		{
			(PropertyType&)*this = PropertyType(nullptr, ValueTypes::String);

			m_String = Value;
			m_Type = Types::Constant;
		}
	}
}