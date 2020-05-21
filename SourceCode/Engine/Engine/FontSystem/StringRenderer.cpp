// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <FontSystem\StringRenderer.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Material.h>

namespace Engine
{
	using namespace Rendering;

	namespace FontSystem
	{
		const String StringRenderer::FONT_TEXTURE_CONSTANT_NAME = "_FontTexture";
		const String StringRenderer::FONT_TEXTURE_UV_CONSTANT_NAME = "_FontTextureUV";

		void StringRenderer::Render(DrawCallback DrawCallback, const Matrix4F& Model, const WString& Text, Font* Font, float32 Size, float32 Alignment)
		{
			if (Font->GetRenderType() == Font::RenderTypes::Mesh)
				RenderMeshSting(DrawCallback, Model, Text, Font, Size, Alignment);
			else if (Font->GetRenderType() == Font::RenderTypes::Texture)
				RenderTextureString(DrawCallback, Model, Text, Font, Size, Alignment);
		}

		void StringRenderer::Render(DeviceInterface* Device, const Matrix4F& Model, const Matrix4F& Projection, const WString& Text, Font* Font, Material* Material, float32 Size, float32 Alignment)
		{
			auto drawMeshCallback = [&](const Font::Character* Character, const Matrix4F& Model)
			{
				Matrix4F mvp = Projection * Model;

				if (Font->GetRenderType() == Font::RenderTypes::Texture)
				{
					Pass& pass = Material->GetPasses()[0];
					pass.SetTexture(FONT_TEXTURE_CONSTANT_NAME, Character->GetTexture());
					pass.SetVector4(FONT_TEXTURE_UV_CONSTANT_NAME, Character->GetBounds());
				}

				Device->DrawMesh(Character->GetMesh(), Model, Matrix4F::Identity, Projection, mvp, Material);
			};

			Render(drawMeshCallback, Model, Text, Font, Size, Alignment);
		}

		void StringRenderer::RenderMeshSting(StringRenderer::DrawCallback DrawCallback, const Matrix4F& Model, const WString& Text, Font* Font, float32 Size, float32 Alignment)
		{
			// Glyphs are really large
			// TODO: check out size in loading time after moving to ResourceFactory
			Size /= 40;

			float32 maxYAdvance = 0.0F;
			float32 sumYAdvance = 0.0F;
			float32 sumXAdvance = 0.0F;
			float32 lastVerticalBearing = 0.0F;
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

				Vector2F size = ch->GetSize() * Size * Alignment;
				Vector2F bearing = ch->GetBearing() * Size * Alignment;
				Vector2F advance = ch->GetAdvance() * Size * Alignment;

				auto mesh = ch->GetMesh();
				if (mesh != nullptr)
				{
					Matrix4F charMat(Matrix4F::Identity);
					charMat.SetTranslate({ sumXAdvance + bearing.X, size.Y, 0 });
					charMat.SetScale({ Size, -Size, 1 });
					charMat = Model * charMat;

					DrawCallback(ch, charMat);
				}

				sumXAdvance += advance.X;

				if (maxYAdvance < ch->GetSize().Y)
					maxYAdvance = ch->GetSize().Y;

				lastVerticalBearing = bearing.Y;
			}
		}

		void StringRenderer::RenderTextureString(StringRenderer::DrawCallback DrawCallback, const Matrix4F& Model, const WString& Text, Font* Font, float32 Size, float32 Alignment)
		{
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

				float32 heightRatio = Size / ch->GetSize().Y;

				Vector2F size(ch->GetSize().X * heightRatio, Size);

				Vector2F bearing = ch->GetBearing() * heightRatio * Alignment;
				Vector2F advance = ch->GetAdvance() * heightRatio * Alignment;

				if (ch->GetTexture() != nullptr)
				{
					Matrix4F charMat(Matrix4F::Identity);
					charMat.SetTranslate({ sumXAdvance + bearing.X, sumYAdvance + bearing.Y, 0 });

					charMat.SetScale({ size.X, size.Y, 1 });
					charMat = Model * charMat;

					DrawCallback(ch, charMat);
				}

				sumXAdvance += advance.X;

				if (maxYAdvance < size.Y)
					maxYAdvance = size.Y;
			}
		}
	}
}