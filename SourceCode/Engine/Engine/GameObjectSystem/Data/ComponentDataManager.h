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
				ComponentDataManager(SceneData *SceneData);

				IDType Create(void) override;

				void SetGameObjectID(IDType ID, IDType GameObjectID);

			protected:
				int32 GetIndexByGameObjectID(IDType GameObjectID) const;

			protected:
				IDFList m_GameObjectIDs;

			private:
				DynamicSizeAllocator m_GameObjectIDsAllocator;
			};
		}
	}
}

#endif