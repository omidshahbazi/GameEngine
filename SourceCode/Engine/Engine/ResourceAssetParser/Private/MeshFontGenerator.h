// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MESH_FONT_GENERATOR_H
#define MESH_FONT_GENERATOR_H

#include <ResourceAssetParser\Private\FontGeneratorBase.h>
#include <FontSystem\FontInfo.h>
#include <Rendering\MeshInfo.h>

namespace Engine
{
	using namespace Containers;
	using namespace Rendering;
	using namespace FontSystem;

	namespace ResourceAssetParser
	{
		namespace Private
		{
			class MeshFontGenerator : public FontGeneratorBase
			{
			public:
				MeshFontGenerator(const ByteBuffer& TTFBuffer) :
					FontGeneratorBase(TTFBuffer)
				{
				}

				void Generate(FontInfo& FontInfo) override;

			private:
				void GetGlyphMeshInfo(Vertex* WorkingVerticesBuffer, MeshInfo& MeshInfo);
			};
		}
	}
}

#endif