// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef IM_EXPORTER_H
#define IM_EXPORTER_H

#include <Containers\Strings.h>
#include <Common\Definitions.h>
#include <ImExporter.Reflection.h>

namespace Engine
{
	using namespace Containers;

	namespace ResourceSystem
	{
		namespace Private
		{
			REFLECTION_STRUCT()
				class RESOURCESYSTEM_API ImExporter
			{
				IMEXPORTER_OBJECT()

			public:
				REFLECTION_STRUCT()
					struct Settings
				{
					SETTINGS_OBJECT()

				public:
					Settings(void) :
						LastWriteTime(0),
						FileFormatVersion(0)
					{
					}

				public:
					REFLECTION_PROPERTY()
						String ID;

					REFLECTION_PROPERTY()
						int64 LastWriteTime;

					REFLECTION_PROPERTY()
						int8 FileFormatVersion;
				};

				REFLECTION_STRUCT()
					struct TextSettings : public Settings
				{
					TEXTSETTINGS_OBJECT()

				public:
					TextSettings(void)
					{
					}
				};

				REFLECTION_STRUCT()
					struct TextureSettings : public Settings
				{
					TEXTURESETTINGS_OBJECT()

				public:
					REFLECTION_ENUM()
						enum class Types
					{
						TwoD = 0
					};

					REFLECTION_ENUM()
						enum class UseTypes
					{
						Texture = 0,
						Sprite
					};

					REFLECTION_ENUM()
						enum class Formats
					{
						Automatic = 0,
						R8,
						R16,
						R32,
						RGB8,
						RGB16,
						RGB32,
						RGBA8,
						RGBA16,
						RGBA32
					};

				public:
					TextureSettings(void) :
						Type(Types::TwoD),
						UseType(UseTypes::Texture),
						Format(Formats::Automatic),
						Size(0),
						BorderRight(0),
						BorderLeft(0),
						BorderTop(0),
						BorderBottom(0)
					{
					}

				public:
					REFLECTION_PROPERTY()
						Types Type;

					REFLECTION_PROPERTY()
						UseTypes UseType;

					REFLECTION_PROPERTY()
						Formats Format;

					REFLECTION_PROPERTY()
						int32 Size;

					REFLECTION_PROPERTY()
						int32 BorderRight;

					REFLECTION_PROPERTY()
						int32 BorderLeft;

					REFLECTION_PROPERTY()
						int32 BorderTop;

					REFLECTION_PROPERTY()
						int32 BorderBottom;
				};

				REFLECTION_STRUCT()
					struct ProgramSettings : public Settings
				{
					PROGRAMSETTINGS_OBJECT()

				public:
					ProgramSettings(void)
					{
					}
				};

				REFLECTION_STRUCT()
					struct MeshSettings : public Settings
				{
					MESHSETTINGS_OBJECT()

				public:
					MeshSettings(void)
					{
					}
				};

				REFLECTION_STRUCT()
					struct FontSettings : public Settings
				{
					FONTSETTINGS_OBJECT()

				public:
					REFLECTION_ENUM()
						enum class RenderTypes
					{
						Mesh = 0,
						Texture
					};

				public:
					FontSettings(void) :
						RenderType(RenderTypes::Mesh)
					{
					}

				public:
					REFLECTION_PROPERTY()
						RenderTypes RenderType;
				};

			public:
				static bool ImportText(const WString& FilePath, TextSettings* Settings);
				static bool ExportText(const WString& FilePath, TextSettings* Settings);

				static bool ImportTexture(const WString& FilePath, TextureSettings* Settings);
				static bool ExportTexture(const WString& FilePath, TextureSettings* Settings);

				static bool ImportProgram(const WString& FilePath, ProgramSettings* Settings);
				static bool ExportProgram(const WString& FilePath, ProgramSettings* Settings);

				static bool ImportMesh(const WString& FilePath, MeshSettings* Settings);
				static bool ExportMesh(const WString& FilePath, MeshSettings* Settings);

				static bool ImportFont(const WString& FilePath, FontSettings* Settings);
				static bool ExportFont(const WString& FilePath, FontSettings* Settings);
			};
		}
	}
}
#endif