// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceFactory.h>
#include <ResourceSystem\Resource.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <Rendering\RenderingManager.h>

namespace Engine
{
	using namespace Containers;
	using namespace Rendering;

	namespace ResourceSystem
	{
		using namespace Private;

		SINGLETON_DECLARATION(ResourceFactory)

			Resource::Types GetTypeByExtension(const WString &Extension)
		{
			if (Extension == L".png")
				return Resource::Types::Texture;

			return Resource::Types::Unknown;
		}

		ResourceFactory::ResourceFactory(void)
		{
		}

		ResourceFactory::~ResourceFactory(void)
		{
		}

		ByteBuffer *ResourceFactory::Compile(const WString &Extension, ByteBuffer *Buffer)
		{
			Resource::Types type = GetTypeByExtension(Extension);

			if (type == Resource::Types::Unknown)
				return nullptr;

			ByteBuffer *buffer = ResourceSystemAllocators::Allocate<ByteBuffer>(1);
			new (buffer) ByteBuffer(&ResourceSystemAllocators::ResourceAllocator);

			int32 typeInt = (int32)type;
			buffer->AppendBuffer(reinterpret_cast<byte*>(&typeInt), 0, sizeof(int32));

			switch (type)
			{
			case Resource::Types::Texture:
				buffer->AppendBuffer(*Buffer);
				break;
			}

			return buffer;
		}

		Resource *ResourceFactory::Create(ByteBuffer *Buffer)
		{
			Resource::Types type = (Resource::Types)Buffer->ReadValue<int32>(0);

			switch (type)
			{
			case Resource::Types::Texture:
				return RenderingManager::GetInstance()->GetActiveDevice()->CreateTexture2D(Buffer->GetBuffer(), 10, 10);
			}

			return nullptr;
		}
	}
}