// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_FARCTORY_H
#define RESOURCE_FARCTORY_H

#include <Containers\Strings.h>
#include <MemoryManagement\Singleton.h>
#include <Containers\Buffer.h>
#include <ResourceSystem\Resource.h>
#include <ResourceSystem\Text.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceSystem\Enumerators.h>
#include <ResourceSystem\Resource.h>

namespace Engine
{
	namespace Rendering
	{
		class Texture;
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
				SINGLETON_DECLARATION(ResourceFactory)

			private:
				ResourceFactory(void);
				~ResourceFactory(void);

			public:
				template<typename T>
				T* Create(const ByteBuffer& Buffer)
				{
					ResourceTypes resType = (ResourceTypes)Buffer.ReadValue<int32>(0);
					//uint64 dataSize = Buffer.ReadValue<uint64>(4); //TODO: findout why this isn't working well
					uint64 dataSize = Buffer.GetSize() - 12;

					auto data = ConstCast(byte*, Buffer.ReadValue(12, dataSize));

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

					return ptr;
				}

				bool CompileTXT(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer);
				Text* CreateText(const ByteBuffer& Buffer);
				void DestroyText(Text* Text);

				bool CompilePNG(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer);
				bool CompileJPG(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer);
				Texture* CreateTexture(const ByteBuffer& Buffer);
				void DestroyTexture(Texture* Texture);

				bool CompileSHADER(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer);
				Shader* CreateShader(const ByteBuffer& Buffer, String* Message = nullptr);
				void DestroyShader(Shader* Shader);

				bool CompileOBJ(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer);
				Mesh* CreateMesh(const ByteBuffer& Buffer);
				void DestroyMesh(Mesh* Mesh);
				Mesh* CreatePrimitiveMesh(PrimitiveMeshTypes Type);

				bool CompileTTF(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer);
				Font* CreateFont(const ByteBuffer& Buffer);
				void DestroyFont(Font* Font);

			private:
				void CompileImageFile(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer);
			};
		}
	}
}

#endif