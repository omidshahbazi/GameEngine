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

		void StringRenderer::Render(DrawCallback DrawCallback, const Matrix4F& Model, const WString& Text, const Info* const Info)
		{
			if (Info->Font->GetRenderType() == Font::RenderTypes::Mesh)
				RenderMeshSting(DrawCallback, Model, Text, Info);
			else if (Info->Font->GetRenderType() == Font::RenderTypes::Texture)
				RenderTextureString(DrawCallback, Model, Text, Info);
		}

		void StringRenderer::Render(DeviceInterface* Device, const Matrix4F& Model, const Matrix4F& Projection, const WString& Text, Material* Material, const Info* const Info)
		{
			auto drawMeshCallback = [&](const Font::Character* Character, const Matrix4F& Model)
			{
				Matrix4F mvp = Projection * Model;

				if (Info->Font->GetRenderType() == Font::RenderTypes::Texture)
				{
					Pass& pass = Material->GetPasses()[0];
					pass.SetTexture(FONT_TEXTURE_CONSTANT_NAME, Character->GetTexture());
					pass.SetVector4(FONT_TEXTURE_UV_CONSTANT_NAME, Character->GetBounds());
				}

				Device->DrawMesh(Character->GetMesh(), Model, Matrix4F::Identity, Projection, mvp, Material);
			};

			Render(drawMeshCallback, Model, Text, Info);
		}

		// TODO: change this like texture version
		void StringRenderer::RenderMeshSting(StringRenderer::DrawCallback DrawCallback, const Matrix4F& Model, const WString& Text, const Info* const Info)
		{
			// Glyphs are really large
			// TODO: check out size in loading time after moving to ResourceFactory
			float32 renderSize = Info->Size / 40;

			float32 maxYAdvance = 0.0F;
			float32 sumYAdvance = 0.0F;
			float32 sumXAdvance = 0.0F;
			float32 lastVerticalBearing = 0.0F;
			for (uint32 i = 0; i < Text.GetLength(); ++i)
			{
				char16 charCode = Text[i];

				if (charCode == '\n' || charCode == '\r')
				{
					sumYAdvance -= maxYAdvance + Info->LineSpacing;
					sumXAdvance = 0;
					maxYAdvance = 0;

					continue;
				}

				Font::Character* ch = Info->Font->GetCharacter(Text[i]);

				if (ch == nullptr)
				{
					ch = Info->Font->GetCharacter('?');

					if (ch == nullptr)
						continue;
				}

				Vector2F size = ch->GetSize() * renderSize * Info->Alignment;
				Vector2F bearing = ch->GetBearing() * renderSize * Info->Alignment;
				Vector2F advance = ch->GetAdvance() * renderSize * Info->Alignment;

				auto mesh = ch->GetMesh();
				if (mesh != nullptr)
				{
					Matrix4F charMat(Matrix4F::Identity);
					charMat.SetTranslate({ sumXAdvance + bearing.X, size.Y, 0 });
					charMat.SetScale({ renderSize, -renderSize, 1 });
					charMat = Model * charMat;

					DrawCallback(ch, charMat);
				}

				sumXAdvance += advance.X;

				if (maxYAdvance < ch->GetSize().Y)
					maxYAdvance = ch->GetSize().Y;

				lastVerticalBearing = bearing.Y;
			}
		}

		void StringRenderer::RenderTextureString(StringRenderer::DrawCallback DrawCallback, const Matrix4F& Model, const WString& Text, const Info* const Info)
		{
			float32 maxYAdvance = 0.0F;
			float32 sumYAdvance = 0.0F;
			float32 sumXAdvance = 0.0F;
			for (uint32 i = 0; i < Text.GetLength(); ++i)
			{
				char16 charCode = Text[i];

				if (Info->MultiLine && (charCode == '\n' || charCode == '\r'))
				{
					sumYAdvance += maxYAdvance + Info->LineSpacing;
					sumXAdvance = 0;
					maxYAdvance = 0;

					continue;
				}

				uint8 repeatCount = 1;
				if (charCode == '\t')
				{
					charCode = ' ';
					repeatCount = 4;
				}

				Font::Character* ch = Info->Font->GetCharacter(charCode);

				if (ch == nullptr)
				{
					ch = Info->Font->GetCharacter('?');

					if (ch == nullptr)
						continue;
				}

				for (int8 j = 0; j < repeatCount; ++j)
				{
					float32 heightRatio = Info->Size / Info->Font->GetSize();

					Vector2F size = ch->GetSize() * heightRatio;
					Vector2F bearing = ch->GetBearing() * heightRatio * Info->Alignment;
					Vector2F advance = ch->GetAdvance() * heightRatio * Info->Alignment;

					if (ch->GetTexture() != nullptr)
					{
						Matrix4F charMat(Matrix4F::Identity);
						charMat.SetTranslate({ sumXAdvance + bearing.X, sumYAdvance + (size.Y - bearing.Y), 0 });

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
}