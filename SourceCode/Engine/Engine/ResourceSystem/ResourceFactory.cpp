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

		ResourceFactory::ResourceFactory(void)
		{
		}

		ResourceFactory::~ResourceFactory(void)
		{
		}

		ByteBuffer *ResourceFactory::Compile(const WString &Extension, ByteBuffer *Buffer)
		{
			ResourceTypes type = GetTypeByExtension(Extension);

			if (type == ResourceTypes::Unknown)
				return nullptr;

			ByteBuffer *buffer = ResourceSystemAllocators::Allocate<ByteBuffer>(1);
			new (buffer) ByteBuffer(&ResourceSystemAllocators::ResourceAllocator);

			int32 typeInt = (int32)type;
			buffer->AppendBuffer(ReinterpretCast(byte*, &typeInt), 0, sizeof(int32));

			switch (type)
			{
			case ResourceTypes::Text:
			case ResourceTypes::Texture:
				buffer->AppendBuffer(*Buffer);
				break;
			}

			return buffer;
		}
	}
}