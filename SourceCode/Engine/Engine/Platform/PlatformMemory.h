// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef PLATFORM_MEMORY_H
#define PLATFORM_MEMORY_H

#include <Common\PrimitiveTypes.h>
#include <utility>

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class PLATFORM_API PlatformMemory
		{
		public:
			static byte *Allocate(uint64 Size);

			static void Free(byte *Address);

			static void Set(byte *Address, int32 Value, uint64 Size);

			static void Copy(const byte *Source, byte *Destination, uint64 Size);
			static void Copy(const byte *Source, uint64 SourceIndex, byte *Destination, uint64 DestinationIndex, uint64 Size);

			static bool AreEqual(const byte *Left, const byte *Right, uint64 Length);
			static bool AreEqual(const byte *Left, uint64 LeftIndex, const byte *Right, uint64 RightIndex, uint64 Length);

			template<typename T>
			static T *Allocate(uint64 Length)
			{
				return ReinterpretCast(T*, Allocate(Length * sizeof(T)));
			}

			template<typename T>
			static void Free(T *Address)
			{
				Free(ReinterpretCast(byte*, Address));
			}

			template<typename T>
			static void Set(T *Address, int32 Value, uint64 Length)
			{
				Set(ReinterpretCast(byte*, Address), Value, Length * sizeof(T));
			}

			template<typename T>
			static void Copy(const T * const Source, T *Destination, uint64 Length)
			{
				Copy(Source, 0, Destination, 0, Length);
			}

			template<typename T>
			static void Copy(const T * const Source, uint64 SourceIndex, T *Destination, uint64 DestinationIndex, uint64 Length)
			{
				Copy(ReinterpretCast(const byte*, Source), SourceIndex * sizeof(T), ReinterpretCast(byte*, Destination), DestinationIndex * sizeof(T), Length * sizeof(T));
			}

			template<typename T>
			static bool AreEqual(const T *const Left, const byte *const Right, uint64 Length)
			{
				return AreEqual(Left, 0, Right, 0, Length);
			}

			template<typename T>
			static bool AreEqual(const T *const Left, uint64 LeftIndex, const T *const Right, uint64 RightIndex, uint64 Length)
			{
				return AreEqual(ReinterpretCast(const byte*, Left), LeftIndex * sizeof(T), ReinterpretCast(byte*, Right), RightIndex * sizeof(T), Length * sizeof(T));
			}

			template<typename T>
			static T &Move(T &&Value)
			{
				//return std::move(Value);
				return Value;
			}
		};
	}
}

#endif