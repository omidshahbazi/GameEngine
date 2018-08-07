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
	using namespace Containers;

	namespace ResourceSystem
	{
		using namespace Private;

		template<typename T>
		class Resource;

		class RESOURCESYSTEM_API ResourceFactory
		{
			SINGLETON_DEFINITION(ResourceFactory)

		private:
			enum class ResourceTypes
			{
				Text = 0,
				Texture = 1,
				Unknown
			};

			ResourceTypes GetTypeByExtension(const WString &Extension)
			{
				if (Extension == L".txt")
					return ResourceTypes::Text;

				if (Extension == L".png")
					return ResourceTypes::Texture;

				return ResourceTypes::Unknown;
			}

		public:
			ResourceFactory(void);
			~ResourceFactory(void);

			ByteBuffer *Compile(const WString &Extension, ByteBuffer *Buffer);

			template<typename T>
			T *Create(ByteBuffer *Buffer)
			{
				ResourceTypes type = (ResourceTypes)Buffer->ReadValue<int32>(0);

				switch (type)
				{
				case ResourceTypes::Text:
				{
					Text *text = ResourceSystemAllocators::Allocate<Text>(1);
					new (text) Text(Buffer);
					return reinterpret_cast<T*>(text);
				}

				case ResourceTypes::Texture:
					return reinterpret_cast<T*>(RenderingManager::GetInstance()->GetActiveDevice()->CreateTexture2D(Buffer->GetBuffer(), 10, 10));
				}

				return nullptr;
			}
		};
	}
}

#endif