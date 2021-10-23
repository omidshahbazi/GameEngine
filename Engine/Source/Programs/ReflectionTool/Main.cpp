// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Allocators\RootAllocator.h>
#include <ReflectionTool\ReflectionGenerator.h>
#include <EntryPointUtility\ArgumentParser.h>

using namespace Engine::Common;
using namespace Engine::EntryPointUtility;
using namespace Engine::Allocators;
using namespace Engine::ReflectionTool;

int main(uint8 ArgumentsCount, const char8** Arguments)
{
	DefaultAllocator::Create();
	RootAllocator::Create(DefaultAllocator::GetInstance());

	ReflectionToolAllocators::Create();

	ArgumentParser args(ArgumentsCount, Arguments, true);

	if (args.GetCount() >= 2)
	{
		ReflectionGenerator rg(args.GetAsString(0), args.GetAsString(1));
		if (rg.Generate())
			return 0;
	}

	//ReflectionGenerator rg("D:\\Projects\\GameEngine\\Engine\\Source\\Runtime\\ResourceSystem\\ResourceImportExport\\ImExporter.h", "D:\\Projects\\GameEngine\\Engine\\Intermediate\\ResourceImportExport\\Generated\\ImExporter.Reflection");
	//if (rg.Generate())
	//	return 0;

	return 1;
}