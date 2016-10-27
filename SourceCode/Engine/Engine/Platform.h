// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include "PrimitiveTypes.h"

namespace Engine::Platform
{
	class Memory
	{
	public:
		static byte *Allocate(uint32 Size);

		static void Free(byte *Address);

		static void Set(byte *Address, int32 Value, uint32 Size);

		static void Copy(const byte *Source, byte *Destination, uint32 Size);
	};
}