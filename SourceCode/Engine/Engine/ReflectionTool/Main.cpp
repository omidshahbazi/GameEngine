// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <MemoryManagement\Allocator\Initializer.h>
#include <ReflectionTool\ReflectionGenerator.h>
#include <Utility\ArgumentParser.h>

using namespace Engine::Common;
using namespace Engine::Utility;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::ReflectionTool;

void InitializerInitialize(void)
{
	Initializer::AllocatorInfo allocatorsInfo[MAX_ALLOCATORS_COUNT];
	uint32 allocatorInfoCount = 0;

	allocatorsInfo[allocatorInfoCount].Name = "Root Allocator";
	allocatorsInfo[allocatorInfoCount].ReserveSizeRate = 1;
	++allocatorInfoCount;

	Initializer::Create();
	Initializer::GetInstance()->Initialize(GigaByte * 3, allocatorsInfo, allocatorInfoCount);
}

int main(uint8 ArgumentsCount, const char8** Arguments)
{
	InitializerInitialize();

	ReflectionToolAllocators::Create();

	ArgumentParser args(ArgumentsCount, Arguments, true);

	if (args.GetCount() >= 2)
	{
		ReflectionGenerator rg(args.GetAsString(0), args.GetAsString(1));
		if (rg.Generate())
			return 0;
	}

	//ReflectionGenerator rg("D:\\Projects\\GameEngine\\SourceCode\\Engine\\Engine\\ResourceSystem\\Resource.h", "D:\\Projects\\GameEngine\\SourceCode\\Engine\\Intermediate\\ResourceSystem\\Generated\\ImExporter.Reflection");
	//if (rg.Generate())
	//	return 0;

	return 1;
}