// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_FARCTORY_H
#define RESOURCE_FARCTORY_H

#include <Containers\Strings.h>
#include <MemoryManagement\Singleton.h>
#include <Containers\Buffer.h>
#include <ResourceSystem\Text.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceSystem\Enumerators.h>

namespace Engine
{
	namespace Rendering
	{
		class Texture;
		class Program;
		class Mesh;
	}

	using namespace Containers;
	using namespace Rendering;

	namespace ResourceSystem
	{
		class ResourceManager;

		template<typename T>
		class Resource;

		namespace Private
		{
			class RESOURCESYSTEM_API ResourceFactory
			{
				SINGLETON_DECLARATION(ResourceFactory)

					friend class ResourceManager;

			private:
				enum class FileTypes
				{
					TXT = 0,
					PNG = 1,
					SHADER = 2,
					OBJ = 3,
					Unknown
				};

				enum class ResourceTypes
				{
					Text = 0,
					Texture = 1,
					Shader = 2,
					Model = 3,
					Unknown
				};

			public:
				ResourceFactory(void);
				~ResourceFactory(void);

			private:
				ByteBuffer * Compile(const WString &Extension, ByteBuffer *Buffer);

				void CompileImageFile(ByteBuffer *OutBuffer, ByteBuffer *InBuffer);
				void CompileOBJFile(ByteBuffer *OutBuffer, ByteBuffer *InBuffer);

				Text *CreateText(uint64 Size, const byte *const Data);
				Texture *CreateTexture(uint64 Size, const byte *const Data);
				Program *CreateShader(uint64 Size, const byte *const Data);
				Mesh *CreateModel(uint64 Size, const byte *const Data);

				template<typename T>
				T *Create(ByteBuffer *Buffer)
				{
					ResourceTypes resType = (ResourceTypes)Buffer->ReadValue<int32>(0);
					uint64 size = Buffer->ReadValue<uint64>(4);

					auto data = Buffer->ReadValue(12, 1);

					T *ptr = nullptr;

					switch (resType)
					{
					case ResourceTypes::Text:
						ptr = ReinterpretCast(T*, CreateText(size, data));
						break;

					case ResourceTypes::Texture:
						ptr = ReinterpretCast(T*, CreateTexture(size, data));
						break;

					case ResourceTypes::Shader:
						ptr = ReinterpretCast(T*, CreateShader(size, data));
						break;

					case ResourceTypes::Model:
						ptr = ReinterpretCast(T*, CreateModel(size, data));
						break;
					}

					return ptr;
				}

				Mesh * Create(PrimitiveMeshTypes Type);

				static FileTypes GetFileTypeByExtension(const WString &Extension);
				static ResourceTypes GetResourceTypeByFileType(FileTypes FileType);
			};
		}
	}
}

#endif