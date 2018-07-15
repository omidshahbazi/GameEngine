// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef YAML_ARRAY_H
#define YAML_ARRAY_H

#include <Utility\YAMLData.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Containers;

	namespace Utility
	{
		class UTILITY_API YAMLArray
		{
		public:
			YAMLArray(void)
			{
			}

			YAMLData &operator[](uint32 Index);

			String ToString(int16 Indent = 0) const;

		private:
			Vector<YAMLData> m_Array;
		};
	}
}

#endif