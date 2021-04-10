// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef STRING_RENDERER_H
#define STRING_RENDERER_H
#include <FontSystem\Font.h>
#include <Rendering\DeviceInterface.h>
#include <ResourceSystem\Resource.h>

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
			typedef std::function<void(const Font::Character* Character, const Matrix4F& ModelMatrix)> DrawFunction;

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
			static void Render(DrawFunction DrawFunction, const Matrix4F& Model, const WString& Text, const Info* const Info);

			static void Measure(const WString& Text, const Info* const Info, Vector2F& Size);

		private:
			static void RenderMeshString(DrawFunction DrawFunction, const Matrix4F& Model, const WString& Text, const Info* const Info);
			static void RenderTextureString(DrawFunction DrawFunction, const Matrix4F& Model, const WString& Text, const Info* const Info);

			static void MeasureMesh(const WString& Text, const Info* const Info, Vector2F& Size);
			static void MeasureTexture(const WString& Text, const Info* const Info, Vector2F& Size);
		};
	}
}

#endif