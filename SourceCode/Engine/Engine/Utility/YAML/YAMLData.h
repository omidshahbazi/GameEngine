// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef YAML_DATA_H
#define YAML_DATA_H

#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Utility
	{
		namespace YAML
		{
			class YAMLObject;
			class YAMLArray;

			class UTILITY_API YAMLData
			{
			public:
				union Data
				{
				public:
					Data(void)
					{
						Int64 = 0;
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
					None = 0,
					Bool,
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
				YAMLData(void) :
					m_DataType(DataTypes::None)
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

				YAMLData(bool Value)
				{
					*this = Value;
				}

				YAMLData(int8 Value)
				{
					*this = Value;
				}

				YAMLData(int16 Value)
				{
					*this = Value;
				}

				YAMLData(int32 Value)
				{
					*this = Value;
				}

				YAMLData(const int64 &Value)
				{
					*this = Value;
				}

				YAMLData(float32 Value)
				{
					*this = Value;
				}

				YAMLData(const float64 &Value)
				{
					*this = Value;
				}

				YAMLData(cstr Value)
				{
					*this = Value;
				}

				YAMLData(const String &Value)
				{
					*this = Value;
				}

				YAMLData(YAMLObject *Value)
				{
					*this = Value;
				}

				YAMLData(YAMLArray *Value)
				{
					*this = Value;
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
					return *this;
				}

				INLINE YAMLData &operator=(bool Value)
				{
					m_Data.Bool = Value;
					m_DataType = DataTypes::Bool;
					return *this;
				}

				INLINE YAMLData &operator=(int8 Value)
				{
					m_Data.Int8 = Value;
					m_DataType = DataTypes::Int8;
					return *this;
				}

				INLINE YAMLData &operator=(int16 Value)
				{
					m_Data.Int16 = Value;
					m_DataType = DataTypes::Int16;
					return *this;
				}

				INLINE YAMLData &operator=(int32 Value)
				{
					m_Data.Int32 = Value;
					m_DataType = DataTypes::Int32;
					return *this;
				}

				INLINE YAMLData &operator=(const int64 &Value)
				{
					m_Data.Int64 = Value;
					m_DataType = DataTypes::Int64;
					return *this;
				}

				INLINE YAMLData &operator=(float32 Value)
				{
					m_Data.Float32 = Value;
					m_DataType = DataTypes::Float32;
					return *this;
				}

				INLINE YAMLData &operator=(const float64 &Value)
				{
					m_Data.Float64 = Value;
					m_DataType = DataTypes::Float64;
					return *this;
				}

				INLINE YAMLData &operator=(cstr Value)
				{
					m_Data.String = Value;
					m_DataType = DataTypes::String;
					return *this;
				}

				INLINE YAMLData &operator=(const String &Value)
				{
					m_Data.String = Value;
					m_DataType = DataTypes::String;
					return *this;
				}

				INLINE YAMLData &operator=(YAMLObject *Value)
				{
					m_Data.Object = Value;
					m_DataType = DataTypes::Object;
					return *this;
				}

				INLINE YAMLData &operator=(YAMLArray *Value)
				{
					m_Data.Array = Value;
					m_DataType = DataTypes::Array;
					return *this;
				}

				INLINE bool GetAsBool(void)
				{
					return m_Data.Bool;
				}

				INLINE int8 GetAsInt8(void)
				{
					return m_Data.Int8;
				}

				INLINE int16 GetAsInt16(void)
				{
					return m_Data.Int16;
				}

				INLINE int32 GetAsInt32(void)
				{
					return m_Data.Int32;
				}

				INLINE int64 GetAsInt64(void)
				{
					return m_Data.Int64;
				}

				INLINE float32 GetAsFloat32(void)
				{
					return m_Data.Float32;
				}

				INLINE float64 GetAsFloat64(void)
				{
					return m_Data.Float64;
				}

				INLINE String GetAsString(void)
				{
					return m_Data.String;
				}

				INLINE YAMLObject *GetAsObject(void)
				{
					return m_Data.Object;
				}

				INLINE YAMLArray *GetAsArray(void)
				{
					return m_Data.Array;
				}

				INLINE DataTypes GetType(void) const
				{
					return m_DataType;
				}

				void ToString(String &Result, int16 Indent) const;

			private:
				Data m_Data;
				DataTypes m_DataType;
			};
		}
	}
}

#endif