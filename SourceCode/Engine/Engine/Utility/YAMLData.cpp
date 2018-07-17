// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Utility\YAMLData.h>
#include <Utility\YAMLObject.h>
#include <Utility\YAMLArray.h>
#include <string>

using namespace std;

namespace Engine
{
	namespace Utility
	{
		void YAMLData::ToString(String &Result, int16 Indent) const
		{
			switch (m_DataType)
			{
			case DataTypes::Bool:
				Result += (m_Data.Bool ? "true" : "false");
				break;

			case DataTypes::Int8:
				Result += to_string(m_Data.Int8).c_str();
				break;

			case DataTypes::Int16:
				Result += to_string(m_Data.Int16).c_str();
				break;

			case DataTypes::Int32:
				Result += to_string(m_Data.Int32).c_str();
				break;

			case DataTypes::Int64:
				Result += to_string(m_Data.Int64).c_str();
				break;

			case DataTypes::Float32:
				Result += to_string(m_Data.Float32).c_str();
				break;

			case DataTypes::Float64:
				Result += to_string(m_Data.Float64).c_str();
				break;

			case DataTypes::String:
				Result += String('"') + m_Data.String + '"';
				break;

			case DataTypes::Object:
				Result += m_Data.Object->ToString(Indent);
				break;

			case DataTypes::Array:
				Result += m_Data.Array->ToString(Indent);
				break;
			}
		}
	}
}