#include <Platform\PlatformFile.h>

using namespace Engine::Platform;

void main()
{
	PlatformFile::Handle handle = PlatformFile::Open("E:/1.txt", PlatformFile::Modes::Output);


	PlatformFile::Write(handle, 10);
	//PlatformFile::Write(handle, "aasdasd");
	//PlatformFile::Write(handle, L'س');




	PlatformFile::Close(handle);
}