// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <FontSystem\StringRenderer.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Material.h>

namespace Engine
{
	using namespace Rendering;

	namespace FontSystem
	{
		cstr StringRenderer::FONT_TEXTURE_CONSTANT_NAME = "_FontTexture";
		cstr StringRenderer::FONT_TEXTURE_UV_CONSTANT_NAME = "_FontTextureUV";

		void StringRenderer::Render(DrawFunction DrawFunction, const Matrix4F& Model, const WString& Text, const Info* const Info)
		{
			if (Info->Font == nullptr)
				return;

			if (Info->Font->GetRenderType() == Font::RenderTypes::Mesh)
				RenderMeshSting(DrawFunction, Model, Text, Info);
			else if (Info->Font->GetRenderType() == Font::RenderTypes::Texture)
				RenderTextureString(DrawFunction, Model, Text, Info);
		}

		void StringRenderer::Render(DeviceInterface* Device, const Matrix4F& Model, const Matrix4F& Projection, const WString& Text, Material* Material, const Info* const Info)
		{
			auto drawMeshCallback = [&](const Font::Character* Character, const Matrix4F& Model)
			{
				Matrix4F mvp = Projection * Model;

				if (Info->Font->GetRenderType() == Font::RenderTypes::Texture)
				{
					Pass& pass = Material->GetPasses()[0];

					static Pass::ConstantHash ConstantHash_font_tex = Pass::GetHash(FONT_TEXTURE_CONSTANT_NAME);
					static Pass::ConstantHash ConstantHash_font_tex_uv = Pass::GetHash(FONT_TEXTURE_UV_CONSTANT_NAME);

					pass.SetTexture(ConstantHash_font_tex, Character->GetTexture());
					//pass.SetVector4(ConstantHash_font_tex_uv, Character->GetBounds());
				}

				Device->DrawMesh(Character->GetMesh(), Model, Matrix4F::Identity, Projection, mvp, Material);
			};

			Render(drawMeshCallback, Model, Text, Info);
		}

		void StringRenderer::Measure(const WString& Text, const Info* const Info, Vector2F& Size)
		{
			if (Info->Font == nullptr)
				return;

			if (Info->Font->GetRenderType() == Font::RenderTypes::Mesh)
				MeasureMesh(Text, Info, Size);
			else if (Info->Font->GetRenderType() == Font::RenderTypes::Texture)
				MeasureTexture(Text, Info, Size);
		}

		//LOTODO: Change this like texture version
		void StringRenderer::RenderMeshSting(DrawFunction DrawFunction, const Matrix4F& Model, const WString& Text, const Info* const Info)
		{
			if (Info->Font == nullptr)
				return;

			//LOTODO: Glyphs are really large, do something to remove this dummy division
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

					DrawFunction(ch, charMat);
				}

				sumXAdvance += advance.X;

				if (maxYAdvance < ch->GetSize().Y)
					maxYAdvance = ch->GetSize().Y;

				lastVerticalBearing = bearing.Y;
			}
		}

		void StringRenderer::RenderTextureString(DrawFunction DrawFunction, const Matrix4F& Model, const WString& Text, const Info* const Info)
		{
			if (Info->Font == nullptr)
				return;

			float32 heightRatio = Info->Size / Info->Font->GetSize();
			float32 lineSpacing = Info->LineSpacing * heightRatio;

			float32 sumYAdvance = 0.0F;
			float32 sumXAdvance = 0.0F;

			for (uint32 i = 0; i < Text.GetLength(); ++i)
			{
				char16 charCode = Text[i];

				if (Info->MultiLine && (charCode == '\n' || charCode == '\r'))
				{
					sumYAdvance += Info->Size + lineSpacing;
					sumXAdvance = 0;

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

				Vector2F size = ch->GetSize() * heightRatio * Info->Alignment;
				Vector2F halfSize = size * 0.5F;
				Vector2F bearing = ch->GetBearing() * heightRatio * Info->Alignment;
				Vector2F advance = (ch->GetAdvance() + Info->CharacterSpacing) * heightRatio * Info->Alignment;

				for (int8 j = 0; j < repeatCount; ++j)
				{
					if (ch->GetTexture() != nullptr)
					{
						Matrix4F charMat(Matrix4F::Identity);
						charMat.SetTranslate({ sumXAdvance + halfSize.X + bearing.X, sumYAdvance - halfSize.Y + (size.Y - bearing.Y), 0 });

						charMat.SetScale({ size.X, size.Y, 1 });
						charMat = Model * charMat;

						DrawFunction(ch, charMat);
					}

					sumXAdvance += advance.X;
				}
			}
		}

		//LOTODO: Fill MeasureMesh
		void StringRenderer::MeasureMesh(const WString& Text, const Info* const Info, Vector2F& Size)
		{
			if (Info->Font == nullptr)
				return;

		}

		void StringRenderer::MeasureTexture(const WString& Text, const Info* const Info, Vector2F& Size)
		{
			if (Info->Font == nullptr)
				return;

			float32 heightRatio = Info->Size / Info->Font->GetSize();
			float32 lineSpacing = Info->LineSpacing * heightRatio;

			float32 sumYAdvance = Info->Size + lineSpacing;
			float32 sumXAdvance = 0.0F;

			for (uint32 i = 0; i < Text.GetLength(); ++i)
			{
				char16 charCode = Text[i];

				if (Info->MultiLine && (charCode == '\n' || charCode == '\r'))
				{
					sumYAdvance += Info->Size + lineSpacing;
					sumXAdvance = 0;

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

				Vector2F size = ch->GetSize() * heightRatio * Info->Alignment;
				Vector2F bearing = ch->GetBearing() * heightRatio * Info->Alignment;
				Vector2F advance = (ch->GetAdvance() + Info->CharacterSpacing) * heightRatio * Info->Alignment;

				for (int8 j = 0; j < repeatCount; ++j)
					sumXAdvance += advance.X;

				sumYAdvance += advance.Y;

				if (Size.X < sumXAdvance)
					Size.X = sumXAdvance;
			}

			if (Size.Y < sumYAdvance)
				Size.Y = sumYAdvance;
		}
	}
}