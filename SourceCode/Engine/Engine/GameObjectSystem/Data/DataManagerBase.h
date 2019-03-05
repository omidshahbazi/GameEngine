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

		namespace Data
		{
			class GAMEOBJECTSYSTEM_API DataManagerBase
			{
				friend class GameObject;

			public:
				DataManagerBase(void);

				virtual IDType Create(void);

				virtual void Update(void) = 0;
				virtual void Render(void) = 0;

			protected:
				int32 GetIndex(IDType ID) const;

			protected:
				IDFList m_IDs;

			private:
				IDType m_LastID;

				DynamicSizeAllocator m_IDAllocator;
			};
		}
	}
}

#endif