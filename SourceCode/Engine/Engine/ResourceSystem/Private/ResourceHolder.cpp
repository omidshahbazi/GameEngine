// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceHolder.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceAssetParser\ShaderParser.h>
#include <ResourceAssetParser\Private\ResourceAssetParserAllocators.h>
#include <Containers\Buffer.h>
#include <Containers\StringStream.h>
#include <Platform\PlatformDirectory.h>
#include <Utility\FileSystem.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\RenderWindow.h>
#include <Rendering\Sprite.h>
#include <FontSystem\Font.h>
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
	using namespace FontSystem;

	namespace ResourceSystem
	{
		namespace Private
		{
#define IMPLEMENT_TYPES_IMPLEMENT(TypeName) \
			switch (TypeName) \
			{ \
				case ResourceTypes::Text: \
				{ \
					IMPLEMENT(Text); \
				} break; \
				case ResourceTypes::Texture: \
				{ \
					IMPLEMENT(Texture); \
				} break; \
				case ResourceTypes::Sprite: \
				{ \
					IMPLEMENT(Sprite); \
				} break; \
				case ResourceTypes::Shader: \
				{ \
					IMPLEMENT(Shader); \
				} break; \
				case ResourceTypes::Mesh: \
				{ \
					IMPLEMENT(Mesh); \
				} break; \
				case ResourceTypes::Font: \
				{ \
					IMPLEMENT(Font); \
				} break; \
			}

			void ResourceHolder::ResourceLoaderTask::operator()(void)
			{
				WString finalPath = Utilities::GetDataFileName(FilePath);

				ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);

				if (!Utilities::ReadDataFile(inBuffer, Path::Combine(Holder->GetLibraryPath(), finalPath)))
					return;

#if DEBUG_MODE
				Holder->LoadInternal(inBuffer, Type, Resource, Path::GetFileName(FilePath));
#else
				Holder->LoadInternal(inBuffer, Type, Resource);
#endif
			}

			ResourceHolder::ResourceHolder(const WString& ResourcesFullPath, const WString& LibraryFullPath) :
				m_Compiler(ResourcesFullPath, LibraryFullPath),
				m_LibraryPath(LibraryFullPath)
			{
				ResourceAssetParserAllocators::Create();
				ResourceSystemAllocators::Create();

				m_Compiler.AddListener(this);
				Compiler::GetInstance()->AddListener(this);

				m_IOThread.Initialize([this](void*) { IOThreadWorker(); });
				m_IOThread.SetName("ResourceHolder IO");
			}

			ResourceHolder::~ResourceHolder(void)
			{
				m_IOThread.Shutdown().Wait();

				m_Compiler.AddListener(this);
				Compiler::GetInstance()->RemoveListener(this);

				for each (auto & resourcePair in m_LoadedResources)
				{
					const ResourceInfo& info = resourcePair.GetSecond();

					Unload(info.Resource);

					DeallocateResource(info.Resource);
				}

				m_LoadedResources.Clear();
			}

			void ResourceHolder::Unload(ResourceBase* Resource)
			{
				ResourceTypes type = ResourceTypes::Unknown;

				for each (auto & resourcePair in m_LoadedResources)
				{
					const ResourceInfo& info = resourcePair.GetSecond();

					if (info.Resource != Resource)
						continue;

					type = info.Type;
					break;
				}

#define IMPLEMENT(TypeName) \
				ResourceSystem::Resource<TypeName>* handle = ReinterpretCast(ResourceSystem::Resource<TypeName>*, Resource); \
				if (handle->IsNull()) \
					return; \
				ResourceFactory::Destroy##TypeName(handle->GetPointer()); \

				IMPLEMENT_TYPES_IMPLEMENT(type);

#undef IMPLEMENT
			}

			void ResourceHolder::AddLoadTask(const WString& FilePath, ResourceTypes Type, ResourceBase* ResourcePtr)
			{
				ResourceLoaderTask* task = ResourceSystemAllocators::ResourceAllocator_Allocate<ResourceLoaderTask>();
				Construct(task, this, FilePath, Type, ResourcePtr);

				m_ResourceLoaderTasksLock.Lock();
				m_ResourceLoaderTasks.Enqueue(task);
				m_ResourceLoaderTasksLock.Release();
			}

			void ResourceHolder::LoadInternal(const ByteBuffer& Buffer, ResourceTypes Type, ResourceBase* ResourcePtr, const WString& Name)
			{
#if DEBUG_MODE
#define IMPLEMENT(TypeName) \
				Resource<TypeName>* handle = ReinterpretCast(Resource<TypeName>*, ResourcePtr); \
				TypeName* oldResource = handle->GetPointer(); \
				auto result = ResourceFactory::Create<TypeName>(Buffer); \
				result.Resource->SetName(Name); \
				handle->SetID(result.ID); \
				handle->Swap(result.Resource); \
				if (oldResource != nullptr) \
					ResourceFactory::Destroy##TypeName(oldResource);
#else
#define IMPLEMENT(TypeName) \
				Resource<TypeName>* handle = ReinterpretCast(Resource<TypeName>*, ResourcePtr); \
				TypeName* oldResource = handle->GetPointer(); \
				auto result = ResourceFactory::Create<TypeName>(Buffer); \
				handle->SetID(result.ID); \
				handle->Swap(result.Resource); \
				if (oldResource != nullptr) \
					ResourceFactory::Destroy##TypeName(oldResource);
#endif

				IMPLEMENT_TYPES_IMPLEMENT(Type);

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

				m_LoadedResources[hash] = { "", Type, Resource };
			}

			void ResourceHolder::DeallocateResource(ResourceBase* Resource) const
			{
				ResourceSystemAllocators::ResourceAllocator_Deallocate(Resource);
			}

			void ResourceHolder::IOThreadWorker(void)
			{
				ResourceLoaderTask* task = nullptr;

				while (!m_IOThread.GetShouldExit())
				{
					PlatformThread::Sleep(1);

					if (!m_ResourceLoaderTasksLock.TryLock())
						continue;

					if (m_ResourceLoaderTasks.GetSize() == 0)
						task = nullptr;
					else
						m_ResourceLoaderTasks.Dequeue(&task);

					m_ResourceLoaderTasksLock.Release();

					if (task == nullptr)
						continue;

					(*task)();

					ResourceSystemAllocators::ResourceAllocator_Deallocate(task);
				}

				while (m_ResourceLoaderTasks.GetSize() != 0)
				{
					m_ResourceLoaderTasks.Dequeue(&task);

					ResourceSystemAllocators::ResourceAllocator_Deallocate(task);
				}
			}

			bool ResourceHolder::FetchShaderSource(const String& Name, String& Source)
			{
				WString finalPath = Utilities::GetDataFileName(Name.ChangeType<char16>());

				ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);

				if (!Utilities::ReadDataFile(inBuffer, Path::Combine(GetLibraryPath(), finalPath)))
					return false;

				String id;
				ResourceTypes resType = ResourceTypes::Unknown;
				uint64 dataSize = 0;
				byte* data = nullptr;
				if (!ResourceFactory::ReadHeader(inBuffer, &id, &resType, &dataSize, &data))
					return false;

				ByteBuffer buffer(data, dataSize);

				ShaderInfo info;
				ShaderParser::Parse(buffer, info);

				Source = info.Source;

				return true;
			}

			void ResourceHolder::OnResourceCompiled(const WString& FullPath, uint32 Hash, const String& ResourceID)
			{
				WString relativePath = Path::GetRelativePath(m_Compiler.GetResourcesPath(), FullPath);

				if (m_LoadedResources.Contains(Hash))
				{
					ResourceInfo& info = m_LoadedResources[Hash];
					info.ID = ResourceID;

					AddLoadTask(relativePath, info.Type, info.Resource);

					return;
				}

				for each (auto & resourcePair in m_LoadedResources)
				{
					const ResourceInfo& info = resourcePair.GetSecond();

					if (info.ID != ResourceID)
						continue;

					m_LoadedResources.Remove(resourcePair.GetFirst());
					m_LoadedResources[Hash] = { info.ID, info.Type, info.Resource };

					AddLoadTask(relativePath, info.Type, info.Resource);

					break;
				}
			}
		}
	}
}