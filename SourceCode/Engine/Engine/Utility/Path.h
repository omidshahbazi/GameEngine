// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#ifndef PATH_H
#define PATH_H

#include <Containers\Strings.h>

namespace Engine
{
	namespace Utility
	{
#define FORWARD_SLASH StringUtility::Character<T, '/'>::Value
#define BACKWARD_SLASH StringUtility::Character<T, '\\'>::Value
#define DOT StringUtility::Character<T, '.'>::Value

		class Path
		{
		public:
			template<typename T>
			INLINE static DynamicString<T> ChangeExtension(const DynamicString<T> &Path, const DynamicString<T> &Extension)
			{

			}

			template<typename T>
			INLINE static DynamicString<T> Combine(const DynamicString<T> &Path1, const DynamicString<T> &Path2)
			{

			}

			template<typename T>
			INLINE static DynamicString<T> Combine(const DynamicString<T> &Path1, const DynamicString<T> &Path2, const DynamicString<T> &Path3)
			{

			}

			template<typename T>
			INLINE static DynamicString<T> Combine(const DynamicString<T> &Path1, const DynamicString<T> &Path2, const DynamicString<T> &Path3, const DynamicString<T> &Path4)
			{

			}

			template<typename T>
			INLINE static DynamicString<T> GetDirectoryName(const DynamicString<T> &Path)
			{

			}

			template<typename T>
			INLINE static DynamicString<T> GetExtension(const DynamicString<T> &Path)
			{
				auto str = Normalize(Path);

				int32 index = str.LastIndexOf(DOT);

				if (index == -1)
					return "";

				return str.SubString(index);
			}

			template<typename T>
			INLINE static DynamicString<T> GetFileName(const DynamicString<T> &Path)
			{
				auto str = Normalize(Path);

				int32 slashIndex = str.LastIndexOf(FORWARD_SLASH);

				if (slashIndex == -1)
					return "";

				if (str.LastIndexOf(DOT) == -1)
					return "";

				return str.SubString(slashIndex);
			}

			template<typename T>
			INLINE static DynamicString<T> GetFileNameWithoutExtension(const DynamicString<T> &Path)
			{
				auto str = Normalize(Path);

				int32 slashIndex = str.LastIndexOf(FORWARD_SLASH);

				if (slashIndex == -1)
					return "";

				int32 dotIndex = str.LastIndexOf(DOT);

				if (dotIndex == -1)
					return "";

				return str.SubString(slashIndex, dotIndex - slashIndex);
			}

			template<typename T>
			INLINE static DynamicString<T> GetPathRoot(const DynamicString<T> &Path)
			{

			}

			template<typename T>
			INLINE static bool HasExtension(const DynamicString<T> &Path)
			{
				auto parts = Normalize(Path).Split(FORWARD_SLASH);

				return parts[parts.GetSize() - 1].Contains(DOT);
			}

			template<typename T>
			INLINE static DynamicString<T> Normalize(const DynamicString<T> &Path)
			{
				return Path.Replace(BACKWARD_SLASH, FORWARD_SLASH).Replace("//", FORWARD_SLASH);
			}
		};
	}
}

#endif