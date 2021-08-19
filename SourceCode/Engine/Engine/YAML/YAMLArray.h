// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef YAML_ARRAY_H
#define YAML_ARRAY_H

#include <YAML\YAMLData.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Containers;

	namespace YAML
	{
		class YAML_API YAMLArray
		{
		public:
			YAMLArray(void)
			{
			}

			YAMLData& operator[](uint32 Index);

			INLINE void Add(const AnyDataType& Value)
			{
				m_Array.Add(Value);
			}

			INLINE void Add(YAMLObject* Value)
			{
				m_Array.Add(Value);
			}

			INLINE void Add(YAMLArray* Value)
			{
				m_Array.Add(Value);
			}

			INLINE uint32 GetSize(void) const
			{
				return m_Array.GetSize();
			}

			String ToString(int16 Indent = 0) const;

		private:
			Vector<YAMLData> m_Array;
		};
	}
}

#endif