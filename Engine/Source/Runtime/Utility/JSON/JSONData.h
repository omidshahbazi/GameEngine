// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef JSON_DATA_H
#define JSON_DATA_H

#include <Containers\AnyDataType.h>

namespace Engine
{
	using namespace Containers;

	namespace JSON
	{
		class JSONObject;
		class JSONArray;

		class JSON_API JSONData
		{
		public:
			enum class Types
			{
				Object,
				Array,
				Any
			};

		public:
			JSONData(void);
			JSONData(const JSONData& Other);
			JSONData(JSONObject* Object);
			JSONData(JSONArray* Array);
			JSONData(const AnyDataType& Data);
			~JSONData(void);

			JSONData& operator=(const JSONData& Other);
			JSONData& operator=(JSONObject* Object);
			JSONData& operator=(JSONArray* Array);
			JSONData& operator=(const AnyDataType& Data);

			bool operator==(const JSONData& Other);
			bool operator!=(const JSONData& Other);

			Types GetType(void) const
			{
				return m_Type;
			}

			JSONObject& GetObject(void)
			{
				return *m_Object;
			}
			const JSONObject& GetObject(void) const
			{
				return *m_Object;
			}

			JSONArray& GetArray(void)
			{
				return *m_Array;
			}
			const JSONArray& GetArray(void) const
			{
				return *m_Array;
			}

			const AnyDataType& GetAny(void) const
			{
				return m_Any;
			}

		private:
			Types m_Type;
			union
			{
				JSONObject* m_Object;
				JSONArray* m_Array;
				AnyDataType m_Any;
			};
		};
	}
}

#endif