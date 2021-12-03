// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_FARCTORY_H
#define RESOURCE_FARCTORY_H

#include <Containers\Strings.h>
#include <MemoryManagement\Singleton.h>
#include <Containers\Buffer.h>
#include <ResourceSystem\Text.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceCommon\Resource.h>
#include <ResourceImportExport\ImporterExporter.h>
#include <RenderDevice\TextureInfo.h>

namespace Engine
{
	namespace RenderSystem
	{
		class Sprite;
		class Program;
		class Mesh;
	}

	namespace FontSystem
	{
		class Font;
	}

	using namespace Containers;
	using namespace RenderDevice;
	using namespace RenderSystem;
	using namespace FontSystem;
	using namespace ResourceCommon;
	using namespace ResourceImportExport;

	namespace ResourceSystem
	{
		class ResourceHolder;

		namespace Private
		{
			class ResourceFactory
			{
			public:
				template<typename T>
				static T* Create(const ByteBuffer& Buffer)
				{
					ResourceTypes resType = ResourceTypes::Unknown;
					uint64 dataSize = 0;
					byte* data = nullptr;
					if (!ReadHeader(Buffer, &resType, &dataSize, &data))
						return nullptr;

					ByteBuffer buffer(data, dataSize);

					T* ptr = nullptr;

					switch (resType)
					{
					case ResourceTypes::Text:
						ptr = ReinterpretCast(T*, CreateText(buffer));
						break;

					case ResourceTypes::Texture:
						ptr = ReinterpretCast(T*, CreateTexture(buffer));
						break;

					case ResourceTypes::Sprite:
						ptr = ReinterpretCast(T*, CreateSprite(buffer));
						break;

					case ResourceTypes::Program:
						ptr = ReinterpretCast(T*, CreateProgram(buffer));
						break;

					case ResourceTypes::Mesh:
						ptr = ReinterpretCast(T*, CreateMesh(buffer));
						break;

					case ResourceTypes::Font:
						ptr = ReinterpretCast(T*, CreateFont(buffer));
						break;
					}

					return ptr;
				}

				static bool CompileTXT(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImporterExporter::TextSettings& Settings);
				static Text* CreateText(const ByteBuffer& Buffer);
				static void DestroyText(Text* Text);

				static bool CompilePNG(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImporterExporter::TextureSettings& Settings);
				static bool CompileJPG(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImporterExporter::TextureSettings& Settings);
				static Texture* CreateTexture(const ByteBuffer& Buffer);
				static void DestroyTexture(Texture* Texture);
				static Sprite* CreateSprite(const ByteBuffer& Buffer);
				static void DestroySprite(Sprite* Sprite);

				static bool CompilePROGRAM(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImporterExporter::ProgramSettings& Settings);
				static String GetProgramSource(const ByteBuffer& Buffer);
				static Program* CreateProgram(const ByteBuffer& Buffer);
				static void DestroyProgram(Program* Program);

				static bool CompileOBJ(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImporterExporter::MeshSettings& Settings);
				static Mesh* CreateMesh(const ByteBuffer& Buffer);
				static void DestroyMesh(Mesh* Mesh);

				static bool CompileTTF(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImporterExporter::FontSettings& Settings);
				static Font* CreateFont(const ByteBuffer& Buffer);
				static void DestroyFont(Font* Font);

				static bool ReadHeader(const ByteBuffer& Buffer, ResourceTypes* ResourceType, uint64* DataSize, byte** Data);

			private:
				static void WriteHeader(ByteBuffer& Buffer, ResourceTypes Type, uint64 DataSize);

				static void CompileImageFile(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImporterExporter::TextureSettings& Settings);
			};
		}
	}
}

#endif