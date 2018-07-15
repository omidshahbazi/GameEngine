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

		String YAMLObject::ToString(int16 Indent) const
		{
			String result;

			for each (const auto &it in m_Map)
			{
				for (int i = 0; i < Indent; ++i)
					result += '\t';

				result += it.GetFirst();
				result += ':';
				result += it.GetSecond().ToString(Indent);
				result += '\n';
			}

			return result;
		}
	}
}