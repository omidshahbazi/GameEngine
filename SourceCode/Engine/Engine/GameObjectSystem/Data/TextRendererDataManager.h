// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXT_RENDERER_DATA_MANAGER_H
#define TEXT_RENDERER_DATA_MANAGER_H

#include <GameObjectSystem\Data\ComponentDataManager.h>
#include <FontSystem\Font.h>

namespace Engine
{
	using namespace FontSystem;

	namespace GameObjectSystem
	{
		namespace Data
		{
			class GAMEOBJECTSYSTEM_API TextRendererDataManager : public ComponentDataManager
			{
			public:
				TextRendererDataManager(SceneData *SceneData);

				IDType Create(void) override;

				void SetFont(IDType ID, FontHandle *Mesh);
				void SetText(IDType ID, const WString &Text);
				void SetMaterial(IDType ID, Material *Material);

				void Update(void) override
				{
				}

				void Render(void) override;

			private:
				DataContainer<FontHandle*> m_Fonts;
				DataContainer<WString> m_Texts;
				MaterialList m_Materials;

				DynamicSizeAllocator m_FontHandlesAllocator;
				DynamicSizeAllocator m_TextsAllocator;
				DynamicSizeAllocator m_MaterialsAllocator;
			};
		}
	}
}

#endif