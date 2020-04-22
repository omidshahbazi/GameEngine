// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceFactory.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceSystem\Private\BuiltInAssets.h>
#include <Rendering\RenderingManager.h>
#include <FontSystem\FontManager.h>
#include <Utility\AssetParser\OBJParser.h>
#include <Utility\AssetParser\InternalModelParser.h>

#define STB_IMAGE_IMPLEMENTATION
#include <STB\stb_image.h>

namespace Engine
{
	using namespace Containers;
	using namespace Rendering;
	using namespace FontSystem;

	namespace ResourceSystem
	{
		namespace Private
		{
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
				Vector2I dimension;
				int32 channelCount;
				Vector4I borders;
				Texture::Formats format;
				const byte* data;

				CreateTextureInternal(Buffer, dimension, channelCount, borders, format, &data);

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateTexture2D(dimension, format, data);
			}

			Sprite* ResourceFactory::CreateSprite(const ByteBuffer& Buffer)
			{
				Vector2I dimension;
				int32 channelCount;
				Vector4I borders;
				Texture::Formats format;
				const byte* data;

				CreateTextureInternal(Buffer, dimension, channelCount, borders, format, &data);

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateSprite(dimension, borders, format, data);
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
				AssetParser::OBJParser objParser;
				objParser.Parse(InBuffer, meshInfo);

				AssetParser::InternalModelParser internalParser;

				uint64 size = internalParser.GetDumpSize(meshInfo);

				WriteHeader(OutBuffer, ResourceTypes::Shader, size);

				internalParser.Dump(OutBuffer, meshInfo);

				return true;
			}

			Mesh* ResourceFactory::CreateMesh(const ByteBuffer& Buffer)
			{
				MeshInfo meshInfo;

				AssetParser::InternalModelParser parser;
				parser.Parse(Buffer, meshInfo);

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateMesh(&meshInfo, GPUBuffer::Usages::StaticDraw);
			}

			void ResourceFactory::DestroyMesh(Mesh* Mesh)
			{
				RenderingManager::GetInstance()->GetActiveDevice()->DestroyMesh(Mesh);
			}

			bool ResourceFactory::CompileTTF(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::FontSettings& Settings)
			{
				WriteHeader(OutBuffer, ResourceTypes::Shader, InBuffer.GetSize());

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
					AssetParser::OBJParser parser;
					parser.Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::CUBE_MESH.GetValue())), BuiltInAssets::CUBE_MESH.GetLength()), subInfo);
				}
				else if (Type == PrimitiveMeshTypes::Sphere)
				{
					AssetParser::OBJParser parser;
					parser.Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::SPHERE_MESH.GetValue())), BuiltInAssets::SPHERE_MESH.GetLength()), subInfo);
				}
				else if (Type == PrimitiveMeshTypes::Cone)
				{
					AssetParser::OBJParser parser;
					parser.Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::CONE_MESH.GetValue())), BuiltInAssets::CONE_MESH.GetLength()), subInfo);
				}

				MeshInfo info(&ResourceSystemAllocators::ResourceAllocator);
				info.SubMeshes.Add(subMeshInfo);

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateMesh(&info, GPUBuffer::Usages::StaticDraw);
			}

			void ResourceFactory::CompileImageFile(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, const ImExporter::TextureSettings& Settings)
			{
				int32 width;
				int32 height;
				int32 channelCount;

				stbi_set_flip_vertically_on_load(true);

				int32 desiredChannelCount = STBI_rgb;
				if (Settings.ImportAlpha)
					desiredChannelCount = STBI_rgb_alpha;

				const byte* const data = stbi_load_from_memory(InBuffer.GetBuffer(), InBuffer.GetSize(), &width, &height, &channelCount, desiredChannelCount);

				desiredChannelCount = Mathematics::Min(desiredChannelCount, channelCount);

				uint64 dataSize = width * height * desiredChannelCount;

				//					Width			Height			ChannelCount	BorderRight		BorderLeft		BorderTop		BorderBottom	DataSize
				uint64 bufferSize = sizeof(int32) + sizeof(int32) + sizeof(int32) + sizeof(int32) + sizeof(int32) + sizeof(int32) + sizeof(int32) + dataSize;

				WriteHeader(OutBuffer, (Settings.UseType == ImExporter::TextureSettings::UseTypes::Texture ? ResourceTypes::Texture : ResourceTypes::Sprite), bufferSize);

				OutBuffer << width;
				OutBuffer << height;
				OutBuffer << channelCount;
				OutBuffer << Settings.BorderRight;
				OutBuffer << Settings.BorderLeft;
				OutBuffer << Settings.BorderTop;
				OutBuffer << Settings.BorderBottom;

				OutBuffer.AppendBuffer(data, 0, dataSize);

				stbi_image_free(ConstCast(byte*, data));
			}

			void ResourceFactory::CreateTextureInternal(const ByteBuffer& Buffer, Vector2I& Dimension, int32& ChannelCount, Vector4I& Borders, Texture::Formats& Format, const byte** Data)
			{
				uint64 index = 0;
				Dimension.X = Buffer.ReadValue<int32>(index);
				index += sizeof(int32);
				Dimension.Y = Buffer.ReadValue<int32>(index);
				index += sizeof(int32);
				ChannelCount = Buffer.ReadValue<int32>(index);
				index += sizeof(int32);

				Borders.X = Buffer.ReadValue<int32>(index);
				index += sizeof(int32);
				Borders.Z = Buffer.ReadValue<int32>(index);
				index += sizeof(int32);
				Borders.Y = Buffer.ReadValue<int32>(index);
				index += sizeof(int32);
				Borders.W = Buffer.ReadValue<int32>(index);
				index += sizeof(int32);

				*Data = Buffer.ReadValue(index, (Dimension.X * Dimension.Y * ChannelCount));

				Format = (ChannelCount == 3 ? Texture::Formats::RGB8 : Texture::Formats::RGBA8);
			}
		}
	}
}