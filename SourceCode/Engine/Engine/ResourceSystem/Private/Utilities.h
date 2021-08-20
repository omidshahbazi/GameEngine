// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef UTILITIES_H
#define UTILITIES_H

#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace ResourceSystem
	{
		namespace Private
		{
			class RESOURCESYSTEM_API Utilities
			{
			public:
				static cwstr DATA_EXTENSION;

			public:
				static uint32 GetHash(const WString& Value);

				static bool ReadDataFile(ByteBuffer& Buffer, const WString& Path);

				static bool WriteDataFile(const WString& Path, const ByteBuffer& Buffer);

				static WString GetDataFileName(const WString& RelativeFilePath);
			};
		}
	}
}
#endif