// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef IMPORTER_EXPORTER_H
#define IMPORTER_EXPORTER_H

#include <Containers\Strings.h>
#include <Containers\GUID.h>
#include <Common\Definitions.h>
#include <Platform\PlatformFile.h>
#include <Reflection\PropertyType.h>
#include <ImporterExporter.Reflection.h>

namespace Engine
{
	using namespace Containers;
	using namespace Platform;

	namespace ResourceImportExport
	{
		REFLECTION_STRUCT()
		class RESOURCEIMPORTEXPORT_API ImporterExporter
		{
			IMPORTEREXPORTER_OBJECT()

		public:
			static cwstr META_EXTENSION;
			static const int8 META_FILE_FORMAT_VERSION;

		public:
			REFLECTION_STRUCT()
			struct RESOURCEIMPORTEXPORT_API Settings
			{
				SETTINGS_OBJECT()

			public:
				Settings(void) :
					FileFormatVersion(0)
				{
				}

			public:
				REFLECTION_PROPERTY()
					String ID;

				REFLECTION_PROPERTY()
					int8 FileFormatVersion;
			};

			REFLECTION_STRUCT()
				struct RESOURCEIMPORTEXPORT_API TextSettings : public Settings
			{
				TEXTSETTINGS_OBJECT()

			public:
				TextSettings(void)
				{
				}
			};

			REFLECTION_STRUCT()
				struct RESOURCEIMPORTEXPORT_API TextureSettings : public Settings
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
				struct RESOURCEIMPORTEXPORT_API ProgramSettings : public Settings
			{
				PROGRAMSETTINGS_OBJECT()

			public:
				ProgramSettings(void)
				{
				}
			};

			REFLECTION_STRUCT()
				struct RESOURCEIMPORTEXPORT_API MeshSettings : public Settings
			{
				MESHSETTINGS_OBJECT()

			public:
				MeshSettings(void)
				{
				}
			};

			REFLECTION_STRUCT()
				struct RESOURCEIMPORTEXPORT_API FontSettings : public Settings
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
			static bool Invalidate(const WString& FilePath);

			template<typename T>
			static bool Import(const WString& FilePath, T* Settings)
			{
				WString metaFilePath = GetMetaFilePath(FilePath);

				if (!PlatformFile::Exists(metaFilePath.GetValue()))
					return false;

				Reflection::TypeList properties;
				GetProperties(T::GetType(), properties);

				ReadMetaFile(metaFilePath, properties, Settings);

				return true;
			}

			template<typename T>
			static bool Export(const WString& FilePath, T* Settings)
			{
				if (Settings->ID.GetLength() == 0)
					Settings->ID = GUID::Create().ToString();

				Settings->FileFormatVersion = META_FILE_FORMAT_VERSION;

				Reflection::TypeList properties;
				GetProperties(T::GetType(), properties);

				WriteMetaFile(GetMetaFilePath(FilePath), properties, Settings);

				return true;
			}

			static bool ImportText(const WString& FilePath, TextSettings* Settings)
			{
				return Import(FilePath, Settings);
			}
			static bool ExportText(const WString& FilePath, TextSettings* Settings)
			{
				return Export(FilePath, Settings);
			}

			static bool ImportTexture(const WString& FilePath, TextureSettings* Settings)
			{
				return Import(FilePath, Settings);
			}
			static bool ExportTexture(const WString& FilePath, TextureSettings* Settings)
			{
				return Export(FilePath, Settings);
			}

			static bool ImportProgram(const WString& FilePath, ProgramSettings* Settings)
			{
				return Import(FilePath, Settings);
			}
			static bool ExportProgram(const WString& FilePath, ProgramSettings* Settings)
			{
				return Export(FilePath, Settings);
			}

			static bool ImportMesh(const WString& FilePath, MeshSettings* Settings)
			{
				return Import(FilePath, Settings);
			}
			static bool ExportMesh(const WString& FilePath, MeshSettings* Settings)
			{
				return Export(FilePath, Settings);
			}

			static bool ImportFont(const WString& FilePath, FontSettings* Settings)
			{
				return Import(FilePath, Settings);
			}
			static bool ExportFont(const WString& FilePath, FontSettings* Settings)
			{
				return Export(FilePath, Settings);
			}

			static WString GetResourceFilePath(const WString& FilePath);

		private:
			static WString GetMetaFilePath(const WString& FilePath);

			static void GetProperties(const Reflection::DataStructureType& Type, Reflection::TypeList& Properties);
			static void ReadMetaFile(const WString& FilePath, Reflection::TypeList& Properties, void* SettingObject);
			static void WriteMetaFile(const WString& FilePath, Reflection::TypeList& Properties, const void* SettingObject);
		};
	}
}
#endif