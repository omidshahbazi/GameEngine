// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMPONENT_DATA_MANAGER_H
#define COMPONENT_DATA_MANAGER_H

#include <GameObjectSystem\Data\DataManagerBase.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		namespace Data
		{
			class GAMEOBJECTSYSTEM_API ComponentDataManager : public DataManagerBase
			{
			public:
				ComponentDataManager(SceneData *SceneData);

				IDType Create(void) override;
			};
		}
	}
}

#endif