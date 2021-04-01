// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceFactory.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceAssetParser\Private\ResourceAssetParserAllocators.h>
#include <ResourceAssetParser\ImageParser.h>
#include <ResourceAssetParser\TextureParser.h>
#include <ResourceAssetParser\MeshParser.h>
#include <ResourceAssetParser\OBJParser.h>
#include <ResourceAssetParser\TextParser.h>
#include <ResourceAssetParser\ProgramParser.h>
#include <ResourceAssetParser\CompiledProgramParser.h>
#include <ResourceAssetParser\TTFParser.h>
#include <ResourceAssetParser\FontParser.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Private\ProgramCompiler\CompilerHelper.h>
#include <FontSystem\FontManager.h>

namespace Engine
{
	using namespace Containers;
	using namespace Rendering;
	using namespace Rendering::Private::ProgramCompiler;
	using namespace FontSystem;
	using namespace ResourceAssetParser;
	using namespace ResourceAssetParser::Private;

	namespace ResourceSystem
	{
		namespace Private
		{
			const DeviceTypes DEVICE_TYPES[] = { DeviceTypes::OpenGL, DeviceTypes::DirectX12, DeviceTypes::Vulkan };
			const uint8 DEVICE_TYPE_COUNT = _countof(DEVICE_TYPES);

			bool ResourceFactory::CompileTXT(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::TextSettings& Settings)
			{
				TextInfo info;

				TextParser::Parse(InBuffer, info);

				WriteHeader(OutBuffer, Settings.ID, ResourceTypes::Text, TextParser::GetDumpSize(info));

				TextParser::Dump(OutBuffer, info);

				return true;
			}

			Text* ResourceFactory::CreateText(const ByteBuffer& Buffer)
			{
				TextInfo info;
				TextParser::Parse(Buffer, info);

				Text* text = ResourceSystemAllocators::ResourceAllocator_Allocate<Text>();
				ConstructMacro(Text, text, info.Value);

				return text;
			}

			void ResourceFactory::DestroyText(Text* Text)
			{
				ResourceSystemAllocators::ResourceAllocator_Deallocate(Text);
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

			void ResourceFactory::DestroyTexture(Texture* Texture)
			{
				RenderingManager::GetInstance()->GetActiveDevice()->DestroyTexture(Texture);
			}

			Sprite* ResourceFactory::CreateSprite(const ByteBuffer& Buffer)
			{
				TextureInfo info;

				TextureParser::Parse(Buffer, info);

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateSprite(&info);
			}

			void ResourceFactory::DestroySprite(Sprite* Sprite)
			{
				RenderingManager::GetInstance()->GetActiveDevice()->DestroyTexture(ReinterpretCast(Texture*, Sprite));
			}

			bool ResourceFactory::CompilePROGRAM(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::ProgramSettings& Settings)
			{
				ProgramInfo info;

				ProgramParser::Parse(InBuffer, info);

				static const uint16 COMPILED_SHADER_BUFFER_SIZE = 8192;
				static byte compiledVeretexShader[DEVICE_TYPE_COUNT][COMPILED_SHADER_BUFFER_SIZE];
				static byte compiledTessellationShaderShader[DEVICE_TYPE_COUNT][COMPILED_SHADER_BUFFER_SIZE];
				static byte compiledGeometryShader[DEVICE_TYPE_COUNT][COMPILED_SHADER_BUFFER_SIZE];
				static byte compiledFragmentShader[DEVICE_TYPE_COUNT][COMPILED_SHADER_BUFFER_SIZE];
				static byte compiledComputeShader[DEVICE_TYPE_COUNT][COMPILED_SHADER_BUFFER_SIZE];

				CompiledProgramInfo compiledInfos[DEVICE_TYPE_COUNT] = {};
				
				for (uint8 i = 0; i < DEVICE_TYPE_COUNT; ++i)
				{
					CompiledProgramInfo& compiledInfo = compiledInfos[i];

					compiledInfo.VertexShader.Buffer = compiledVeretexShader[i];
					compiledInfo.VertexShader.Size = COMPILED_SHADER_BUFFER_SIZE;
					compiledInfo.TessellationShader.Buffer = compiledTessellationShaderShader[i];
					compiledInfo.TessellationShader.Size = COMPILED_SHADER_BUFFER_SIZE;
					compiledInfo.GeometryShader.Buffer = compiledGeometryShader[i];
					compiledInfo.GeometryShader.Size = COMPILED_SHADER_BUFFER_SIZE;
					compiledInfo.FragmentShader.Buffer = compiledFragmentShader[i];
					compiledInfo.FragmentShader.Size = COMPILED_SHADER_BUFFER_SIZE;
					compiledInfo.ComputeShader.Buffer = compiledComputeShader[i];
					compiledInfo.ComputeShader.Size = COMPILED_SHADER_BUFFER_SIZE;
				}

				auto onError = [&](const String& Message, uint16 Line)
				{
					printf(Message.GetValue());
					//CALL_CALLBACK(IListener, OnError, Message);
				};

				CompilerHelper::Compile(info, DEVICE_TYPES, DEVICE_TYPE_COUNT, compiledInfos, onError);

				WriteHeader(OutBuffer, Settings.ID, ResourceTypes::Program, ProgramParser::GetDumpSize(info));

				for (uint8 i = 0; i < DEVICE_TYPE_COUNT; ++i)
					CompiledProgramParser::Dump(OutBuffer, compiledInfos[i]);

				return true;
			}

			Program* ResourceFactory::CreateProgram(const ByteBuffer& Buffer)
			{
				CompiledProgramInfo info;

				for (uint8 i = 0; i < DEVICE_TYPE_COUNT; ++i)
				{
					CompiledProgramParser::Parse(Buffer, info);


				}

				//info is invlid, based on deviceTYpe

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateProgram(&info);
			}

			void ResourceFactory::DestroyProgram(Program* Program)
			{
				RenderingManager::GetInstance()->GetActiveDevice()->DestroyProgram(Program);
			}

			bool ResourceFactory::CompileOBJ(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::MeshSettings& Settings)
			{
				MeshInfo info(ResourceSystemAllocators::ResourceAllocator);

				OBJParser::Parse(InBuffer, info);

				WriteHeader(OutBuffer, Settings.ID, ResourceTypes::Mesh, MeshParser::GetDumpSize(info));

				MeshParser::Dump(OutBuffer, info);

				return true;
			}

			Mesh* ResourceFactory::CreateMesh(const ByteBuffer& Buffer)
			{
				MeshInfo info;

				MeshParser::Parse(Buffer, info);

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateMesh(&info);
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

				WriteHeader(OutBuffer, Settings.ID, ResourceTypes::Font, FontParser::GetDumpSize(info));

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
				FontManager::GetInstance()->DestroyFont(Font);
			}

			bool ResourceFactory::ReadHeader(const ByteBuffer& Buffer, String* ID, ResourceTypes* ResourceType, uint64* DataSize, byte** Data)
			{
				uint32 index = 0;
				uint32 idLength = Buffer.ReadValue<uint32>(index);
				index += sizeof(uint32);

				*ID = String(ReinterpretCast(cstr, Buffer.ReadValue(index, idLength)), idLength);
				index += idLength;

				*ResourceType = (ResourceTypes)Buffer.ReadValue<int32>(index);
				index += sizeof(int32);

				uint64 dataSize = Buffer.ReadValue<uint64>(index);
				*DataSize = dataSize;
				index += sizeof(uint64);

				*Data = ConstCast(byte*, Buffer.ReadValue(index, dataSize));

				return (dataSize != 0);
			}

			void ResourceFactory::WriteHeader(ByteBuffer& Buffer, const String& ID, ResourceTypes Type, uint64 DataSize)
			{
				Buffer << ID.GetLength();
				Buffer << ID;
				Buffer << (int32)Type;
				Buffer << DataSize;
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

				WriteHeader(OutBuffer, Settings.ID, (Settings.UseType == ImExporter::TextureSettings::UseTypes::Texture ? ResourceTypes::Texture : ResourceTypes::Sprite), TextureParser::GetDumpSize(info));

				TextureParser::Dump(OutBuffer, info);

				PlatformMemory::Free(ConstCast(byte*, info.Data));
			}
		}
	}
}