// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef HELPER_H
#define HELPER_H

#include <Containers\Color.h>
#include <MathContainers\MathContainers.h>
#include <Platform\PlatformMemory.h>
#include <RenderCommon\Enums.h>

namespace Engine
{
	using namespace Platform;
	using namespace Containers;
	using namespace MathContainers;

	namespace RenderCommon
	{
		class RENDERCOMMON_API Helper
		{
		public:
			static void GetNormalizedColor(const ColorUI8& InColor, Vector4F& OutColor);

			static void ReverseBuffer(const byte* Source, uint32 Length, uint32 RowPitch, byte* Destination);

			static uint8 GetTextureChannelSize(Formats Format);
			static uint8 GetTextureChannelCount(Formats Format);
			static uint8 GetTexturePixelSize(Formats Format);
			static uint32 GetTextureRowPitch(Formats Format, int32 Width);
			static uint32 GetTextureBufferSize(Formats Format, const Vector2I& Dimension);

			static uint32 GetMeshVertexSize(void);
			static uint32 GetMeshIndexSize(void);
			static uint32 GetMeshVertexBufferSize(uint32 Count);
			static uint32 GetMeshIndexBufferSize(uint32 Count);

			static bool IsColorPoint(AttachmentPoints Point);
		};
	}
}

#endif