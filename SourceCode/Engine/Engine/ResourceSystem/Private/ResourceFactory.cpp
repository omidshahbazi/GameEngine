// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceFactory.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceAssetParser\Private\ResourceAssetParserAllocators.h>
#include <ResourceAssetParser\ImageParser.h>
#include <ResourceAssetParser\TextureParser.h>
#include <ResourceAssetParser\MeshParser.h>
#include <ResourceAssetParser\OBJParser.h>
#include <ResourceAssetParser\TextParser.h>
#include <ResourceAssetParser\ShaderParser.h>
#include <ResourceAssetParser\TTFParser.h>
#include <ResourceAssetParser\FontParser.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\ShaderInfo.h>
#include <FontSystem\FontManager.h>

namespace Engine
{
	using namespace Containers;
	using namespace Rendering;
	using namespace FontSystem;
	using namespace ResourceAssetParser;
	using namespace ResourceAssetParser::Private;

	namespace ResourceSystem
	{
		namespace Private
		{
			//TODO: add structure for Font like MeshInfo
			void WriteHeader(ByteBuffer& Buffer, ResourceTypes Type, uint64 DataSize)
			{
				Buffer << (int32)Type;
				Buffer << DataSize;
			}

			bool ResourceFactory::CompileTXT(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::TextSettings& Settings)
			{
				TextInfo info;

				TextParser::Parse(InBuffer, info);

				WriteHeader(OutBuffer, ResourceTypes::Text, TextParser::GetDumpSize(info));

				TextParser::Dump(OutBuffer, info);

				return true;
			}

			Text* ResourceFactory::CreateText(const ByteBuffer& Buffer)
			{
				TextInfo info;
				TextParser::Parse(Buffer, info);

				Text* text = ResourceSystemAllocators::Allocate<Text>(1);
				ConstructMacro(Text, text, info.Value);

				return text;
			}

			void ResourceFactory::DestroyText(Text* Text)
			{
				ResourceSystemAllocators::Deallocate(Text);
			}

			bool ResourceFactory::CompilePNG(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::TextureSettings& Settings)
			{
				CompileImageFile(OutBuffer, InBuffer, Settings);

				return true;
			}

			bool ResourceFactory::CompileJPG(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::TextureSettings& Settings)
			{
				CompileImageFile(OutBuffer, InBuffer, Settings);

				return true;
			}

			Texture* ResourceFactory::CreateTexture(const ByteBuffer& Buffer)
			{
				TextureInfo info;

				TextureParser::Parse(Buffer, info);

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateTexture(&info);
			}

			Sprite* ResourceFactory::CreateSprite(const ByteBuffer& Buffer)
			{
				TextureInfo info;

				TextureParser::Parse(Buffer, info);

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateSprite(&info);
			}

			void ResourceFactory::DestroyTexture(Texture* Texture)
			{
				RenderingManager::GetInstance()->GetActiveDevice()->DestroyTexture(Texture);
			}

			bool ResourceFactory::CompileSHADER(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::ShaderSettings& Settings)
			{
				ShaderInfo info;

				ShaderParser::Parse(InBuffer, info);

				WriteHeader(OutBuffer, ResourceTypes::Shader, ShaderParser::GetDumpSize(info));

				ShaderParser::Dump(OutBuffer, info);

				return true;
			}

			Shader* ResourceFactory::CreateShader(const ByteBuffer& Buffer, String* Message)
			{
				ShaderInfo info;

				ShaderParser::Parse(Buffer, info);

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateShader(&info, Message);
			}

			void ResourceFactory::DestroyShader(Shader* Shader)
			{
				RenderingManager::GetInstance()->GetActiveDevice()->DestroyShader(Shader);
			}

			bool ResourceFactory::CompileOBJ(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::MeshSettings& Settings)
			{
				MeshInfo info(&ResourceSystemAllocators::ResourceAllocator);

				OBJParser::Parse(InBuffer, info);

				WriteHeader(OutBuffer, ResourceTypes::Mesh, MeshParser::GetDumpSize(info));

				MeshParser::Dump(OutBuffer, info);

				return true;
			}

			Mesh* ResourceFactory::CreateMesh(const ByteBuffer& Buffer)
			{
				MeshInfo info;

				MeshParser::Parse(Buffer, info);

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateMesh(&info, GPUBuffer::Usages::StaticDraw);
			}

			void ResourceFactory::DestroyMesh(Mesh* Mesh)
			{
				RenderingManager::GetInstance()->GetActiveDevice()->DestroyMesh(Mesh);
			}

			bool ResourceFactory::CompileTTF(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::FontSettings& Settings)
			{
				FontInfo info;
				info.RenderType = (Settings.RenderType == ImExporter::FontSettings::RenderTypes::Mesh ? Font::RenderTypes::Mesh : Font::RenderTypes::Texture);

				TTFParser::Parse(InBuffer, info);

				WriteHeader(OutBuffer, ResourceTypes::Font, FontParser::GetDumpSize(info));

				FontParser::Dump(OutBuffer, info);

				if (info.RenderType == Font::RenderTypes::Texture)
					ResourceAssetParserAllocators::MeshGeneratorAllocator_Deallocate(ConstCast(byte*, info.TextureInfo.Data));

				return true;
			}

			Font* ResourceFactory::CreateFont(const ByteBuffer& Buffer)
			{
				FontInfo info;

				FontParser::Parse(Buffer, info);

				return FontManager::GetInstance()->CreateFont(&info);
			}

			void ResourceFactory::DestroyFont(Font* Font)
			{
				FontManager* fontMgr = FontManager::GetInstance();

				fontMgr->DestroyFont(Font);
			}

			void ResourceFactory::CompileImageFile(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::TextureSettings& Settings)
			{
				TextureInfo info;

				switch (Settings.Type)
				{
				case ImExporter::TextureSettings::Types::TwoD:
					info.Type = Texture::Types::TwoD;
					break;
				}

				info.Dimension = Vector2I(Settings.Size, Settings.Size);

				switch (Settings.Format)
				{
				case ImExporter::TextureSettings::Formats::Automatic:
					info.Format = Texture::Formats::RGBA8;
					break;

				case ImExporter::TextureSettings::Formats::R8:
				case ImExporter::TextureSettings::Formats::R16:
				case ImExporter::TextureSettings::Formats::R32:
					info.Format = Texture::Formats::R8;
					break;

				case ImExporter::TextureSettings::Formats::RGB8:
				case ImExporter::TextureSettings::Formats::RGB16:
				case ImExporter::TextureSettings::Formats::RGB32:
					info.Format = Texture::Formats::RGB8;
					break;

				case ImExporter::TextureSettings::Formats::RGBA8:
				case ImExporter::TextureSettings::Formats::RGBA16:
				case ImExporter::TextureSettings::Formats::RGBA32:
					info.Format = Texture::Formats::RGBA8;
					break;
				}

				info.Borders = Vector4I(Settings.BorderLeft, Settings.BorderTop, Settings.BorderRight, Settings.BorderBottom);

				ImageParser::Parse(InBuffer, info);

				WriteHeader(OutBuffer, (Settings.UseType == ImExporter::TextureSettings::UseTypes::Texture ? ResourceTypes::Texture : ResourceTypes::Sprite), TextureParser::GetDumpSize(info));

				TextureParser::Dump(OutBuffer, info);

				PlatformMemory::Free(ConstCast(byte*, info.Data));
			}
		}
	}
}