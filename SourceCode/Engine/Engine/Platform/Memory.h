// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>

#ifndef MEMORY_H
#define MEMORY_H

namespace Engine
{
	using namespace Common;

	namespace Platform
	{
		class Memory
		{
		public:
			static byte *Allocate(uint64 Size);

			static void Free(byte *Address);

			static void Set(byte *Address, int32 Value, uint64 Size);

			static void Copy(const byte *Source, byte *Destination, uint64 Size);
		};
	}
}

#endif