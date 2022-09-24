// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Allocators\RootAllocator.h>
#include <ReflectionGenerator\ReflectionGenerator.h>
#include <ReflectionGenerator\ReflectionGeneratorException.h>
#include <EntryPointUtility\ArgumentParser.h>
#include <EntryPointUtility\EntryPoint.h>
#include <iostream>

using namespace Engine::Common;
using namespace Engine::EntryPointUtility;
using namespace Engine::Allocators;
using namespace Engine::ReflectionGenerator;
using namespace Engine::Containers;

BEGIN_ENTRY_POINT
{
	DefaultAllocator::Create();
	RootAllocator::Create(DefaultAllocator::GetInstance());

	DynamicSizeAllocator allocator("Types Allocator", RootAllocator::GetInstance(), 10 * MegaByte);

	//ArgumentParser args(ArgumentCount, Arguments, true);
	//if (args.GetCount() < 2)
	//{
	//	return 2;
	//}

	//WString filePath = args.GetAsWString(0);
	//WString outputBaseFileName = args.GetAsWString(1);

	WString filePath = LR"(D:\Projects\GameEngine1\Engine\Source\Runtime\ResourceSystem\ResourceImportExport\ImporterExporter.h)";
	WString outputBaseFileName = LR"(D:\Projects\GameEngine1\Engine\Intermediate\ResourceImportExport\Generated\ImporterExporter.Reflection)";

	//WString filePath = LR"(D:\ReflectionTest\Test.h)";
	//WString outputBaseFileName = LR"(D:\ReflectionTest\Generated\Test.Reflection)";

	try
	{
		bool result = ReflectionGenerator::Generate(&allocator, filePath, outputBaseFileName);

		return (result ? 0 : 1);
	}
	catch (const Exception& ex)
	{
		std::cout << ex.ToString().GetValue();

		return 2;
	}
}
END_ENTRY_POINT