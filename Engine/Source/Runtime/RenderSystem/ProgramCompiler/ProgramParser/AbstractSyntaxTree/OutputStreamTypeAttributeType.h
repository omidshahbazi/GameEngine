// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OUTPUT_STREAM_TYPE_ATTRIBUTE_TYPE_H
#define OUTPUT_STREAM_TYPE_ATTRIBUTE_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\BaseAttributeType.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API OutputStreamTypeAttributeType : public BaseAttributeType
			{
			public:
				enum class Types
				{
					Point = 0,
					Line,
					Triangle
				};

			public:
				void SetType(Types Type)
				{
					m_Type = Type;
				}

				Types GetType(void) const
				{
					return m_Type;
				}

				void SetDataType(const String& DataType)
				{
					m_DataType = DataType;
				}

				const String& GetDataType(void) const
				{
					return m_DataType;
				}

				virtual String ToString(void) const override
				{
					String result = "[OutputStreamType";

					result += "(";

					switch (m_Type)
					{
					case Types::Point:
						result += "Point";
						break;

					case Types::Line:
						result += "Line";
						break;

					case Types::Triangle:
						result += "Triangle";
						break;
					}

					result += ", ";

					result += m_DataType;

					result += ")]";

					return result;
				}

			private:
				Types m_Type;
				String m_DataType;
			};
		}
	}
}

#endif