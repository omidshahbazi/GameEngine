// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceFactory.h>
#include <Containers\Buffer.h>
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

		Resource *ResourceFactory::Create(Buffer *Buffer)
		{
			Resource::Types type = (Resource::Types)Buffer->ReadValue<int16>(0);

			switch (type)
			{
			case Resource::Types::Texture:
				return RenderingManager::GetInstance()->GetActiveDevice()->CreateTexture2D(Buffer->GetBuffer(), 10, 10);
			}

			return nullptr;
		}
	}
}