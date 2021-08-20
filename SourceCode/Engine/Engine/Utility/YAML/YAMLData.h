// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef YAML_DATA_H
#define YAML_DATA_H

#include <Containers\AnyDataType.h>

namespace Engine
{
	using namespace Containers;

	namespace YAML
	{
		class YAMLObject;
		class YAMLArray;

		class YAML_API YAMLData
		{
		public:
			union Data
			{
			public:
				Data(void) :
					Any()
				{
				}

				Data(const Data& Other) :
					Any()
				{
					*this = Other;
				}

				~Data(void)
				{
				}

				Data& operator=(const Data& Other)
				{
					Object = Other.Object;
					Array = Other.Array;
					Any = Other.Any;

					return *this;
				}

			public:
				YAMLObject* Object;
				YAMLArray* Array;
				AnyDataType Any;
			};

			enum class DataTypes
			{
				None = 0,
				Object,
				Array,
				Any
			};

		public:
			YAMLData(void) :
				m_DataType(DataTypes::None)
			{
			}

			YAMLData(const YAMLData& Other)
			{
				*this = Other;
			}

			YAMLData(const AnyDataType& Value)
			{
				*this = Value;
			}

			YAMLData(YAMLObject* Value)
			{
				*this = Value;
			}

			YAMLData(YAMLArray* Value)
			{
				*this = Value;
			}

			~YAMLData(void)
			{
				if (m_DataType == DataTypes::Object)
					Destruct(&m_Data.Object);
				else if (m_DataType == DataTypes::Array)
					Destruct(&m_Data.Array);
				else if (m_DataType == DataTypes::Any)
					Destruct(&m_Data.Any);
			}

			INLINE YAMLData& operator=(const YAMLData& Other)
			{
				m_Data = Other.m_Data;
				m_DataType = Other.m_DataType;
				return *this;
			}

			INLINE YAMLData& operator=(const AnyDataType& Value)
			{
				m_Data.Any = Value;
				m_DataType = DataTypes::Any;
				return *this;
			}

			INLINE YAMLData& operator=(YAMLObject* Value)
			{
				m_Data.Object = Value;
				m_DataType = DataTypes::Object;
				return *this;
			}

			INLINE YAMLData& operator=(YAMLArray* Value)
			{
				m_Data.Array = Value;
				m_DataType = DataTypes::Array;
				return *this;
			}

			INLINE const AnyDataType& GetAsAny(void) const
			{
				return m_Data.Any;
			}

			INLINE YAMLObject* GetAsObject(void) const
			{
				return m_Data.Object;
			}

			INLINE YAMLArray* GetAsArray(void) const
			{
				return m_Data.Array;
			}

			INLINE DataTypes GetType(void) const
			{
				return m_DataType;
			}

			void ToString(String& Result, int16 Indent) const;

		private:
			Data m_Data;
			DataTypes m_DataType;
		};
	}
}

#endif