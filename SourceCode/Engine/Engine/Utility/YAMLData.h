// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef YAML_DATA_H
#define YAML_DATA_H

#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Utility
	{
		class YAMLObject;
		class YAMLArray;

		union YAMLData
		{
		public:
			union Data
			{
			public:
				Data(void)
				{
				}

				~Data(void)
				{
				}

			public:
				bool Bool;
				int8 Int8;
				int16 Int16;
				int32 Int32;
				int64 Int64;
				float32 Float32;
				float64 Float64;
				String String;
				YAMLObject *Object;
				YAMLArray *Array;
			};

			enum class DataTypes
			{
				Bool = 0,
				Int8,
				Int16,
				Int32,
				Int64,
				Float32,
				Float64,
				String,
				Object,
				Array
			};

		public:
			YAMLData(void)
			{
			}

			YAMLData(const YAMLData &Other)
			{
				*this = Other;
			}

			YAMLData(YAMLData &&Other)
			{
				*this = Other;
			}

			~YAMLData(void)
			{
			}

			INLINE YAMLData &operator=(const YAMLData &Other)
			{
				m_Data.Int64 = Other.m_Data.Int64;
				m_DataType = Other.m_DataType;
				return *this;
			}

			INLINE YAMLData &operator=(YAMLData &&Other)
			{
				*this = Other;
			}

			INLINE YAMLData &operator=(bool Value)
			{
				m_Data.Bool == Value;
				m_DataType = DataTypes::Bool;
				return *this;
			}

			String ToString(void) const;

		private:
			Data m_Data;
			DataTypes m_DataType;
		};
	};
}

#endif