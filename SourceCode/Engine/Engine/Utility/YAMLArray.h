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

			INLINE void Add(bool Value)
			{
				m_Array.Add(Value);
			}

			INLINE void Add(int8 Value)
			{
				m_Array.Add(Value);
			}

			INLINE void Add(int16 Value)
			{
				m_Array.Add(Value);
			}

			INLINE void Add(int32 Value)
			{
				m_Array.Add(Value);
			}

			INLINE void Add(const int64 &Value)
			{
				m_Array.Add(Value);
			}

			INLINE void Add(float32 Value)
			{
				m_Array.Add(Value);
			}

			INLINE void Add(const float64 &Value)
			{
				m_Array.Add(Value);
			}

			INLINE void Add(cstr Value)
			{
				m_Array.Add(Value);
			}

			INLINE void Add(const String &Value)
			{
				m_Array.Add(Value);
			}

			INLINE void Add(YAMLObject *Value)
			{
				m_Array.Add(Value);
			}

			INLINE void Add(YAMLArray *Value)
			{
				m_Array.Add(Value);
			}

			String ToString(int16 Indent = 0) const;

		private:
			Vector<YAMLData> m_Array;
		};
	}
}

#endif