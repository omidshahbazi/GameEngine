// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <FontSystem\FontLoader.h>
#include <FontSystem\Private\MeshFontLoader.h>

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
		}

		void FontLoader::LoadFont(const ByteBuffer& Buffer, Font* Font)
		{
			Font::RenderTypes renderType = (Font::RenderTypes)Buffer.ReadValue<int32>(0);

			int32 readSize = sizeof(int32);

			if (renderType == Font::RenderTypes::Mesh)
				MeshFontLoader::Create(ByteBuffer(ConstCast(byte*, Buffer.GetBuffer()), readSize, Buffer.GetSize() - readSize), Font);
		}
	}
}