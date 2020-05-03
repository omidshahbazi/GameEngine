// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <FontSystem\StringRenderer.h>

namespace Engine
{
	namespace FontSystem
	{
		void StringRenderer::Render(DrawCallback DrawCallback, const Matrix4F& Model, const WString& Text, Font* Font, float32 Size, float32 Alignment)
		{
			// Glyphs are really large
			// TODO: check about size in loading time after moving to ResourceFactory
			Size /= 40;

			Matrix4F charMat(Matrix4F::Identity);
			charMat.SetScale({ Size, Size, 1 });

			float32 maxYAdvance = 0.0F;
			float32 sumYAdvance = 0.0F;
			float32 sumXAdvance = 0.0F;
			for (uint32 i = 0; i < Text.GetLength(); ++i)
			{
				char16 charCode = Text[i];

				if (charCode == '\n' || charCode == '\r')
				{
					sumYAdvance -= maxYAdvance;
					sumXAdvance = 0;
					maxYAdvance = 0;

					continue;
				}

				Font::Character* ch = Font->GetCharacter(Text[i]);

				if (ch == nullptr)
				{
					ch = Font->GetCharacter('?');

					if (ch == nullptr)
						continue;
				}

				Vector2F bearing = ch->GetBearing() * Size * Alignment;
				Vector2F advance = ch->GetAdvance() * Size * Alignment;

				auto mesh = ch->GetMesh();
				if (mesh != nullptr)
				{
					Matrix4F charMat(Matrix4F::Identity);
					charMat.SetTranslate({ sumXAdvance + bearing.X, sumYAdvance + bearing.Y, 0 });
					charMat.SetScale({ Size, Size, 1 });
					charMat = Model * charMat;

					DrawCallback(ch->GetMesh(), charMat);
				}

				sumXAdvance += advance.X;

				if (maxYAdvance < ch->GetSize().Y)
					maxYAdvance = ch->GetSize().Y;
			}
		}

		void StringRenderer::Render(DeviceInterface* Device, const Matrix4F& Model, const Matrix4F& Projection, const WString& Text, Font* Font, Material* Material, float32 Size, float32 Alignment)
		{
			auto drawMeshCallback = [&](Mesh* Mesh, const Matrix4F& Model)
			{
				Matrix4F mvp = Projection * Model;

				Device->DrawMesh(Mesh, Model, Matrix4F::Identity, Projection, mvp, Material);
			};

			Render(drawMeshCallback, Model, Text, Font, Size, Alignment);
		}
	}
}