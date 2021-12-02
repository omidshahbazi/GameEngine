// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Allocators\RootAllocator.h>
#include <ReflectionTool\ReflectionGenerator.h>
#include <EntryPointUtility\ArgumentParser.h>
#include <EntryPointUtility\EntryPoint.h>

using namespace Engine::Common;
using namespace Engine::EntryPointUtility;
using namespace Engine::Allocators;
using namespace Engine::ReflectionTool;

BEGIN_ENTRY_POINT
{
	DefaultAllocator::Create();
	RootAllocator::Create(DefaultAllocator::GetInstance());

	ReflectionToolAllocators::Create();

	ArgumentParser args(ArgumentCount, Arguments, true);

	if (args.GetCount() >= 2)
	{
		ReflectionGenerator rg(args.GetAsWString(0), args.GetAsWString(1));
		if (rg.Generate())
			return 0;
	}

	//ReflectionGenerator rg(L"D:\\Projects\\GameEngine\\Engine\\Source\\Runtime\\ResourceSystem\\ResourceImportExport\\ImporterExporter.h", L"D:\\Projects\\GameEngine\\Engine\\Intermediate\\ResourceImportExport\\Generated\\ImExporter.Reflection");
	//if (rg.Generate())
	//	return 0;

	return 1;
}
END_ENTRY_POINT