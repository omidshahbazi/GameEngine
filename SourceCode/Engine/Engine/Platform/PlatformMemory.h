// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>
#include <utility>

#ifndef PLATFORM_MEMORY_H
#define PLATFORM_MEMORY_H

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
				return reinterpret_cast<T*>(Allocate(Length * sizeof(T)));
			}

			template<typename T>
			static void Free(T *Address)
			{
				Free(reinterpret_cast<byte*>(Address));
			}

			template<typename T>
			static void Set(T *Address, int32 Value, uint64 Length)
			{
				Set(reinterpret_cast<byte*>(Address), Value, Length * sizeof(T));
			}

			template<typename T>
			static void Copy(const T *Source, T *Destination, uint64 Length)
			{
				Copy(Source, 0, Destination, 0, Length);
			}

			template<typename T>
			static void Copy(const T *Source, uint64 SourceIndex, T *Destination, uint64 DestinationIndex, uint64 Length)
			{
				Copy(reinterpret_cast<const byte*>(Source), SourceIndex * sizeof(T), reinterpret_cast<byte*>(Destination), DestinationIndex * sizeof(T), Length * sizeof(T));
			}

			template<typename T>
			static bool AreEqual(const T *Left, const byte *T, uint64 Length)
			{
				return AreEqual(Left, 0, Right, 0, Length);
			}

			template<typename T>
			static bool AreEqual(const T *Left, uint64 LeftIndex, const T *Right, uint64 RightIndex, uint64 Length)
			{
				return AreEqual(reinterpret_cast<const byte*>(Left), LeftIndex * sizeof(T), reinterpret_cast<byte*>(Right), RightIndex * sizeof(T), Length * sizeof(T));
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