#include <Platform\PlatformFile.h>
#include <Common\BitwiseUtils.h>

using namespace Engine::Platform;
using namespace Engine::Common;

void main()
{
	PlatformFile::Handle handle = PlatformFile::Open("E:/1.txt", PlatformFile::Modes::Output | PlatformFile::Modes::WideCharacter);


	PlatformFile::Write(handle, 10);
	PlatformFile::Write(handle, "aasdasd");
	PlatformFile::Write(handle, L'س');




	PlatformFile::Close(handle);
}