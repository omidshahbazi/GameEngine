// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <GameObjectSystem\Data\ComponentDataManager.h>
#include <GameObjectSystem\Private\GameObjectSystemAllocators.h>

namespace Engine
{
	using namespace Rendering;
	using namespace MemoryManagement::Allocator;

	namespace GameObjectSystem
	{
		using namespace Private;

		namespace Data
		{
			ComponentDataManager::ComponentDataManager(SceneData *SceneData) :
				DataManagerBase(SceneData)
			{
			}

			IDType ComponentDataManager::Create(void)
			{
				auto id = DataManagerBase::Create();

				return id;
			}
		}
	}
}
