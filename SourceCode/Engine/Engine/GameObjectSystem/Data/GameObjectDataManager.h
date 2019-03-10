// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef GAME_OBJECT_DATA_MANAGER_H
#define GAME_OBJECT_DATA_MANAGER_H

#include <GameObjectSystem\Data\DataManagerBase.h>

namespace Engine
{
	namespace GameObjectSystem
	{
		class GameObject;

		namespace Data
		{
			class GAMEOBJECTSYSTEM_API GameObjectDataManager : public DataManagerBase
			{
				enum class ComponentTypes
				{
					Transform = 0,
					Camera = 1,
					Renderer = 2
				};

				friend class GameObject;

			public:
				GameObjectDataManager(SceneData *SceneData);

				IDType Create(void) override;

				void SetComponentFlagState(IDType ID, ComponentTypes Component, bool Enabled);

				void Update(void) override
				{
				}
				void Render(void) override
				{
				}

			private:
				IDFList m_ParentIDs;
				ComponentMaskList m_ComponentMasks;

				DynamicSizeAllocator m_ParentIDAllocator;
				DynamicSizeAllocator m_ComponentMaskAllocator;
			};
		}
	}
}

#endif