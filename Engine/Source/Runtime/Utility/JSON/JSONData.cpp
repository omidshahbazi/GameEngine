// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <JSON\JSONData.h>

namespace Engine
{
	namespace JSON
	{
		JSONData::JSONData(void) :
			m_Type(Types::Any)
		{
		}

		JSONData::JSONData(const JSONData& Other)
		{
			*this = Other;
		}

		JSONData::JSONData(JSONObject* Object)
		{
			*this = Object;
		}

		JSONData::JSONData(JSONArray* Array)
		{
			*this = Array;
		}

		JSONData::JSONData(const AnyDataType& Data)
		{
			*this = Data;
		}

		JSONData::~JSONData(void)
		{
			if (m_Type == Types::Any)
				Destruct(&m_Any);
		}

		JSONData& JSONData::operator=(const JSONData& Other)
		{
			m_Type = Other.m_Type;

			switch (m_Type)
			{
			case JSONData::Types::Object: m_Object = Other.m_Object; break;

			case JSONData::Types::Array: m_Array = Other.m_Array; break;

			case JSONData::Types::Any:
				Construct(&m_Any);
				m_Any = Other.m_Any;
				break;
			}

			return *this;
		}

		JSONData& JSONData::operator=(JSONObject* Object)
		{
			m_Type = Types::Object;
			m_Object = Object;

			return *this;
		}

		JSONData& JSONData::operator=(JSONArray* Array)
		{
			m_Type = Types::Array;
			m_Array = Array;

			return *this;
		}

		JSONData& JSONData::operator=(const AnyDataType& Data)
		{
			Construct(&m_Any);

			m_Type = Types::Any;
			m_Any = Data;

			return *this;
		}

		bool JSONData::operator==(const JSONData& Other)
		{
			if (m_Type != Other.m_Type)
				return false;

			if (m_Type == Types::Object && m_Object != Other.m_Object)
				return false;

			if (m_Type == Types::Array && m_Array != Other.m_Array)
				return false;

			if (m_Type == Types::Any && m_Any != Other.m_Any)
				return false;

			return true;
		}

		bool JSONData::operator!=(const JSONData& Other)
		{
			return !(*this == Other);
		}
	}
}