// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <MemoryManagement\Allocator\Initializer.h>
#include <ReflectionTool\ReflectionGenerator.h>
#include <Utility\ArgumentParser.h>

using namespace Engine::Common;
using namespace Engine::Utility;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::ReflectionTool;

int main(uint8 ArgumentsCount, const char8** Arguments)
{
	Initializer::Create();
	Initializer::GetInstance()->Initialize(GigaByte * 3, L"Alllocators.data");

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