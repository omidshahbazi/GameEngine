// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef HELPER_H
#define HELPER_H

#include <Containers\Color.h>
#include <MathContainers\MathContainers.h>

namespace Engine
{
	using namespace Containers;
	using namespace MathContainers;

	namespace Rendering
	{
		namespace Private
		{
			class Helper
			{
			public:
				static void GetNormalizedColor(const ColorUI8& InColor, Vector4F& OutColor)
				{
					OutColor.X = InColor.R / 255.F;
					OutColor.Y = InColor.G / 255.F;
					OutColor.Z = InColor.B / 255.F;
					OutColor.W = InColor.A / 255.F;
				}

				static void ReverseBuffer(const byte* Source, uint32 Length, uint32 RowPitch, byte* Destination)
				{
					const uint32 len = Length / RowPitch;
					const uint16 maxIndex = len - 1;
					for (uint32 y = 0; y < len; ++y)
						PlatformMemory::Copy(Source + (RowPitch * y), Destination + (RowPitch * (maxIndex - y)), RowPitch);
				}
			};
		}
	}
}

#endif