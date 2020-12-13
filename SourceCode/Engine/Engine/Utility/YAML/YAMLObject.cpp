// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Utility\YAML\YAMLObject.h>

namespace Engine
{
	namespace Utility
	{
		namespace YAML
		{
			YAMLData& YAMLObject::operator[](const String& Key)
			{
				return m_Map[Key];
			}

			String YAMLObject::ToString(int16 Indent) const
			{
				String result;

				for (const auto& it : m_Map)
				{
					for (int i = 0; i < Indent; ++i)
						result += '\t';

					result += it.GetFirst();
					result += ':';

					auto& data = it.GetSecond();

					switch (data.GetType())
					{
					case YAMLData::DataTypes::Object:
					case YAMLData::DataTypes::Array:
						result += "\n";
						break;
					}

					data.ToString(result, Indent);
					result += '\n';
				}

				return result;
			}
		}
	}
}