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
				Compiler::GetInstance()->AddListener(this);
			}

			ResourceHolder::~ResourceHolder(void)
			{
				for each (auto & resourcePair in m_LoadedResources)
				{
					const ResourceInfo& info = resourcePair.GetSecond();

					UnloadInternal(info.Type, info.Resource);

					DeallocateResourceHandle(info.Resource);
				}

				m_LoadedResources.Clear();

				Compiler::GetInstance()->RemoveListener(this);
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

			void ResourceHolder::UnloadInternal(ResourceTypes Type, ResourceHandleBase* Holder)
			{
				switch (Type)
				{
				case ResourceTypes::Text:
				{
					ResourceHandle<Text>* handle = ReinterpretCast(ResourceHandle<Text>*, Holder);

					if (handle->IsNull())
						return;

					ResourceFactory::DestroyText(**handle);
				} break;

				case ResourceTypes::Texture:
				{
					ResourceHandle<Texture>* handle = ReinterpretCast(ResourceHandle<Texture>*, Holder);

					if (handle->IsNull())
						return;

					ResourceFactory::DestroyTexture(**handle);
				} break;

				case ResourceTypes::Sprite:
				{
					ResourceHandle<Sprite>* handle = ReinterpretCast(ResourceHandle<Sprite>*, Holder);

					if (handle->IsNull())
						return;

					ResourceFactory::DestroyTexture(**handle);
				} break;

				case ResourceTypes::Shader:
				{
					ResourceHandle<Shader>* handle = ReinterpretCast(ResourceHandle<Shader>*, Holder);

					if (handle->IsNull())
						return;

					ResourceFactory::DestroyShader(**handle);
				} break;

				case ResourceTypes::Mesh:
				{
					ResourceHandle<Mesh>* handle = ReinterpretCast(ResourceHandle<Mesh>*, Holder);

					if (handle->IsNull())
						return;

					ResourceFactory::DestroyMesh(**handle);
				} break;

				case ResourceTypes::Font:
				{
					ResourceHandle<Font>* handle = ReinterpretCast(ResourceHandle<Font>*, Holder);

					if (handle->IsNull())
						return;

					ResourceFactory::DestroyFont(**handle);
				} break;
				}
			}

			ResourceHandleBase* ResourceHolder::GetFromLoaded(const WString& Name)
			{
				uint32 hash = Utilities::GetHash(Name);

				if (m_LoadedResources.Contains(hash))
					return m_LoadedResources[hash].Resource;

				return nullptr;
			}

			void ResourceHolder::AddToLoaded(const WString& Name, ResourceTypes Type, ResourceHandleBase* Holder)
			{
				uint32 hash = Utilities::GetHash(Name);

				m_LoadedResources[hash] = { Type, Holder };
			}

			bool ResourceHolder::FetchShaderSource(const String& Name, String& Source)
			{
				WString finalPath = Utilities::GetDataFileName(Name.ChangeType<char16>());

				ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);

				//if (!Utilities::ReadDataFile(inBuffer, Path::Combine(GetLibraryPath(), finalPath)))
					return false;

				ShaderInfo info;
				ShaderParser::Parse(inBuffer, info);

				Source = info.Source;

				return true;
			}
		}
	}
}