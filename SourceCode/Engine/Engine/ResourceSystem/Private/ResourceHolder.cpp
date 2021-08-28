// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceHolder.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceSystem\Private\ResourceDatabase.h>
#include <ResourceAssetParser\ProgramParser.h>
#include <ResourceAssetParser\Private\ResourceAssetParserAllocators.h>
#include <Containers\Buffer.h>
#include <Containers\StringStream.h>
#include <Platform\PlatformDirectory.h>
#include <FileUtility\FileSystem.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\RenderWindow.h>
#include <Rendering\Sprite.h>
#include <FontSystem\Font.h>
#include <MemoryManagement\Allocator\FrameAllocator.h>

namespace Engine
{
	using namespace ResourceAssetParser;
	using namespace ResourceAssetParser::Private;
	using namespace FileUtility;
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
				case ResourceTypes::Program: \
				{ \
					IMPLEMENT(Program); \
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
				WString finalPath = Utilities::GetDataFileName(GUID);

				ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);

				if (!Utilities::ReadDataFile(inBuffer, Path::Combine(Holder->GetLibraryPath(), finalPath)))
					return;

				Holder->LoadInternal(inBuffer, Type, Resource);
			}

			ResourceHolder::ResourceHolder(const WString& ResourcesFullPath, const WString& LibraryFullPath) :
				m_Compiler(ResourcesFullPath, LibraryFullPath),
				m_LibraryPath(LibraryFullPath)
			{
				ResourceAssetParserAllocators::Create();
				ResourceSystemAllocators::Create();

				m_Compiler.Initialize();
				m_Compiler.OnResourceCompiledEvent += EventListener_OnResourceCompiled;

				m_IOThread.Initialize([this](void*) { IOThreadWorker(); });
				m_IOThread.SetName("ResourceHolder IO");
			}

			ResourceHolder::~ResourceHolder(void)
			{
				m_IOThread.Shutdown().Wait();

				m_Compiler.OnResourceCompiledEvent -= EventListener_OnResourceCompiled;

				for (auto& resourcePair : m_LoadedResources)
				{
					const ResourceInfo& info = resourcePair.GetSecond();

					Unload(info.Resource);
				}

				m_LoadedResources.Clear();
			}

			void ResourceHolder::Unload(ResourceBase* Resource)
			{
				ResourceTypes type = GetResourceType(Resource);
				if (type == ResourceTypes::Unknown)
					return;

#define IMPLEMENT(TypeName) \
				ResourceCommon::Resource<TypeName>* handle = ReinterpretCast(ResourceCommon::Resource<TypeName>*, Resource); \
				if (!handle->IsNull()) \
					ResourceFactory::Destroy##TypeName(handle->GetPointer()); \
				ResourceSystemAllocators::ResourceAllocator_Deallocate(handle); \
				//m_LoadedResources.Remove("")

				IMPLEMENT_TYPES_IMPLEMENT(type);

#undef IMPLEMENT
			}

			void ResourceHolder::AddLoadTask(const GUID& GUID, ResourceTypes Type, ResourceBase* ResourcePtr)
			{
				ResourceLoaderTask* task = ResourceSystemAllocators::ResourceAllocator_Allocate<ResourceLoaderTask>();
				Construct(task, this, GUID, Type, ResourcePtr);

				m_ResourceLoaderTasksLock.Lock();
				m_ResourceLoaderTasks.Enqueue(task);
				m_ResourceLoaderTasksLock.Release();
			}

			void ResourceHolder::LoadInternal(const ByteBuffer& Buffer, ResourceTypes Type, ResourceBase* ResourcePtr)
			{
#if DEBUG_MODE
#define IMPLEMENT(TypeName) \
				Resource<TypeName>* handle = ReinterpretCast(Resource<TypeName>*, ResourcePtr); \
				TypeName* oldResource = handle->GetPointer(); \
				auto result = ResourceFactory::Create<TypeName>(Buffer); \
				if (result.Resource != nullptr) \
					 result.Resource->SetName(Path::GetFileName(m_Compiler.GetDatabase()->GetRelativeFilePath(result.ID))); \
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

			GUID ResourceHolder::FindGUID(const WString& RelativeFilePath) const
			{
				return m_Compiler.GetDatabase()->GetGUID(RelativeFilePath);
			}

			ResourceBase* ResourceHolder::GetFromLoaded(const GUID& GUID) const
			{
				uint32 hash = Utilities::GetHash(GUID);

				if (m_LoadedResources.Contains(hash))
					return m_LoadedResources[hash].Resource;

				return nullptr;
			}

			void ResourceHolder::AddToLoaded(const GUID& GUID, ResourceTypes Type, ResourceBase* Resource)
			{
				uint32 hash = Utilities::GetHash(GUID);

				m_LoadedResources[hash] = { GUID, Type, Resource };
			}

			ResourceTypes ResourceHolder::GetResourceType(ResourceBase* Resource) const
			{
				for (auto& resourcePair : m_LoadedResources)
				{
					const ResourceInfo& info = resourcePair.GetSecond();

					if (info.Resource != Resource)
						continue;

					return info.Type;
				}

				return ResourceTypes::Unknown;
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
					{
						PlatformThread::YieldThread();
						continue;
					}

					(*task)();

					ResourceSystemAllocators::ResourceAllocator_Deallocate(task);
				}

				while (m_ResourceLoaderTasks.GetSize() != 0)
				{
					m_ResourceLoaderTasks.Dequeue(&task);

					ResourceSystemAllocators::ResourceAllocator_Deallocate(task);
				}
			}

			void ResourceHolder::OnResourceCompiled(const GUID& GUID, const WString& RelativeFilePath)
			{
				uint32 hash = Utilities::GetHash(GUID);

				if (m_LoadedResources.Contains(hash))
				{
					ResourceInfo& info = m_LoadedResources[hash];
					info.ID = GUID;

					AddLoadTask(GUID, info.Type, info.Resource);

					return;
				}

				//for (auto& resourcePair : m_LoadedResources)
				//{
				//	const ResourceInfo& info = resourcePair.GetSecond();

				//	if (info.ID != GUID)
				//		continue;

				//	m_LoadedResources.Remove(resourcePair.GetFirst());
				//	m_LoadedResources[hash] = { info.ID, info.Type, info.Resource };

				//	AddLoadTask(GUID, RelativeFilePath, info.Type, info.Resource);

				//	break;
				//}
			}
		}
	}
}