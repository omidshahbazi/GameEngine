// Copyright 2016-2017 ?????????????. All Rights Reserved.
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
				ComponentDataManager(void);

				IDType Create(void) override;

				void SetGameObjectID(IDType ID, IDType GameObjectID);

			private:
				IDFList m_GameObjectIDs;

				DynamicSizeAllocator m_GameObjectIDsAllocator;
			};
		}
	}
}

#endif