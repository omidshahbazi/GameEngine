// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Utility\YAMLData.h>

namespace Engine
{
	namespace Utility
	{
		String YAMLData::ToString(void) const
		{
			switch (m_DataType)
			{
			case DataTypes::Bool:
				return (m_Data.Bool ? "true" : "false");
			case DataTypes::Int8:
				return (m_Data.Bool ? "true" : "false");
			case DataTypes::Int16:
				return (m_Data.Bool ? "true" : "false");
			case DataTypes::Int32:
				return (m_Data.Bool ? "true" : "false");
			case DataTypes::Int64:
				return (m_Data.Bool ? "true" : "false");
			case DataTypes::Float32:
				return (m_Data.Bool ? "true" : "false");
			case DataTypes::Float64:
				return (m_Data.Bool ? "true" : "false");
			case DataTypes::String:
				return (m_Data.Bool ? "true" : "false");
			case DataTypes::Object:
				return (m_Data.Bool ? "true" : "false");
			case DataTypes::Array:
				return (m_Data.Bool ? "true" : "false");
			}
		}
	}
}