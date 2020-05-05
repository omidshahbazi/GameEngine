// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceFactory.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceSystem\Private\BuiltInAssets.h>
#include <ResourceAssetParser\ImageParser.h>
#include <ResourceAssetParser\TextureParser.h>
#include <ResourceAssetParser\MeshParser.h>
#include <ResourceAssetParser\OBJParser.h>
#include <Rendering\RenderingManager.h>
#include <FontSystem\FontManager.h>

namespace Engine
{
	using namespace Containers;
	using namespace Rendering;
	using namespace FontSystem;
	using namespace ResourceAssetParser;

	namespace ResourceSystem
	{
		namespace Private
		{
			//TODO: add structures for others like MeshInfo and TextureInfo
				//shader, text
			void WriteHeader(ByteBuffer& Buffer, ResourceTypes Type, uint64 DataSize)
			{
				Buffer << (int32)Type;
				Buffer << DataSize;
			}

			bool ResourceFactory::CompileTXT(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::TextSettings& Settings)
			{
				WriteHeader(OutBuffer, ResourceTypes::Text, InBuffer.GetSize());

				OutBuffer.AppendBuffer(InBuffer);

				return true;
			}

			Text* ResourceFactory::CreateText(const ByteBuffer& Buffer)
			{
				wstr data = ResourceSystemAllocators::Allocate<char16>(Buffer.GetSize() + 1);
				CharacterUtility::ChangeType(Buffer.GetBuffer(), data, Buffer.GetSize());
				data[Buffer.GetSize()] = CharacterUtility::Character<char16, '\0'>::Value;

				Text* text = ResourceSystemAllocators::Allocate<Text>(1);
				Construct(text, data);

				ResourceSystemAllocators::Deallocate(data);

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

				CreateTextureInternal(Buffer, &info);

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateTexture(&info);
			}

			Sprite* ResourceFactory::CreateSprite(const ByteBuffer& Buffer)
			{
				TextureInfo info;

				CreateTextureInternal(Buffer, &info);

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateSprite(&info);
			}

			void ResourceFactory::DestroyTexture(Texture* Texture)
			{
				RenderingManager::GetInstance()->GetActiveDevice()->DestroyTexture(Texture);
			}

			bool ResourceFactory::CompileSHADER(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::ShaderSettings& Settings)
			{
				WriteHeader(OutBuffer, ResourceTypes::Shader, InBuffer.GetSize());

				OutBuffer.AppendBuffer(InBuffer);

				return true;
			}

			Shader* ResourceFactory::CreateShader(const ByteBuffer& Buffer, String* Message)
			{
				auto data = ConstCast(str, ReinterpretCast(cstr, Buffer.GetBuffer()));
				data[Buffer.GetSize()] = CharacterUtility::Character<char8, '\0'>::Value;

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateShader(data, Message);
			}

			void ResourceFactory::DestroyShader(Shader* Shader)
			{
				RenderingManager::GetInstance()->GetActiveDevice()->DestroyShader(Shader);
			}

			bool ResourceFactory::CompileOBJ(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::MeshSettings& Settings)
			{
				MeshInfo meshInfo(&ResourceSystemAllocators::ResourceAllocator);
				OBJParser::Parse(InBuffer, meshInfo);

				uint64 size = MeshParser::GetDumpSize(meshInfo);

				WriteHeader(OutBuffer, ResourceTypes::Mesh, size);

				MeshParser::Dump(OutBuffer, meshInfo);

				return true;
			}

			Mesh* ResourceFactory::CreateMesh(const ByteBuffer& Buffer)
			{
				MeshInfo meshInfo;

				MeshParser::Parse(Buffer, meshInfo);

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateMesh(&meshInfo, GPUBuffer::Usages::StaticDraw);
			}

			void ResourceFactory::DestroyMesh(Mesh* Mesh)
			{
				RenderingManager::GetInstance()->GetActiveDevice()->DestroyMesh(Mesh);
			}

			bool ResourceFactory::CompileTTF(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::FontSettings& Settings)
			{
				WriteHeader(OutBuffer, ResourceTypes::Font, InBuffer.GetSize());

				OutBuffer.AppendBuffer(InBuffer);

				return true;
			}

			Font* ResourceFactory::CreateFont(const ByteBuffer& Buffer)
			{
				FontManager* fontMgr = FontManager::GetInstance();

				return fontMgr->LoadFont(Buffer);
			}

			void ResourceFactory::DestroyFont(Font* Font)
			{
				FontManager* fontMgr = FontManager::GetInstance();

				fontMgr->DestroyFont(Font);
			}

			Mesh* ResourceFactory::CreatePrimitiveMesh(PrimitiveMeshTypes Type)
			{
				SubMeshInfo* subMeshInfo = ReinterpretCast(SubMeshInfo*, AllocateMemory(&ResourceSystemAllocators::ResourceAllocator, sizeof(SubMeshInfo)));
				Construct(subMeshInfo, &ResourceSystemAllocators::ResourceAllocator);
				auto& subInfo = *subMeshInfo;

				if (Type == PrimitiveMeshTypes::Quad)
				{
					subInfo.Vertices.Add({ Vector3F(-0.5F, 0.5F, 0), Vector2F(0, 1) });
					subInfo.Vertices.Add({ Vector3F(-0.5F, -0.5F, 0), Vector2F(0, 0) });
					subInfo.Vertices.Add({ Vector3F(0.5F, 0.5F, 0), Vector2F(1, 1) });
					subInfo.Vertices.Add({ Vector3F(0.5F, -0.5F, 0), Vector2F(1, 0) });
					subInfo.Indices.Add(0);
					subInfo.Indices.Add(1);
					subInfo.Indices.Add(2);
					subInfo.Indices.Add(2);
					subInfo.Indices.Add(1);
					subInfo.Indices.Add(3);
					subInfo.Type = SubMesh::PolygonTypes::Triangles;
					subInfo.Layout = SubMesh::VertexLayouts::Position | SubMesh::VertexLayouts::UV;
				}
				else if (Type == PrimitiveMeshTypes::Cube)
				{
					OBJParser::Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::CUBE_MESH.GetValue())), BuiltInAssets::CUBE_MESH.GetLength()), subInfo);
				}
				else if (Type == PrimitiveMeshTypes::Sphere)
				{
					OBJParser::Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::SPHERE_MESH.GetValue())), BuiltInAssets::SPHERE_MESH.GetLength()), subInfo);
				}
				else if (Type == PrimitiveMeshTypes::Cone)
				{
					OBJParser::Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::CONE_MESH.GetValue())), BuiltInAssets::CONE_MESH.GetLength()), subInfo);
				}

				MeshInfo info(&ResourceSystemAllocators::ResourceAllocator);
				info.SubMeshes.Add(subMeshInfo);

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateMesh(&info, GPUBuffer::Usages::StaticDraw);
			}

			void ResourceFactory::CompileImageFile(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::TextureSettings& Settings)
			{
				TextureInfo info;
				ImageParser::Parse(InBuffer, info, Settings.ImportAlpha);

				uint64 bufferSize = TextureParser::GetDumpSize(info);
				WriteHeader(OutBuffer, (Settings.UseType == ImExporter::TextureSettings::UseTypes::Texture ? ResourceTypes::Texture : ResourceTypes::Sprite), bufferSize);

				TextureParser::Dump(OutBuffer, info);

				PlatformMemory::Free(ConstCast(byte*, info.Data));


				//int32 width;
				//int32 height;
				//int32 channelCount;

				//stbi_set_flip_vertically_on_load(true);

				//int32 desiredChannelCount = STBI_rgb;
				//if (Settings.ImportAlpha)
				//	desiredChannelCount = STBI_rgb_alpha;

				//const byte* const data = stbi_load_from_memory(InBuffer.GetBuffer(), InBuffer.GetSize(), &width, &height, &channelCount, desiredChannelCount);

				//desiredChannelCount = Mathematics::Min(desiredChannelCount, channelCount);

				//uint64 dataSize = width * height * desiredChannelCount;

				////					Width			Height			ChannelCount	BorderRight		BorderLeft		BorderTop		BorderBottom	DataSize
				//uint64 bufferSize = sizeof(int32) + sizeof(int32) + sizeof(int32) + sizeof(int32) + sizeof(int32) + sizeof(int32) + sizeof(int32) + dataSize;

				//WriteHeader(OutBuffer, (Settings.UseType == ImExporter::TextureSettings::UseTypes::Texture ? ResourceTypes::Texture : ResourceTypes::Sprite), bufferSize);

				//? ?
				//	//TODO: move this to ResourceParser
				//	OutBuffer << width;
				//OutBuffer << height;
				//OutBuffer << desiredChannelCount;
				//OutBuffer << Settings.BorderRight;
				//OutBuffer << Settings.BorderLeft;
				//OutBuffer << Settings.BorderTop;
				//OutBuffer << Settings.BorderBottom;

				//OutBuffer.AppendBuffer(data, 0, dataSize);

				//stbi_image_free(ConstCast(byte*, data));
			}

			//TODO: Cleanup these funcions
			void ResourceFactory::CreateTextureInternal(const ByteBuffer& Buffer, TextureInfo* Info)
			{
				TextureParser::Parse(Buffer, *Info);




				//uint64 index = 0;
				//Info->Dimension.X = Buffer.ReadValue<int32>(index);
				//index += sizeof(int32);
				//Info->Dimension.Y = Buffer.ReadValue<int32>(index);
				//index += sizeof(int32);
				//Info->ChannelCount = Buffer.ReadValue<int32>(index);
				//index += sizeof(int32);

				//Info->Borders.X = Buffer.ReadValue<int32>(index);
				//index += sizeof(int32);
				//Info->Borders.Z = Buffer.ReadValue<int32>(index);
				//index += sizeof(int32);
				//Info->Borders.Y = Buffer.ReadValue<int32>(index);
				//index += sizeof(int32);
				//Info->Borders.W = Buffer.ReadValue<int32>(index);
				//index += sizeof(int32);

				//Info->Data = Buffer.ReadValue(index, (Info->Dimension.X * Info->Dimension.Y * Info->ChannelCount));

				//Info->Format = (Info->ChannelCount == 3 ? Texture::Formats::RGB8 : Texture::Formats::RGBA8);
			}
		}
	}
}