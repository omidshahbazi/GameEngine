// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>
#include <FontSystem\FontGenerator.h>
#include <Utility\FileSystem.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

using namespace Engine::Common;
using namespace Engine::Containers;
using namespace Engine::FontSystem;
using namespace Engine::Utility;
using namespace Engine::MemoryManagement::Allocator;

void main()
{
	const WString ttfPath = L"D:/calibri.ttf";
	uint64 fileSize = FileSystem::GetSize(ttfPath);
	{
		byte *fontBuffer = AllocateMemory(RootAllocator::GetInstance(), fileSize);
		FileSystem::ReadAllBytes(ttfPath, fontBuffer, fileSize);

		FontGenerator fontGenerator;
		fontGenerator.LoadFont(fontBuffer, fileSize);
		fontGenerator.Generate(L"D:/Projects/GameEngineAssets/Assets/calibri.font");

		DeallocateMemory(RootAllocator::GetInstance(), fontBuffer);
	}
}