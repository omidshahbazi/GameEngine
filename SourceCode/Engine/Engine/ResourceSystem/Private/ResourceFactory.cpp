// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceFactory.h>
#include <ResourceSystem\Resource.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <Rendering\RenderingManager.h>
#include <ResourceSystem\Private\Parser\OBJParser.h>

#define STB_IMAGE_IMPLEMENTATION
#include <STB\stb_image.h>

namespace Engine
{
	using namespace Containers;
	using namespace Rendering;

	namespace ResourceSystem
	{
		namespace Private
		{
			SINGLETON_DEFINITION(ResourceFactory)

				ResourceFactory::ResourceFactory(void)
			{
			}

			ResourceFactory::~ResourceFactory(void)
			{
			}

			ByteBuffer *ResourceFactory::Compile(const WString &Extension, ByteBuffer *Buffer)
			{
				FileTypes fileType = GetFileTypeByExtension(Extension);

				if (fileType == FileTypes::Unknown)
					return nullptr;

				ResourceTypes resType = GetResourceTypeByFileType(fileType);

				ByteBuffer *buffer = ResourceSystemAllocators::Allocate<ByteBuffer>(1);
				new (buffer) ByteBuffer(&ResourceSystemAllocators::ResourceAllocator);

				*buffer << (int32)resType;
				*buffer << Buffer->GetSize();

				switch (resType)
				{
				case ResourceTypes::Text:
				case ResourceTypes::Shader:
				{
					buffer->AppendBuffer(*Buffer);
				}
				break;

				case ResourceTypes::Texture:
				{
					CompileImageFile(buffer, Buffer);
				} 
				break;

				case ResourceTypes::Model:
				{
					if (fileType == FileTypes::OBJ)
						CompileOBJFile(buffer, Buffer);
				}
				break;
				}

				return buffer;
			}

			void ResourceFactory::CompileImageFile(ByteBuffer * OutBuffer, ByteBuffer * InBuffer)
			{
				int32 width;
				int32 height;
				int32 channelsCount;
				const byte* const data = stbi_load_from_memory(InBuffer->GetBuffer(), InBuffer->GetSize(), &width, &height, &channelsCount, 0);

				*OutBuffer << width;
				*OutBuffer << height;
				*OutBuffer << channelsCount;
				int32 size = width * height * channelsCount;

				OutBuffer->AppendBuffer(data, 0, size);

				stbi_image_free(ConstCast(byte*, data));
			}

			void ResourceFactory::CompileOBJFile(ByteBuffer * OutBuffer, ByteBuffer * InBuffer)
			{
				MeshInfo meshInfo;
				Parser::OBJParser parser;
				parser.Parse(InBuffer->GetBuffer(), meshInfo);

				OutBuffer->Append(meshInfo.SubMeshes.GetSize());
				for each (auto &subMesh in meshInfo.SubMeshes)
				{
					OutBuffer->Append((int32)subMesh.Layout);

					OutBuffer->Append(subMesh.Vertices.GetSize());
					for each (auto &vertex in subMesh.Vertices)
					{
						OutBuffer->Append(vertex.Position.X);
						OutBuffer->Append(vertex.Position.Y);
						OutBuffer->Append(vertex.Position.Z);

						OutBuffer->Append(vertex.Normal.X);
						OutBuffer->Append(vertex.Normal.Y);
						OutBuffer->Append(vertex.Normal.Z);

						OutBuffer->Append(vertex.UV.X);
						OutBuffer->Append(vertex.UV.Y);
					}

					OutBuffer->Append(subMesh.Indices.GetSize());
					for each (auto &index in subMesh.Indices)
						OutBuffer->Append(index);
				}
			}

			Text *ResourceFactory::CreateText(uint64 Size, const byte *const Data)
			{
				wstr data = ResourceSystemAllocators::Allocate<char16>(Size + 1);
				CharacterUtility::ChangeType(Data, data, Size);
				data[Size] = CharacterUtility::Character<char16, '\0'>::Value;

				Text *text = ResourceSystemAllocators::Allocate<Text>(1);
				Construct(text, data);

				ResourceSystemAllocators::Deallocate(data);

				return text;
			}

			Texture *ResourceFactory::CreateTexture(uint64 Size, const byte *const Data)
			{
				ByteBuffer buffer(&ResourceSystemAllocators::ResourceAllocator, Data, Size * 4);

				uint64 index = 0;
				int32 width = buffer.ReadValue<int32>(index);
				index += sizeof(int32);
				int32 height = buffer.ReadValue<int32>(index);
				index += sizeof(int32);
				int32 channelCount = buffer.ReadValue<int32>(index);
				index += sizeof(int32);

				const byte * const data = buffer.ReadValue(index, (width* height * channelCount));

				Texture *tex = RenderingManager::GetInstance()->GetActiveDevice()->CreateTexture2D(data, width, height, channelCount, IDevice::TextureFormats::RGBA32);

				return tex;
			}

			Program *ResourceFactory::CreateShader(uint64 Size, const byte *const Data)
			{
				auto data = ConstCast(str, ReinterpretCast(cstr, Data));
				data[Size] = CharacterUtility::Character<char8, '\0'>::Value;

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateProgram(data);
			}

			Mesh * ResourceFactory::CreateModel(uint64 Size, const byte * const Data)
			{
				ByteBuffer buffer(&ResourceSystemAllocators::ResourceAllocator, Data, Size);

				MeshInfo meshInfo;

				uint64 index = 0;
				uint32 subMeshCount = buffer.ReadValue<uint32>(index);
				index += sizeof(uint32);

				for (uint32 i = 0; i < subMeshCount; ++i)
				{
					SubMeshInfo subMeshInfo;

					subMeshInfo.Layout = (SubMeshInfo::VertexLayouts)buffer.ReadValue<int32>(index);
					index += sizeof(int32);

					uint32 vertexCount = buffer.ReadValue<uint32>(index);
					index += sizeof(uint32);
					for (uint32 j = 0; j < vertexCount; ++j)
					{
						Vector3F pos;
						pos.X = buffer.ReadValue<float32>(index);
						index += sizeof(float32);
						pos.Y = buffer.ReadValue<float32>(index);
						index += sizeof(float32);
						pos.Z = buffer.ReadValue<float32>(index);
						index += sizeof(float32);

						Vector3F norm;
						norm.X = buffer.ReadValue<float32>(index);
						index += sizeof(float32);
						norm.Y = buffer.ReadValue<float32>(index);
						index += sizeof(float32);
						norm.Z = buffer.ReadValue<float32>(index);
						index += sizeof(float32);

						Vector2F uv;
						uv.X = buffer.ReadValue<float32>(index);
						index += sizeof(float32);
						uv.Y = buffer.ReadValue<float32>(index);
						index += sizeof(float32);

						subMeshInfo.Vertices.Add({ pos, norm, uv });
					}

					uint32 uvCount = buffer.ReadValue<uint32>(index);
					index += sizeof(uint32);
					for (uint32 j = 0; j < uvCount; ++j)
					{
						uint32 idx = buffer.ReadValue<uint32>(index);
						index += sizeof(uint32);

						subMeshInfo.Indices.Add(idx);
					}

					meshInfo.SubMeshes.Add(subMeshInfo);
				}

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateMesh(&meshInfo, IDevice::BufferUsages::StaticDraw);
			}

			ResourceFactory::FileTypes ResourceFactory::GetFileTypeByExtension(const WString &Extension)
			{
				if (Extension == L".txt")
					return FileTypes::TXT;

				if (Extension == L".png")
					return FileTypes::PNG;

				if (Extension == L".shader")
					return FileTypes::SHADER;

				if (Extension == L".obj")
					return FileTypes::OBJ;

				return FileTypes::Unknown;
			}

			ResourceFactory::ResourceTypes ResourceFactory::GetResourceTypeByFileType(FileTypes FileType)
			{
				switch (FileType)
				{
				case FileTypes::TXT:
					return ResourceTypes::Text;

				case FileTypes::PNG:
					return ResourceTypes::Texture;

				case FileTypes::SHADER:
					return ResourceTypes::Shader;

				case FileTypes::OBJ:
					return ResourceTypes::Model;
				}
			}
		}
	}
}