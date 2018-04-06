#include <MemoryManagement\SharedMemory.h>
#include <MemoryManagement\Allocator\DefaultAllocator.h>
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagement\SharedMemory.h>
#include <Parallelizing\JobManager.h>
#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformThread.h>
#include <Platform\PlatformFile.h>
#include <Common\BitwiseUtils.h>
#include <chrono>
#include <iostream>
#include <future>
#include <fstream>

#include <vector>

using namespace Engine::MemoryManagement;
using namespace Engine::Common;
using namespace Engine::MemoryManagement;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Parallelizing;
using namespace Engine::Platform;

int Add(int a, int b)
{
	return a + b;
}

int Value1()
{
	return 6;
}

int Value2()
{
	return 6;
}

extern template class MEMORYMANAGEMENT_API SharedMemory<Job<int>>;

int NewAdd()
{
	Job<int> desc1 = RunJob([](int a, int b) { return Add(a, b); }, 1, 2);
	Job<int> desc2 = RunJob(Value2);

	while (!desc1.IsFinished() || !desc2.IsFinished())
		continue;

	int result = 0;

	for (int i = 0; i < 22; ++i)
		result += desc1.Get() + desc2.Get();

	return result;
}

void ReadFile(cstr Path)
{
	PlatformFile::Handle handle = PlatformFile::Open(Path, PlatformFile::OpenModes::Binary | PlatformFile::OpenModes::Input);

	uint64 size = PlatformFile::Size(handle);

	byte *buffer = new byte[size];

	PlatformFile::Read(handle, buffer, size);

	std::cout << Path;
}

void main()
{
	JobManager::Create(DefaultAllocator::GetInstance());
	//	make waiting mechanism

	Job<int> a = RunJob(NewAdd);

	while (!a.IsFinished())
	{
		Engine::Platform::PlatformThread::Sleep(1000);
	}

	JobManager::Destroy();
}