// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <YAML\YAMLData.h>
#include <YAML\YAMLObject.h>
#include <YAML\YAMLArray.h>
#include <Containers\StringUtility.h>

using namespace std;

namespace Engine
{
	namespace YAML
	{
		void AppendIndent(String& Rresult, int16 Indent)
		{
			for (int i = 0; i < Indent; ++i)
				Rresult += '\t';
		}

		void YAMLData::ToString(String& Result, int16 Indent) const
		{
			if (m_DataType == DataTypes::Any)
			{
				switch (m_Data.Any.GetValueType())
				{
				case ValueTypes::Bool:
					AppendIndent(Result, Indent);
					Result += (m_Data.Any.GetAsBool() ? "true" : "false");
					break;

				case ValueTypes::Int8:
					AppendIndent(Result, Indent);
					Result += StringUtility::ToString<char8>(m_Data.Any.GetAsInt8());
					break;

				case ValueTypes::Int16:
					AppendIndent(Result, Indent);
					Result += StringUtility::ToString<char8>(m_Data.Any.GetAsInt16());
					break;

				case ValueTypes::Int32:
					AppendIndent(Result, Indent);
					Result += StringUtility::ToString<char8>(m_Data.Any.GetAsInt32());
					break;

				case ValueTypes::Int64:
					AppendIndent(Result, Indent);
					Result += StringUtility::ToString<char8>(m_Data.Any.GetAsInt64());
					break;

				case ValueTypes::Float32:
					AppendIndent(Result, Indent);
					Result += StringUtility::ToString<char8>(m_Data.Any.GetAsFloat32());
					break;

				case ValueTypes::Float64:
					AppendIndent(Result, Indent);
					Result += StringUtility::ToString<char8>(m_Data.Any.GetAsFloat64());
					break;

				case ValueTypes::String:
					AppendIndent(Result, Indent);
					Result += String('"') + m_Data.Any.GetAsString() + '"';
					break;
				}
			}
			else if (m_DataType == DataTypes::Object)
			{
				Result += m_Data.Object->ToString(Indent);
			}
			else if (m_DataType == DataTypes::Array)
			{
				Result += m_Data.Array->ToString(Indent);
			}
		}
	}
}