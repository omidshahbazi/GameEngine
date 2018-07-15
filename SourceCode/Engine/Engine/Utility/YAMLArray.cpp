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
			String result;

			for each (const auto &it in m_Array)
			{
				for (int i = 0; i < Indent; ++i)
					result += '\t';

				result += it.ToString(Indent);
				result += '\n';
			}

			return result;
		}
	}
}