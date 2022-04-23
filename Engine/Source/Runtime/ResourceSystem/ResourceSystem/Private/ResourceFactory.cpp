// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceFactory.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ImageAssetParser\ImageParser.h>
#include <TextureAssetParser\TextureParser.h>
#include <MeshAssetParser\MeshParser.h>
#include <OBJAssetParser\OBJParser.h>
#include <TextAssetParser\TextParser.h>
#include <ProgramAssetParser\ProgramParser.h>
#include <CompiledProgramAssetParser\CompiledProgramParser.h>
#include <TTFAssetParser\TTFParser.h>
#include <TTFAssetParser\Private\TTFParserAllocators.h>
#include <FontAssetParser\FontParser.h>
#include <RenderSystem\RenderManager.h>
#include <ProgramCompiler\Compiler.h>
#include <FontSystem\FontManager.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Containers;
	using namespace Debugging;
	using namespace ProgramCompiler;
	using namespace FontSystem;
	using namespace ResourceAssetParser;
	using namespace ResourceAssetParser::Private;

	namespace ResourceSystem
	{
		namespace Private
		{
			byte COMPILED_VERETEX_SHADER[DEVICE_TYPE_COUNT][DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE];
			byte COMPILED_HULL_SHADER[DEVICE_TYPE_COUNT][DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE];
			byte COMPILED_DOMAIN_SHADER[DEVICE_TYPE_COUNT][DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE];
			byte COMPILED_GEOMETRY_SHADER[DEVICE_TYPE_COUNT][DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE];
			byte COMPILED_FRAGMENT_SHADER[DEVICE_TYPE_COUNT][DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE];
			byte COMPILED_COMPUTE_SHADER[DEVICE_TYPE_COUNT][DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE];

			bool ResourceFactory::CompileTXT(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImporterExporter::TextSettings& Settings)
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

			bool ResourceFactory::CompilePNG(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImporterExporter::TextureSettings& Settings)
			{
				CompileImageFile(OutBuffer, InBuffer, Settings);

				return true;
			}

			bool ResourceFactory::CompileJPG(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImporterExporter::TextureSettings& Settings)
			{
				CompileImageFile(OutBuffer, InBuffer, Settings);

				return true;
			}

			Texture* ResourceFactory::CreateTexture(const ByteBuffer& Buffer)
			{
				TextureInfo info;

				TextureParser::Parse(Buffer, info);

				return RenderManager::GetInstance()->GetDevice()->CreateTexture(&info);
			}

			void ResourceFactory::DestroyTexture(Texture* Texture)
			{
				RenderManager::GetInstance()->GetDevice()->DestroyTexture(Texture);
			}

			Sprite* ResourceFactory::CreateSprite(const ByteBuffer& Buffer)
			{
				TextureInfo info;

				TextureParser::Parse(Buffer, info);

				return RenderManager::GetInstance()->GetDevice()->CreateSprite(&info);
			}

			void ResourceFactory::DestroySprite(Sprite* Sprite)
			{
				RenderManager::GetInstance()->GetDevice()->DestroyTexture(ReinterpretCast(Texture*, Sprite));
			}

			bool ResourceFactory::CompilePROGRAM(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImporterExporter::ProgramSettings& Settings)
			{
				ProgramInfo info;
				ProgramParser::Parse(InBuffer, info);

				info.Defines.AddRange(Settings.Defines);
				info.DebugMode = Settings.DebugMode;

				CompiledProgramInfo compiledInfos[DEVICE_TYPE_COUNT] = {};
				for (uint8 i = 0; i < DEVICE_TYPE_COUNT; ++i)
				{
					CompiledProgramInfo& compiledInfo = compiledInfos[i];

					compiledInfo.VertexShader.Buffer = COMPILED_VERETEX_SHADER[i];
					compiledInfo.VertexShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;

					compiledInfo.HullShader.Buffer = COMPILED_HULL_SHADER[i];
					compiledInfo.HullShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;

					compiledInfo.DomainShader.Buffer = COMPILED_DOMAIN_SHADER[i];
					compiledInfo.DomainShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
					
					compiledInfo.GeometryShader.Buffer = COMPILED_GEOMETRY_SHADER[i];
					compiledInfo.GeometryShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
					
					compiledInfo.FragmentShader.Buffer = COMPILED_FRAGMENT_SHADER[i];
					compiledInfo.FragmentShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
					
					compiledInfo.ComputeShader.Buffer = COMPILED_COMPUTE_SHADER[i];
					compiledInfo.ComputeShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
				}

				if (!Compiler::Compile(info, DEVICE_TYPES, DEVICE_TYPE_COUNT, compiledInfos))
				{
					for (uint8 i = 0; i < DEVICE_TYPE_COUNT; ++i)
					{
						auto& compiledInfo = compiledInfos[i];

						if (compiledInfo.ErrorMessage == String::Empty)
							continue;

						THROW_FULL_EXCEPTION(Categories::ProgramCompiler, "Compiling program has failed", compiledInfo.ErrorMessage);
					}

					THROW_EXCEPTION(Categories::ProgramCompiler, "Compiling a program has failed for unknown reason");

					return false;
				}

				uint64 dumpSize = 0;
				dumpSize += CompiledProgramParser::GetDumpSize(info.Source);
				for (uint8 i = 0; i < DEVICE_TYPE_COUNT; ++i)
					dumpSize += CompiledProgramParser::GetDumpSize(compiledInfos[i]);

				WriteHeader(OutBuffer, ResourceTypes::Program, dumpSize);

				CompiledProgramParser::Dump(OutBuffer, info.Source);
				for (uint8 i = 0; i < DEVICE_TYPE_COUNT; ++i)
					CompiledProgramParser::Dump(OutBuffer, DEVICE_TYPES[i], compiledInfos[i]);

				return true;
			}

			String ResourceFactory::GetProgramSource(const ByteBuffer& Buffer)
			{
				ResourceTypes type;
				uint64 dataSize;
				byte* data;
				ResourceFactory::ReadHeader(Buffer, &type, &dataSize, &data);

				ByteBuffer buffer(data, dataSize);

				String source;
				CompiledProgramParser::Parse(buffer, source);

				return source;
			}

			Program* ResourceFactory::CreateProgram(const ByteBuffer& Buffer)
			{
				String source;
				CompiledProgramParser::Parse(Buffer, source);

				DeviceInterface* device = RenderManager::GetInstance()->GetDevice();

				for (uint8 i = 0; i < DEVICE_TYPE_COUNT; ++i)
				{
					CompiledProgramInfo compiledInfos;

					compiledInfos.VertexShader.Buffer = COMPILED_VERETEX_SHADER[0];
					compiledInfos.VertexShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;

					compiledInfos.HullShader.Buffer = COMPILED_HULL_SHADER[0];
					compiledInfos.HullShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;

					compiledInfos.DomainShader.Buffer = COMPILED_DOMAIN_SHADER[0];
					compiledInfos.DomainShader.Size = DeviceInterface::DEFAULT_COMPILED_SHADER_BUFFER_SIZE;
					
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
				RenderManager::GetInstance()->GetDevice()->DestroyProgram(Program);
			}

			bool ResourceFactory::CompileOBJ(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImporterExporter::MeshSettings& Settings)
			{
				MeshInfo info(ResourceSystemAllocators::ResourceAllocator);

				OBJParser::Parse(InBuffer, info);

				WriteHeader(OutBuffer, ResourceTypes::Mesh, MeshParser::GetDumpSize(info));

				MeshParser::Dump(OutBuffer, info);

				return true;
			}

			Mesh* ResourceFactory::CreateMesh(const ByteBuffer& Buffer)
			{
				MeshInfo info(ResourceSystemAllocators::ResourceAllocator);

				MeshParser::Parse(Buffer, info);

				return RenderManager::GetInstance()->GetDevice()->CreateMesh(&info);
			}

			void ResourceFactory::DestroyMesh(Mesh* Mesh)
			{
				RenderManager::GetInstance()->GetDevice()->DestroyMesh(Mesh);
			}

			bool ResourceFactory::CompileTTF(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImporterExporter::FontSettings& Settings)
			{
				FontInfo info;
				info.RenderType = (Settings.RenderType == ImporterExporter::FontSettings::RenderTypes::Mesh ? FontRenderTypes::Mesh : FontRenderTypes::Texture);

				TTFParser::Parse(InBuffer, info);

				WriteHeader(OutBuffer, ResourceTypes::Font, FontParser::GetDumpSize(info));

				FontParser::Dump(OutBuffer, info);

				if (info.RenderType == FontRenderTypes::Texture)
					TTFParserAllocators::MeshGeneratorAllocator_Deallocate(ConstCast(byte*, info.TextureInfo.Data));

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

			void ResourceFactory::CompileImageFile(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImporterExporter::TextureSettings& Settings)
			{
				TextureInfo info;

				switch (Settings.Type)
				{
				case ImporterExporter::TextureSettings::Types::TwoD:
					info.Type = TextureTypes::TwoD;
					break;
				}

				info.Dimension = Vector2I(Settings.Size, Settings.Size);

				switch (Settings.Format)
				{
				case ImporterExporter::TextureSettings::Formats::Automatic:
					info.Format = Formats::RGBA8;
					break;

				case ImporterExporter::TextureSettings::Formats::R8:
				case ImporterExporter::TextureSettings::Formats::R16:
				case ImporterExporter::TextureSettings::Formats::R32:
					info.Format = Formats::R8;
					break;

				case ImporterExporter::TextureSettings::Formats::RGB8:
				case ImporterExporter::TextureSettings::Formats::RGB16:
				case ImporterExporter::TextureSettings::Formats::RGB32:
					info.Format = Formats::RGB8;
					break;

				case ImporterExporter::TextureSettings::Formats::RGBA8:
				case ImporterExporter::TextureSettings::Formats::RGBA16:
				case ImporterExporter::TextureSettings::Formats::RGBA32:
					info.Format = Formats::RGBA8;
					break;
				}

				info.Borders = Vector4I(Settings.BorderLeft, Settings.BorderTop, Settings.BorderRight, Settings.BorderBottom);

				ImageParser::Parse(InBuffer, info);

				WriteHeader(OutBuffer, (Settings.UseType == ImporterExporter::TextureSettings::UseTypes::Texture ? ResourceTypes::Texture : ResourceTypes::Sprite), TextureParser::GetDumpSize(info));

				TextureParser::Dump(OutBuffer, info);

				PlatformMemory::Free(ConstCast(byte*, info.Data));
			}
		}
	}
}