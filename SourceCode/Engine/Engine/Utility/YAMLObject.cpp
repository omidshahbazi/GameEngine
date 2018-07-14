// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Utility\YAMLObject.h>

namespace Engine
{
	namespace Utility
	{
		YAMLData &YAMLObject::operator[](const String &Key)
		{
			return m_Map[Key];
		}

		String YAMLObject::ToString(void) const
		{
			String str;

			for each (const auto &it in m_Map)
			{
				str += it.first;
				str += ':';
				str += it.second.ToString();
			}

			return str;
		}
	}
}