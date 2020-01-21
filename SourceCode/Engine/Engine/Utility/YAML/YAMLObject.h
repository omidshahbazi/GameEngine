// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef YAML_OBJECT_H
#define YAML_OBJECT_H

#include <Utility\YAML\YAMLData.h>
#include <Containers\Map.h>

namespace Engine
{
	using namespace Containers;

	namespace Utility
	{
		namespace YAML
		{
			class UTILITY_API YAMLObject
			{
			public:
				YAMLObject(void)
				{
				}

				YAMLData &operator[](const String &Key);

				String ToString(int16 Indent = 0) const;

			private:
				Map<String, YAMLData> m_Map;
			};
		}
	}
}

#endif