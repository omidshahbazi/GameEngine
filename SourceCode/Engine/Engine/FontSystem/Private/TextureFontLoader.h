// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXTURE_FONT_LOADER_H
#define TEXTURE_FONT_LOADER_H

#include <FontSystem\Font.h>
#include <FontSystem\Private\FontLoaderBase.h>
#include <Rendering\MeshInfo.h>

namespace Engine
{
	using namespace Containers;
	using namespace Rendering;

	namespace FontSystem
	{
		namespace Private
		{
			class TextureFontLoader : public FontLoaderBase
			{
			public:
				void Load(const ByteBuffer& InBuffer, ByteBuffer& OutBuffer) override;

				static void Create(const ByteBuffer& Buffer, Font* Font);

			private:
				void GetGlyphMeshInfo(Vertex* WorkingVerticesBuffer, MeshInfo& MeshInfo);
			};
		}
	}
}

#endif