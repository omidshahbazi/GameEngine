// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_FARCTORY_H
#define RESOURCE_FARCTORY_H

#include <Containers\Strings.h>
#include <MemoryManagement\Singleton.h>
#include <Containers\Buffer.h>
#include <ResourceSystem\Resource.h>
#include <ResourceSystem\Text.h>
#include <ResourceSystem\Resource.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceSystem\Private\ImExporter.h>
#include <Rendering\TextureInfo.h>

namespace Engine
{
	namespace Rendering
	{
		class Sprite;
		class Shader;
		class Mesh;
	}

	namespace FontSystem
	{
		class Font;
	}

	using namespace Containers;
	using namespace Rendering;
	using namespace FontSystem;

	namespace ResourceSystem
	{
		namespace Private
		{
			class ResourceHolder;

			class RESOURCESYSTEM_API ResourceFactory
			{
			public:
				template<typename T>
				struct CreateResult
				{
				public:
					String ID;
					T* Resource;
				};

			public:
				template<typename T>
				static CreateResult<T> Create(const ByteBuffer& Buffer)
				{
					String id;
					ResourceTypes resType = ResourceTypes::Unknown;
					uint64 dataSize = 0;
					byte* data = nullptr;
					if (!ReadHeader(Buffer, &id, &resType, &dataSize, &data))
						return { "", nullptr };

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

					case ResourceTypes::Shader:
						ptr = ReinterpretCast(T*, CreateShader(buffer));
						break;

					case ResourceTypes::Mesh:
						ptr = ReinterpretCast(T*, CreateMesh(buffer));
						break;

					case ResourceTypes::Font:
						ptr = ReinterpretCast(T*, CreateFont(buffer));
						break;
					}

					return { id, ptr };
				}

				static bool CompileTXT(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::TextSettings& Settings);
				static Text* CreateText(const ByteBuffer& Buffer);
				static void DestroyText(Text* Text);

				static bool CompilePNG(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::TextureSettings& Settings);
				static bool CompileJPG(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::TextureSettings& Settings);
				static Texture* CreateTexture(const ByteBuffer& Buffer);
				static void DestroyTexture(Texture* Texture);
				static Sprite* CreateSprite(const ByteBuffer& Buffer);
				static void DestroySprite(Sprite* Sprite);

				static bool CompileSHADER(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::ShaderSettings& Settings);
				static Shader* CreateShader(const ByteBuffer& Buffer);
				static void DestroyShader(Shader* Shader);

				static bool CompileOBJ(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::MeshSettings& Settings);
				static Mesh* CreateMesh(const ByteBuffer& Buffer);
				static void DestroyMesh(Mesh* Mesh);

				static bool CompileTTF(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::FontSettings& Settings);
				static Font* CreateFont(const ByteBuffer& Buffer);
				static void DestroyFont(Font* Font);

				static bool ReadHeader(const ByteBuffer& Buffer, String* ID, ResourceTypes* ResourceType, uint64* DataSize, byte** Data);

			private:
				static void WriteHeader(ByteBuffer& Buffer, const String& ID, ResourceTypes Type, uint64 DataSize);

				static void CompileImageFile(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::TextureSettings& Settings);
			};
		}
	}
}

#endif