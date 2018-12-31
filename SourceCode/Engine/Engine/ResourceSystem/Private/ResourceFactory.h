// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_FARCTORY_H
#define RESOURCE_FARCTORY_H

#include <Containers\Strings.h>
#include <MemoryManagement\Singleton.h>
#include <Containers\Buffer.h>
#include <ResourceSystem\Text.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		class Texture;
		class Program;
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
				enum class ResourceTypes
				{
					Text = 0,
					Texture = 1,
					Shader = 2,
					Unknown
				};

			public:
				ResourceFactory(void);
				~ResourceFactory(void);

			private:
				ByteBuffer * Compile(const WString &Extension, ByteBuffer *Buffer);

				Text *CreateText(ResourceTypes Type, uint64 Size, const byte *const Data);
				Texture *CreateTexture(ResourceTypes Type, uint64 Size, const byte *const Data);
				Program *CreateShader(ResourceTypes Type, uint64 Size, const byte *const Data);

				template<typename T>
				T *Create(ByteBuffer *Buffer)
				{
					ResourceTypes type = (ResourceTypes)Buffer->ReadValue<int32>(0);
					uint64 size = Buffer->ReadValue<uint64>(4);

					auto data = Buffer->ReadValue(12, size);

					T *ptr = nullptr;

					switch (type)
					{
					case ResourceTypes::Text:
						ptr = ReinterpretCast(T*, CreateText(type, size, data));
						break;

					case ResourceTypes::Texture:
						ptr = ReinterpretCast(T*, CreateTexture(type, size, data));
						break;

					case ResourceTypes::Shader:
						ptr = ReinterpretCast(T*, CreateShader(type, size, data));
						break;
					}

					return ptr;
				}

				static ResourceTypes GetTypeByExtension(const WString &Extension);
			};
		}
	}
}

#endif