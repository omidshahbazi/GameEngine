﻿#include <Platform\PlatformFile.h>
#include <Common\BitwiseUtils.h>

using namespace Engine::Platform;
using namespace Engine::Common;

void main()
{
	PlatformFile::Handle handle = PlatformFile::Open(L"E:/1.txt", PlatformFile::OpenModes::Output | PlatformFile::OpenModes::WideCharacter);


	PlatformFile::Write(handle, 10);
	PlatformFile::Write(handle, "aasdasd");
	PlatformFile::Write(handle, L'س');




	PlatformFile::Close(handle);
}