// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef PATH_H
#define PATH_H

#include <Containers\Strings.h>
#include <filesystem>

namespace Engine
{
	namespace Utility
	{
#define FORWARD_SLASH CharacterUtility::Character<T, '/'>::Value
#define BACKWARD_SLASH CharacterUtility::Character<T, '\\'>::Value
#define DOT CharacterUtility::Character<T, '.'>::Value

		class Path
		{
		public:
			template<typename T>
			INLINE static DynamicString<T> ChangeExtension(const DynamicString<T>& Path, const DynamicString<T>& Extension)
			{std::filesystem
				auto str = Normalize(Path);

				int32 index = str.LastIndexOf(DOT);

				if (index == -1)
					return Path;

				return str.SubString(0, index) + Extension;
			}

			template<typename T>
			INLINE static DynamicString<T> Combine(const DynamicString<T>& Path1, const DynamicString<T>& Path2)
			{
				auto str1 = Normalize(Path1);
				auto str2 = Normalize(Path2);

				if (!str1.EndsWith(FORWARD_SLASH))
					str1 += FORWARD_SLASH;

				return str1 + str2;
			}

			template<typename T>
			INLINE static DynamicString<T> Combine(const DynamicString<T>& Path1, const DynamicString<T>& Path2, const DynamicString<T>& Path3)
			{
				return Combine(Combine(Path1, Path2), Path3);
			}

			template<typename T>
			INLINE static DynamicString<T> Combine(const DynamicString<T>& Path1, const DynamicString<T>& Path2, const DynamicString<T>& Path3, const DynamicString<T>& Path4)
			{
				return Combine(Combine(Path1, Path2), Path3, Path4);
			}

			template<typename T>
			INLINE static DynamicString<T> GetRelativePath(const DynamicString<T>& BasePath, const DynamicString<T>& Path)
			{
				auto baseParts = Normalize(BasePath).Split(FORWARD_SLASH);
				auto pathParts = Normalize(Path).Split(FORWARD_SLASH);

				const DynamicString<T> BACKWARD = DynamicString<T>(DOT) + DOT + FORWARD_SLASH;

				DynamicString<T> result;

				uint8 index = 0;
				for (uint8 i = 0; i < baseParts.GetSize(); ++i)
				{
					if (i >= pathParts.GetSize())
					{
						result += BACKWARD;
						continue;
					}

					if (baseParts[i] == pathParts[i])
					{
						index = i;
						continue;
					}

					result += BACKWARD;
				}

				++index;

				for (; index < pathParts.GetSize(); ++index)
				{
					result += pathParts[index];

					if (index != pathParts.GetSize() - 1)
						result += FORWARD_SLASH;
				}

				return result;
			}

			template<typename T>
			INLINE static DynamicString<T> GetDirectoryName(const DynamicString<T>& Path)
			{
				auto str = Normalize(Path);

				int32 slashIndex = str.LastIndexOf(FORWARD_SLASH);

				if (slashIndex == -1)
					return DynamicString<T>();

				return str.SubString(0, slashIndex);
			}

			template<typename T>
			INLINE static DynamicString<T> GetExtension(const DynamicString<T>& Path)
			{
				auto str = Normalize(Path);

				int32 index = Path.LastIndexOf(DOT);

				if (index == -1)
					return DynamicString<T>();

				return Path.SubString(index);
			}

			template<typename T>
			INLINE static DynamicString<T> GetFileName(const DynamicString<T>& Path)
			{
				auto str = Normalize(Path);

				if (str.LastIndexOf(DOT) == -1)
					return DynamicString<T>();

				int32 slashIndex = str.LastIndexOf(FORWARD_SLASH);

				if (slashIndex == -1)
					return Path;

				++slashIndex;

				return str.SubString(slashIndex);
			}

			template<typename T>
			INLINE static DynamicString<T> GetFileNameWithoutExtension(const DynamicString<T>& Path)
			{
				auto str = Normalize(Path);

				int32 slashIndex = str.LastIndexOf(FORWARD_SLASH);

				if (slashIndex == -1)
					return DynamicString<T>();

				int32 dotIndex = str.LastIndexOf(DOT);

				if (dotIndex == -1)
					return DynamicString<T>();

				++slashIndex;

				return str.SubString(slashIndex, dotIndex - slashIndex);
			}

			template<typename T>
			INLINE static bool HasExtension(const DynamicString<T>& Path)
			{
				auto parts = Normalize(Path).Split(FORWARD_SLASH);

				return parts[parts.GetSize() - 1].Contains(DOT);
			}

			template<typename T>
			INLINE static DynamicString<T> Normalize(const DynamicString<T>& Path)
			{
				return Path.Replace(BACKWARD_SLASH, FORWARD_SLASH).Replace(String("//").ChangeType<T>(), FORWARD_SLASH);
			}
		};
	}
}

#endif