// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef DATA_MANAGER_BASE_H
#define DATA_MANAGER_BASE_H

#include <GameObjectSystem\Data\DataContainer.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		class GameObject;
		class Transform;

		namespace Data
		{
			class SceneData;

			class GAMEOBJECTSYSTEM_API DataManagerBase
			{
				friend class GameObject;
				friend class Transform;

			public:
				DataManagerBase(SceneData *SceneData);

				virtual IDType Create(void);

				virtual void Update(void) = 0;
				virtual void Render(void) = 0;

			protected:
				int32 GetIndex(IDType ID) const;

				SceneData *GetSceneData(void) const
				{
					return m_SceneData;
				}

			protected:
				IDFList m_IDs;

			private:
				SceneData *m_SceneData;
				IDType m_LastID;

				DynamicSizeAllocator m_IDAllocator;
			};
		}
	}
}

#endif