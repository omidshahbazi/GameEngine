// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Utility\YAML\YAMLData.h>
#include <Utility\YAML\YAMLObject.h>
#include <Utility\YAML\YAMLArray.h>
#include <string>

using namespace std;

namespace Engine
{
	namespace Utility
	{
		namespace YAML
		{
			void AppendIndent(String &Rresult, int16 Indent)
			{
				for (int i = 0; i < Indent; ++i)
					Rresult += '\t';
			}

			void YAMLData::ToString(String &Result, int16 Indent) const
			{
				switch (m_DataType)
				{
				case DataTypes::Bool:
					AppendIndent(Result, Indent);
					Result += (m_Data.Bool ? "true" : "false");
					break;

				case DataTypes::Int8:
					AppendIndent(Result, Indent);
					Result += to_string(m_Data.Int8).c_str();
					break;

				case DataTypes::Int16:
					AppendIndent(Result, Indent);
					Result += to_string(m_Data.Int16).c_str();
					break;

				case DataTypes::Int32:
					AppendIndent(Result, Indent);
					Result += to_string(m_Data.Int32).c_str();
					break;

				case DataTypes::Int64:
					AppendIndent(Result, Indent);
					Result += to_string(m_Data.Int64).c_str();
					break;

				case DataTypes::Float32:
					AppendIndent(Result, Indent);
					Result += to_string(m_Data.Float32).c_str();
					break;

				case DataTypes::Float64:
					AppendIndent(Result, Indent);
					Result += to_string(m_Data.Float64).c_str();
					break;

				case DataTypes::String:
					AppendIndent(Result, Indent);
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
}