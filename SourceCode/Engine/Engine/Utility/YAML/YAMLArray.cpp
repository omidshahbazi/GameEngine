// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Utility\YAML\YAMLArray.h>

namespace Engine
{
	namespace Utility
	{
		namespace YAML
		{
			YAMLData& YAMLArray::operator[](uint32 Index)
			{
				return m_Array[Index];
			}

			String YAMLArray::ToString(int16 Indent) const
			{
				String result;

				for (const auto& it : m_Array)
				{
					for (int i = 0; i < Indent; ++i)
						result += '\t';

					it.ToString(result, Indent + 1);
					result += '\n';
				}

				return result;
			}
		}
	}
}