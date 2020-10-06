// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceHolder.h>
#include <ResourceSystem\Resource.h>
#include <ResourceAssetParser\ShaderParser.h>
#include <Containers\Buffer.h>
#include <Containers\StringStream.h>
#include <Platform\PlatformDirectory.h>
#include <Utility\FileSystem.h>
#include <Rendering\Sprite.h>
#include <MemoryManagement\Allocator\FrameAllocator.h>

namespace Engine
{
	using namespace ResourceAssetParser;
	using namespace Utility;
	using namespace Containers;
	using namespace Platform;
	using namespace MemoryManagement;
	using namespace Rendering;

	namespace ResourceSystem
	{
		namespace Private
		{
			ResourceHolder::ResourceHolder(const WString& ResourcesFullPath, const WString& LibraryFullPath) :
				m_Compiler(ResourcesFullPath, LibraryFullPath),
				m_LibraryPath(LibraryFullPath)
			{
				m_Compiler.AddListener(this);
				Compiler::GetInstance()->AddListener(this);
			}

			ResourceHolder::~ResourceHolder(void)
			{
				m_Compiler.AddListener(this);
				Compiler::GetInstance()->RemoveListener(this);

				for each (auto & resourcePair in m_LoadedResources)
				{
					const ResourceInfo& info = resourcePair.GetSecond();

					UnloadInternal(info.Type, info.Resource);

					DeallocateResource(info.Resource);
				}

				m_LoadedResources.Clear();
			}

			//void ResourceHolder::Reload(const WString& FilePath)
			//{
			//	const WString path = Path::Combine(GetResourcesPath(), FilePath);

			//	ResourceHandleBase* ptr = GetFromLoaded(FilePath);

			//	if (ptr == nullptr)
			//		return;

			//	ResourceTypes type;
			//	if (!Compile(path, type))
			//		return;

			//	ResourceHandleBase oldRes = *ptr;

			//	switch (type)
			//	{
			//	case ResourceTypes::Text:
			//	{
			//		ResourceHandle<Text>* handle = ReinterpretCast(ResourceHandle<Text>*, ptr);

			//		handle->Swap(LoadInternal<Text>(path));
			//	} break;

			//	case ResourceTypes::Texture:
			//	{
			//		ResourceHandle<Texture>* handle = ReinterpretCast(ResourceHandle<Texture>*, ptr);

			//		handle->Swap(LoadInternal<Texture>(path));
			//	} break;

			//	case ResourceTypes::Sprite:
			//	{
			//		ResourceHandle<Sprite>* handle = ReinterpretCast(ResourceHandle<Sprite>*, ptr);

			//		handle->Swap(LoadInternal<Sprite>(path));
			//	} break;

			//	case ResourceTypes::Shader:
			//	{
			//		ResourceHandle<Shader>* handle = ReinterpretCast(ResourceHandle<Shader>*, ptr);

			//		handle->Swap(LoadInternal<Shader>(path));
			//	} break;

			//	case ResourceTypes::Mesh:
			//	{
			//		ResourceHandle<Mesh>* handle = ReinterpretCast(ResourceHandle<Mesh>*, ptr);

			//		handle->Swap(LoadInternal<Mesh>(path));
			//	} break;

			//	case ResourceTypes::Font:
			//	{
			//		ResourceHandle<Font>* handle = ReinterpretCast(ResourceHandle<Font>*, ptr);

			//		handle->Swap(LoadInternal<Font>(path));
			//	} break;
			//	}

			//	UnloadInternal(type, &oldRes);
			//}

			void ResourceHolder::UnloadInternal(ResourceTypes Type, ResourceBase* Resource)
			{
				switch (Type)
				{
				case ResourceTypes::Text:
				{
					TextResource* handle = ReinterpretCast(TextResource*, Resource);

					if (handle->IsNull())
						return;

					ResourceFactory::DestroyText(**handle);
				} break;

				case ResourceTypes::Texture:
				{
					TextureResource* handle = ReinterpretCast(TextureResource*, Resource);

					if (handle->IsNull())
						return;

					ResourceFactory::DestroyTexture(**handle);
				} break;

				case ResourceTypes::Sprite:
				{
					SpriteResource* handle = ReinterpretCast(SpriteResource*, Resource);

					if (handle->IsNull())
						return;

					ResourceFactory::DestroyTexture(**handle);
				} break;

				case ResourceTypes::Shader:
				{
					ShaderResource* handle = ReinterpretCast(ShaderResource*, Resource);

					if (handle->IsNull())
						return;

					ResourceFactory::DestroyShader(**handle);
				} break;

				case ResourceTypes::Mesh:
				{
					MeshResource* handle = ReinterpretCast(MeshResource*, Resource);

					if (handle->IsNull())
						return;

					ResourceFactory::DestroyMesh(**handle);
				} break;

				case ResourceTypes::Font:
				{
					FontResource* handle = ReinterpretCast(FontResource*, Resource);

					if (handle->IsNull())
						return;

					ResourceFactory::DestroyFont(**handle);
				} break;
				}
			}

			ResourceBase* ResourceHolder::GetFromLoaded(const WString& Name)
			{
				uint32 hash = Utilities::GetHash(Name);

				if (m_LoadedResources.Contains(hash))
					return m_LoadedResources[hash].Resource;

				return nullptr;
			}

			void ResourceHolder::AddToLoaded(const WString& Name, ResourceTypes Type, ResourceBase* Resource)
			{
				uint32 hash = Utilities::GetHash(Name);

				m_LoadedResources[hash] = { Type, Resource };
			}

			bool ResourceHolder::FetchShaderSource(const String& Name, String& Source)
			{
				WString finalPath = Utilities::GetDataFileName(Name.ChangeType<char16>());

				ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);

				if (!Utilities::ReadDataFile(inBuffer, Path::Combine(GetLibraryPath(), finalPath)))
					return false;

				ResourceTypes resType = ResourceTypes::Unknown;
				uint64 dataSize = 0;
				byte* data = nullptr;
				if (!ResourceFactory::ReadHeader(inBuffer, &resType, &dataSize, &data))
					return false;

				ByteBuffer buffer(data, dataSize);

				ShaderInfo info;
				ShaderParser::Parse(buffer, info);

				Source = info.Source;

				return true;
			}

			void ResourceHolder::OnResourceCompiled(const WString& FilePath)
			{

			}
		}
	}
}