// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>
#include <FontSystem\FontGenerator.h>
#include <Utility\FileSystem.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Utility\ArgumentParser.h>
#include <Debugging\Debug.h>>

using namespace Engine::Common;
using namespace Engine::Debugging;
using namespace Engine::Containers;
using namespace Engine::FontSystem;
using namespace Engine::Utility;
using namespace Engine::MemoryManagement::Allocator;

void main(uint8 ArgumentsCount, const char8 **Arguments)
{
	ArgumentParser args(ArgumentsCount, Arguments);

	if (args.GetCount() < 2)
	{
		Debug::LogInfo("Parameters must be in order of \"TTF-File-Path\" \"Font-File-Path\"");
		return;
	}

	const WString ttfPath = args.GetAsString(0).ChangeType<WString::CharType>();
	const WString fontPath = args.GetAsString(1).ChangeType<WString::CharType>();

	uint64 fileSize = FileSystem::GetSize(ttfPath);

	if (fileSize == 0)
	{
		Debug::LogError("Specified *.ttf file is invalid");
		return;
	}

	Debug::LogInfo("Reading TTF file content");

	byte *fontBuffer = AllocateMemory(RootAllocator::GetInstance(), fileSize);
	FileSystem::ReadAllBytes(ttfPath, fontBuffer, fileSize);


	FontGenerator fontGenerator;
	fontGenerator.LoadFont(fontBuffer, fileSize);
	fontGenerator.Generate(fontPath);

	Debug::LogInfo("Cleaning up memory");
	DeallocateMemory(RootAllocator::GetInstance(), fontBuffer);
}