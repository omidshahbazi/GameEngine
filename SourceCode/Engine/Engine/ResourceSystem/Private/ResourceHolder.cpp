// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceHolder.h>
#include <ResourceAssetParser\ShaderParser.h>
#include <ResourceAssetParser\Private\ResourceAssetParserAllocators.h>
#include <Containers\Buffer.h>
#include <Containers\StringStream.h>
#include <Platform\PlatformDirectory.h>
#include <Utility\FileSystem.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\RenderWindow.h>
#include <Rendering\Sprite.h>
#include <MemoryManagement\Allocator\FrameAllocator.h>

namespace Engine
{
	using namespace ResourceAssetParser;
	using namespace ResourceAssetParser::Private;
	using namespace Utility;
	using namespace Containers;
	using namespace Platform;
	using namespace MemoryManagement;
	using namespace Rendering;

	namespace ResourceSystem
	{
		namespace Private
		{
			void ResourceHolder::ResourceLoaderTask::operator()(void)
			{
				WString finalPath = Utilities::GetDataFileName(FilePath);

				ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);

				if (!Utilities::ReadDataFile(inBuffer, Path::Combine(Holder->GetLibraryPath(), finalPath)))
					return;

#define IMPLEMENT(Type) \
				ResourceSystem::Resource<Type>* handle = ReinterpretCast(ResourceSystem::Resource<Type>*, Resource); \
				Type* oldResource = handle->GetPointer(); \
				handle->Swap(ResourceFactory::Create<Type>(inBuffer));

				DeviceInterface* device = RenderingManager::GetInstance()->GetActiveDevice();
				RenderContext* currentContext = device->GetContext();

				device->SetContext(Holder->m_Context);

				switch (Type)
				{
				case ResourceTypes::Text:
				{
					IMPLEMENT(Text);
				} break;

				case ResourceTypes::Texture:
				{
					IMPLEMENT(Texture);
				} break;

				case ResourceTypes::Sprite:
				{
					IMPLEMENT(Sprite);
				} break;

				case ResourceTypes::Shader:
				{
					IMPLEMENT(Shader);
				} break;

				case ResourceTypes::Mesh:
				{
					IMPLEMENT(Mesh);
				} break;

				case ResourceTypes::Font:
				{
					IMPLEMENT(Font);
				} break;
				}

				device->SetContext(currentContext);

#undef IMPLEMENT
			}

			ResourceHolder::ResourceHolder(const WString& ResourcesFullPath, const WString& LibraryFullPath) :
				m_ContextWindow(nullptr),
				m_Context(nullptr),
				m_Compiler(ResourcesFullPath, LibraryFullPath),
				m_LibraryPath(LibraryFullPath)
			{
				ResourceAssetParserAllocators::Create();
				ResourceSystemAllocators::Create();

				m_ContextWindow = ResourceSystemAllocators::ResourceAllocator_Allocate<RenderWindow>();
				Construct(m_ContextWindow, "ResourceHolder ContextWindow");
				m_ContextWindow->SetIsVisible(false);

				DeviceInterface* device = RenderingManager::GetInstance()->GetActiveDevice();
				m_Context = device->CreateContext(m_ContextWindow);

				m_Compiler.AddListener(this);
				Compiler::GetInstance()->AddListener(this);

				m_IOThread.Initialize([this](void*) { IOThreadWorker(); });
				m_IOThread.SetName("ResourceHolder IO");
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

			void ResourceHolder::UnloadInternal(ResourceTypes Type, ResourceBase* Resource)
			{
#define IMPLEMENT(Type) \
				ResourceSystem::Resource<Type>* handle = ReinterpretCast(ResourceSystem::Resource<Type>*, Resource); \
				if (handle->IsNull()) \
					return; \
				ResourceFactory::Destroy##Type(handle->GetPointer()); \

				switch (Type)
				{
				case ResourceTypes::Text:
				{
					IMPLEMENT(Text);
				} break;

				case ResourceTypes::Texture:
				{
					IMPLEMENT(Texture);
				} break;

				case ResourceTypes::Sprite:
				{
					IMPLEMENT(Sprite);
				} break;

				case ResourceTypes::Shader:
				{
					IMPLEMENT(Shader);
				} break;

				case ResourceTypes::Mesh:
				{
					IMPLEMENT(Mesh);
				} break;

				case ResourceTypes::Font:
				{
					IMPLEMENT(Font);
				} break;
				}

#undef IMPLEMENT
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

			void ResourceHolder::IOThreadWorker(void)
			{
				while (true)
				{
					PlatformThread::Sleep(1);

					if (!m_ResourceLoaderTasksLock.TryLock())
						continue;

					ResourceLoaderTask* task = nullptr;
					if (m_ResourceLoaderTasks.GetSize() != 0)
						m_ResourceLoaderTasks.Dequeue(&task);

					m_ResourceLoaderTasksLock.Release();

					if (task == nullptr)
						continue;

					(*task)();

					ResourceSystemAllocators::ResourceAllocator_Deallocate(task);
				}
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

			void ResourceHolder::OnResourceCompiled(const WString& FullPath, uint32 Hash, const String& ResourceID)
			{
				if (m_LoadedResources.Contains(Hash))
				{
					ResourceInfo info = m_LoadedResources[Hash];


				}
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
		}
	}
}