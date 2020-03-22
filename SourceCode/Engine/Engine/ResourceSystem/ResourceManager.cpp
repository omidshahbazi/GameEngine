// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceManager.h>
#include <Utility\FileSystem.h>
#include <Utility\Path.h>

namespace Engine
{
	using namespace Utility;

	namespace ResourceSystem
	{
		using namespace Private;

		const WString ASSETS_DIRECTORY_NAME(L"Assets");
		const WString LIBRARY_DIRECTORY_NAME(L"Library");

		const String DEFAULT_SHADER_NAME("Default.shader");
		String DEFAULT_SHADER_SOURCE("float3 pos : POSITION;const matrix4 _MVP;float4 VertexMain(){return _MVP * float4(pos, 1);}float4 FragmentMain(){return float4(1, 0, 1, 1);}");

		SINGLETON_DEFINITION(ResourceManager)

		ResourceManager::ResourceManager(void) : 
			ResourceHolder(Path::Combine(FileSystem::GetWorkingPath(), ASSETS_DIRECTORY_NAME), Path::Combine(FileSystem::GetWorkingPath(), LIBRARY_DIRECTORY_NAME))
		{
			ResourceFactory::Create(&ResourceSystemAllocators::ResourceAllocator);

			CreateDefaultResources();
		}

		ResourceManager::~ResourceManager(void)
		{
		}

		ProgramResource ResourceManager::GetDefaultProgram(void)
		{
			return Load<Program>(DEFAULT_SHADER_NAME);
		}

		void ResourceManager::CreateDefaultResources(void)
		{
			LoadProgram(DEFAULT_SHADER_NAME, DEFAULT_SHADER_SOURCE);
		}
	}
}