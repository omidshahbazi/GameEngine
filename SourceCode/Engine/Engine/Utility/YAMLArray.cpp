// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Utility\YAMLArray.h>

namespace Engine
{
	namespace Utility
	{
		YAMLData &YAMLArray::operator[](uint32 Index)
		{
			return m_Array[Index];
		}

		String YAMLArray::ToString(int16 Indent) const
		{
			String str;

			for each (const auto &it in m_Array)
			{
				str += it.ToString(Indent);
				str += '\n';
			}

			return str;
		}
	}
}