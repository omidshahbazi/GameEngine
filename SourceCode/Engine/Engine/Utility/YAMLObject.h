// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef YAML_OBJECT_H
#define YAML_OBJECT_H

#include <Utility\YAMLData.h>
#include <Containers\Map.h>

namespace Engine
{
	using namespace Containers;

	namespace Utility
	{
		class UTILITY_API YAMLObject
		{
		public:
			YAMLObject(void)
			{
			}

			YAMLData &operator[](const String &Key);

			String ToString(void) const;

		private:
			Map<String, YAMLData> m_Map;
		};
	}
}

#endif