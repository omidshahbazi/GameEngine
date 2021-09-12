// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef UTILITIES_H
#define UTILITIES_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>
#include <Containers\GUID.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace ResourceSystem
	{
		namespace Private
		{
			class Utilities
			{
			public:
				static cwstr DATA_EXTENSION;

			public:
				static uint32 GetHash(const GUID& GUID);

				static bool ReadDataFile(ByteBuffer& Buffer, const WString& Path);

				static bool WriteDataFile(const WString& Path, const ByteBuffer& Buffer);

				static WString GetDataFileName(const GUID& GUID);
			};
		}
	}
}
#endif