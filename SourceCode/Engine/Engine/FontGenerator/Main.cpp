// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Common\PrimitiveTypes.h>
#include <Containers\Strings.h>
#include <FontSystem\FontLoader.h>
#include <Utility\FileSystem.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Utility\ArgumentParser.h>
#include <Debugging\Debug.h>
#include <MemoryManagement\Allocator\FrameAllocator.h>

using namespace Engine::Common;
using namespace Engine::Debugging;
using namespace Engine::Containers;
using namespace Engine::FontSystem;
using namespace Engine::Utility;
using namespace Engine::MemoryManagement::Allocator;

void main(uint8 ArgumentsCount, const char8** Arguments)
{
	//ArgumentParser args(ArgumentsCount, Arguments);

	//if (args.GetCount() < 2)
	//{
	//	Debug::LogInfo("Parameters must be in order of \"TTF-File-Path\" \"Font-File-Path\"");
	//	return;
	//}

	//const WString ttfPath = args.GetAsString(0).ChangeType<WString::CharType>();
	//const WString fontPath = args.GetAsString(1).ChangeType<WString::CharType>();

	const WString ttfPath = L"D:/Projects/GameEngine/SourceCode/Engine/Contents/Roboto-Light.ttf";
	const WString fontPath = L"D:/Projects/GameEngine/SourceCode/Engine/Contents/Roboto-Light.font";




	uint64 fileSize = FileSystem::GetSize(ttfPath);

	if (fileSize == 0)
	{
		Debug::LogError("Specified *.ttf file is invalid");
		return;
	}

	byte* fontBuffer = AllocateMemory(RootAllocator::GetInstance(), fileSize);
	FileSystem::ReadAllBytes(ttfPath, fontBuffer, fileSize);

	const uint64 FONT_ALLOCATOR_SIZE = 500 * MegaByte;
	FrameAllocator fontAllocator("Font Generator Allocator", RootAllocator::GetInstance(), FONT_ALLOCATOR_SIZE);
	ByteBuffer buffer(&fontAllocator, FONT_ALLOCATOR_SIZE);

	FontLoader::CreateFontFromTrueTypeFont(ByteBuffer(fontBuffer, 0, fileSize), buffer, Font::RenderTypes::Texture);

	FileSystem::WriteAllBytes(fontPath, buffer.GetBuffer(), buffer.GetSize());

	DeallocateMemory(RootAllocator::GetInstance(), fontBuffer);

	Debug::LogInfo("Font Generated");
}