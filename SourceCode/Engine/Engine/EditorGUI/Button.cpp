// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Button.h>
#include <EditorGUI\Private\Resources.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace EditorGUI
	{
		using namespace Private;

		Button::Button(void)
		{
		}

		void Button::Render(EditorRenderDeviceBase* Device) const
		{
			//Material* buttonMat = Resources::GetSpriteRendererMaterial();
			//Pass& pass = buttonMat->GetPasses()[0];
			//pass.SetVector2("elemDim", Vector2F(buttonSize.X, buttonSize.Y));

			//SpriteHandle* sprite = Resources::GetButtonSprite();
			//pass.SetSprite("difTex", sprite);
			//auto& dimension = sprite->GetData()->GetDimension();
			//pass.SetVector2("texDim", Vector2F(dimension.X, dimension.Y));
			//auto& borders = sprite->GetData()->GetBorders();
			//pass.SetVector4("texBorder", Vector4F(borders.X, borders.Y, borders.Z, borders.W));

			//pass.SetFloat32("drawMode", 1);
			//Device->DrawMesh(Vector3F::Zero, Vector3F::Zero, buttonSize, buttonMat);
		}
	}
}