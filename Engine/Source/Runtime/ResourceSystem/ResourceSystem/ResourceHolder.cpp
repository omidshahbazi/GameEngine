// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceHolder.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceSystem\Private\ResourceDatabase.h>
#include <ResourceSystem\Private\ResourceFactory.h>
#include <ProgramAssetParser\ProgramParser.h>
#include <TTFAssetParser\Private\TTFParserAllocators.h>
#include <FontAssetParser\Private\FontParserAllocators.h>
#include <Containers\Buffer.h>
#include <Containers\StringStream.h>
#include <Platform\PlatformDirectory.h>
#include <FileUtility\FileSystem.h>
#include <RenderSystem\RenderManager.h>
#include <RenderSystem\RenderWindow.h>
#include <RenderSystem\Program.h>
#include <RenderSystem\Mesh.h>
#include <RenderSystem\Sprite.h>
#include <FontSystem\Font.h>
#include <Allocators\FrameAllocator.h>
#include <ProgramCompiler\ProgramToAPICompiler.h>

namespace Engine
{
	using namespace ResourceAssetParser;
	using namespace ResourceAssetParser::Private;
	using namespace FileUtility;
	using namespace Containers;
	using namespace Platform;
	using namespace MemoryManagement;
	using namespace FontSystem;
	using namespace ProgramCompiler;
	using namespace RenderSystem;

	namespace ResourceSystem
	{
		using namespace Private;

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

			if (!Utilities::ReadDataFile(Path::Combine(Holder->GetLibraryPath(), finalPath), inBuffer))
				return;

			Holder->LoadInternal(GUID, inBuffer, Type, Resource);
		}

		ResourceHolder::ResourceHolder(const WString& ResourcesFullPath, const WString& LibraryFullPath) :
			m_Compiler(ResourcesFullPath, LibraryFullPath),
			m_LibraryPath(LibraryFullPath)
		{
			TTFParserAllocators::Create();
			FontParserAllocators::Create();
			ResourceSystemAllocators::Create();

			m_ResourceDatabase = ResourceSystemAllocators::ResourceAllocator_Allocate<ResourceDatabase>();
			Construct(m_ResourceDatabase, m_LibraryPath);

			m_Compiler.Initialize(m_ResourceDatabase);
			m_Compiler.OnResourceCompiledEvent += EventListener_OnResourceCompiled;

			ProgramToAPICompiler::GetInstance()->OnFetchShaderSourceEvent += EventListener_FetchShaderSource;

			m_IOThread.Initialize([this](void*) { IOThreadWorker(); });
			m_IOThread.SetName("ResourceHolder IO");
		}

		ResourceHolder::~ResourceHolder(void)
		{
			ProgramToAPICompiler::GetInstance()->OnFetchShaderSourceEvent -= EventListener_FetchShaderSource;

			m_IOThread.Shutdown().Wait();

			m_Compiler.OnResourceCompiledEvent -= EventListener_OnResourceCompiled;

			for (auto& resourcePair : m_LoadedResources)
			{
				const ResourceInfo& info = resourcePair.GetSecond();

				Unload(info.Resource);
			}

			m_LoadedResources.Clear();

			for (auto& resourcePair : m_WaitingToCompile)
			{
				const ResourceInfo& info = resourcePair.GetSecond();

				ResourceSystemAllocators::ResourceAllocator_Deallocate(info.Resource);
			}

			m_WaitingToCompile.Clear();

			ResourceSystemAllocators::ResourceAllocator_Deallocate(m_ResourceDatabase);
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

		void ResourceHolder::LoadInternal(const GUID& GUID, const ByteBuffer& Buffer, ResourceTypes Type, ResourceBase* ResourcePtr)
		{
#ifdef DEBUG_MODE
#define IMPLEMENT(TypeName) \
				Resource<TypeName>* handle = ReinterpretCast(Resource<TypeName>*, ResourcePtr); \
				TypeName* oldResource = handle->GetPointer(); \
				auto result = ResourceFactory::Create<TypeName>(Buffer); \
				if (result != nullptr) \
				{ \
					ResourceDatabase::ResourceInfo info = {}; \
					if (m_ResourceDatabase->GetResourceInfo(GUID, info)) \
						result->SetName(Path::GetFileName(info.RelativePath)); \
				} \
				handle->SetID(GUID); \
				handle->Set(result); \
				if (oldResource != nullptr) \
					ResourceFactory::Destroy##TypeName(oldResource);
#else
#define IMPLEMENT(TypeName) \
				Resource<TypeName>* handle = ReinterpretCast(Resource<TypeName>*, ResourcePtr); \
				TypeName* oldResource = handle->GetPointer(); \
				auto result = ResourceFactory::Create<TypeName>(Buffer); \
				handle->SetID(GUID); \
				handle->Set(result); \
				if (oldResource != nullptr) \
					ResourceFactory::Destroy##TypeName(oldResource);
#endif

			IMPLEMENT_TYPES_IMPLEMENT(Type);

#undef IMPLEMENT
		}

		bool ResourceHolder::DoesResourceExists(const GUID& GUID) const
		{
			return m_ResourceDatabase->DoesResourceExists(GUID);
		}

		GUID ResourceHolder::FindGUID(const WString& RelativeFilePath) const
		{
			ResourceDatabase::ResourceInfo info = {};
			if (!m_ResourceDatabase->GetResourceInfo(RelativeFilePath, info))
				return GUID::Invalid;

			return info.GUID;
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
			}

			if (m_WaitingToCompile.Contains(RelativeFilePath))
			{
				ResourceInfo& info = m_WaitingToCompile[RelativeFilePath];

				info.ID = GUID;

				AddToLoaded(GUID, info.Type, info.Resource);

				AddLoadTask(GUID, info.Type, info.Resource);

				m_WaitingToCompile.Remove(RelativeFilePath);
			}
		}

		void ResourceHolder::FetchShaderSource(const String& RelativeFilePath, String& Source)
		{
			ResourceDatabase::ResourceInfo info = {};
			if (!m_ResourceDatabase->GetResourceInfo(RelativeFilePath.ChangeType<char16>(), info))
				return;

			WString finalPath = Utilities::GetDataFileName(info.GUID);

			ByteBuffer inBuffer(ResourceSystemAllocators::ResourceAllocator);
			if (!Utilities::ReadDataFile(Path::Combine(m_LibraryPath, finalPath), inBuffer))
				return;

			Source = ResourceFactory::GetProgramSource(inBuffer);
		}
	}
}