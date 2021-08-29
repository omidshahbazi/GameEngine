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
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Containers;
	using namespace Debugging;
	using namespace Rendering;
	using namespace Rendering::Private::ProgramCompiler;
	using namespace FontSystem;
	using namespace ResourceAssetParser;
	using namespace ResourceAssetParser::Private;

	namespace ResourceSystem
	{
		namespace Private
		{
			byte COMPILED_VERETEX_SHADER[DEVICE_TYPE_COUNT][DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE];
			byte COMPILED_TESSELLATION_SHADER[DEVICE_TYPE_COUNT][DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE];
			byte COMPILED_GEOMETRY_SHADER[DEVICE_TYPE_COUNT][DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE];
			byte COMPILED_FRAGMENT_SHADER[DEVICE_TYPE_COUNT][DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE];
			byte COMPILED_COMPUTE_SHADER[DEVICE_TYPE_COUNT][DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE];

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

				CompiledProgramInfo compiledInfos[DEVICE_TYPE_COUNT] = {};
				for (uint8 i = 0; i < DEVICE_TYPE_COUNT; ++i)
				{
					CompiledProgramInfo& compiledInfo = compiledInfos[i];

					compiledInfo.VertexShader.Buffer = COMPILED_VERETEX_SHADER[i];
					compiledInfo.VertexShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
					compiledInfo.TessellationShader.Buffer = COMPILED_TESSELLATION_SHADER[i];
					compiledInfo.TessellationShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
					compiledInfo.GeometryShader.Buffer = COMPILED_GEOMETRY_SHADER[i];
					compiledInfo.GeometryShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
					compiledInfo.FragmentShader.Buffer = COMPILED_FRAGMENT_SHADER[i];
					compiledInfo.FragmentShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
					compiledInfo.ComputeShader.Buffer = COMPILED_COMPUTE_SHADER[i];
					compiledInfo.ComputeShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
				}

				if (!CompilerHelper::Compile(info, DEVICE_TYPES, DEVICE_TYPE_COUNT, compiledInfos))
				{
					bool foundAnything = false;

					for (uint8 i = 0; i < DEVICE_TYPE_COUNT; ++i)
					{
						auto& compiledInfo = compiledInfos[i];

						if (compiledInfo.ErrorMessage != String::Empty)
							continue;

						CoreDebugLogError(Categories::ProgramCompiler, "Compiling program has failed for {%s}", compiledInfo.ErrorMessage.GetValue());

						foundAnything = true;
					}

					if (!foundAnything)
						CoreDebugLogError(Categories::ProgramCompiler, "Compiling a program has failed for the following reasons");
				}

				uint64 dumpSize = 0;
				for (uint8 i = 0; i < DEVICE_TYPE_COUNT; ++i)
					dumpSize += CompiledProgramParser::GetDumpSize(compiledInfos[i]);

				WriteHeader(OutBuffer, ResourceTypes::Program, dumpSize);

				for (uint8 i = 0; i < DEVICE_TYPE_COUNT; ++i)
					CompiledProgramParser::Dump(OutBuffer, DEVICE_TYPES[i], compiledInfos[i]);

				return true;
			}

			Program* ResourceFactory::CreateProgram(const ByteBuffer& Buffer)
			{
				DeviceInterface* device = RenderingManager::GetInstance()->GetActiveDevice();

				for (uint8 i = 0; i < DEVICE_TYPE_COUNT; ++i)
				{
					CompiledProgramInfo compiledInfos;

					compiledInfos.VertexShader.Buffer = COMPILED_VERETEX_SHADER[0];
					compiledInfos.VertexShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
					compiledInfos.TessellationShader.Buffer = COMPILED_TESSELLATION_SHADER[0];
					compiledInfos.TessellationShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
					compiledInfos.GeometryShader.Buffer = COMPILED_GEOMETRY_SHADER[0];
					compiledInfos.GeometryShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
					compiledInfos.FragmentShader.Buffer = COMPILED_FRAGMENT_SHADER[0];
					compiledInfos.FragmentShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
					compiledInfos.ComputeShader.Buffer = COMPILED_COMPUTE_SHADER[0];
					compiledInfos.ComputeShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;

					DeviceTypes deviceType;
					CompiledProgramParser::Parse(Buffer, deviceType, compiledInfos);

					if (deviceType != device->GetType())
						continue;

					return device->CreateProgram(&compiledInfos);
				}

				return nullptr;
			}

			void ResourceFactory::DestroyProgram(Program* Program)
			{
				RenderingManager::GetInstance()->GetActiveDevice()->DestroyProgram(Program);
			}

			bool ResourceFactory::CompileOBJ(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::MeshSettings& Settings)
			{
				MeshInfo info(ResourceSystemAllocators::ResourceAllocator);

				OBJParser::Parse(InBuffer, info);

				WriteHeader(OutBuffer, ResourceTypes::Mesh, MeshParser::GetDumpSize(info));

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
				FontManager::GetInstance()->DestroyFont(Font);
			}

			bool ResourceFactory::ReadHeader(const ByteBuffer& Buffer, ResourceTypes* ResourceType, uint64* DataSize, byte** Data)
			{
				*ResourceType = (ResourceTypes)Buffer.ReadValue<int32>();

				uint64 dataSize = Buffer.ReadValue<uint64>();
				*DataSize = dataSize;

				*Data = ConstCast(byte*, Buffer.ReadValue(dataSize));

				return (dataSize != 0);
			}

			void ResourceFactory::WriteHeader(ByteBuffer& Buffer, ResourceTypes Type, uint64 DataSize)
			{
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
					info.Format = Formats::RGBA8;
					break;

				case ImExporter::TextureSettings::Formats::R8:
				case ImExporter::TextureSettings::Formats::R16:
				case ImExporter::TextureSettings::Formats::R32:
					info.Format = Formats::R8;
					break;

				case ImExporter::TextureSettings::Formats::RGB8:
				case ImExporter::TextureSettings::Formats::RGB16:
				case ImExporter::TextureSettings::Formats::RGB32:
					info.Format = Formats::RGB8;
					break;

				case ImExporter::TextureSettings::Formats::RGBA8:
				case ImExporter::TextureSettings::Formats::RGBA16:
				case ImExporter::TextureSettings::Formats::RGBA32:
					info.Format = Formats::RGBA8;
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