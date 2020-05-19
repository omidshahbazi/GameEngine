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
			typedef std::function<void(Mesh*, TextureHandle*, const Matrix4F&)> DrawCallback;

		public:
			static void Render(DrawCallback DrawCallback, const Matrix4F& Model, const WString& Text, Font* Font, float32 Size, float32 Alignment);

			static void Render(DeviceInterface* Device, const Matrix4F& Model, const Matrix4F& Projection, const WString& Text, Font* Font, Material* Material, float32 Size, float32 Alignment);

		private:
			static void RenderMeshSting(StringRenderer::DrawCallback DrawCallback, const Matrix4F& Model, const WString& Text, Font* Font, float32 Size, float32 Alignment);

			static void RenderTextureString(StringRenderer::DrawCallback DrawCallback, const Matrix4F& Model, const WString& Text, Font* Font, float32 Size, float32 Alignment);

			static void CreateMeshIfIsNull(void);

		public:
			static const String FONT_TEXTURE_CONSTANT_NAME;

		private:
			static Mesh* m_QuadMesh;
		};
	}
}

#endif