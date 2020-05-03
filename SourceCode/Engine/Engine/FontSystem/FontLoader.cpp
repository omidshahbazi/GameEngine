// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <FontSystem\FontLoader.h>
#include <FontSystem\Private\MeshFontLoader.h>
#include <FontSystem\Private\TextureFontLoader.h>

namespace Engine
{
	namespace FontSystem
	{
		using namespace Private;

		void FontLoader::CreateFontFromTrueTypeFont(const ByteBuffer& InBuffer, ByteBuffer& OutBuffer, Font::RenderTypes RenderType)
		{
			OutBuffer << (int32)RenderType;

			if (RenderType == Font::RenderTypes::Mesh)
			{
				MeshFontLoader loader;
				loader.Load(InBuffer, OutBuffer);
			}
			else if (RenderType == Font::RenderTypes::Texture)
			{
				TextureFontLoader loader;
				loader.Load(InBuffer, OutBuffer);
			}
		}

		void FontLoader::LoadFont(const ByteBuffer& Buffer, Font* Font)
		{
			Font::RenderTypes renderType = (Font::RenderTypes)Buffer.ReadValue<int32>(0);

			int32 readSize = sizeof(int32);

			ByteBuffer buffer(ConstCast(byte*, Buffer.GetBuffer()), readSize, Buffer.GetSize() - readSize);

			if (renderType == Font::RenderTypes::Mesh)
			{
				MeshFontLoader::Create(buffer, Font);
			}
			else if (renderType == Font::RenderTypes::Texture)
			{
				TextureFontLoader::Create(buffer, Font);
			}
		}
	}
}