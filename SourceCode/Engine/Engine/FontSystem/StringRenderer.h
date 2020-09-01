// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef STRING_RENDERER_H
#define STRING_RENDERER_H
#include <FontSystem\Font.h>
#include <Rendering\DeviceInterface.h>

namespace Engine
{
	using namespace Common;
	using namespace Rendering;
	using namespace MathContainers;

	namespace FontSystem
	{
		class FONTSYSTEM_API StringRenderer
		{
		public:
			typedef std::function<void(const Font::Character*, const Matrix4F&)> DrawCallback;

			struct FONTSYSTEM_API Info
			{
			public:
				Font* Font;
				float32 Size;
				float32 Alignment;
				bool MultiLine;
				float32 CharacterSpacing;
				float32 LineSpacing;
			};

		public:
			static void Render(DrawCallback DrawCallback, const Matrix4F& Model, const WString& Text, const Info* const Info);

			static void Render(DeviceInterface* Device, const Matrix4F& Model, const Matrix4F& Projection, const WString& Text, Material* Material, const Info* const Info);

			static void Measure(const WString& Text, const Info* const Info, Vector2F& Size);

		private:
			static void RenderMeshSting(StringRenderer::DrawCallback DrawCallback, const Matrix4F& Model, const WString& Text, const Info* const Info);
			static void RenderTextureString(StringRenderer::DrawCallback DrawCallback, const Matrix4F& Model, const WString& Text, const Info* const Info);

			static void MeasureMesh(const WString& Text, const Info* const Info, Vector2F& Size);
			static void MeasureTexture(const WString& Text, const Info* const Info, Vector2F& Size);

		public:
			static cstr FONT_TEXTURE_CONSTANT_NAME;
			static cstr FONT_TEXTURE_UV_CONSTANT_NAME;
		};
	}
}

#endif