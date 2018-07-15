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
		void AddIndnt(String &Value, int16 Indent)
		{
			for (int i = 0; i < Indent; ++i)
				Value += '\t';
		}

		String YAMLData::ToString(int16 Indent) const
		{
			String result;

			switch (m_DataType)
			{
			case DataTypes::Bool:
				result += (m_Data.Bool ? "true" : "false");
				break;

			case DataTypes::Int8:
				result += to_string(m_Data.Int8).c_str();
				break;

			case DataTypes::Int16:
				result += to_string(m_Data.Int16).c_str();
				break;

			case DataTypes::Int32:
				result += to_string(m_Data.Int32).c_str();
				break;

			case DataTypes::Int64:
				result += to_string(m_Data.Int64).c_str();
				break;

			case DataTypes::Float32:
				result += to_string(m_Data.Float32).c_str();
				break;

			case DataTypes::Float64:
				result += to_string(m_Data.Float64).c_str();
				break;

			case DataTypes::String:
				result += String('"') + m_Data.String + '"';
				break;

			case DataTypes::Object:
				result += '\n';
				AddIndnt(result, Indent);
				result += m_Data.Object->ToString(Indent + 1);
				break;

			case DataTypes::Array:
				result += '\n';
				AddIndnt(result, Indent);
				result += m_Data.Array->ToString(Indent + 1);
				break;
			}

			return result;
		}
	}
}