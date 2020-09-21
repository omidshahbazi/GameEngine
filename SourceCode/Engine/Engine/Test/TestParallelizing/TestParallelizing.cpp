#include <MemoryManagement\Allocator\Initializer.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
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

#include <Containers/Stack.h>
using namespace Engine::Containers;

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

AtomicInt32 counter;

void ReadFile(cwstr Path)
{
	PlatformFile::Handle handle = PlatformFile::Open(Path, PlatformFile::OpenModes::Binary | PlatformFile::OpenModes::Input);

	uint64 size = PlatformFile::Size(handle);

	byte* buffer = new byte[size];

	PlatformFile::Read(handle, buffer, size);

	std::cout << "ReadFile" << std::endl;
}

int NewAdd()
{
	//Job<void> r1 = RunJob(ReadFile, L"D:/1.mkv");

	//WaitFor(r1);

	Job<int> desc1 = RunJob([](int a, int b) { return Add(a, b); }, 1, 2);
	//Job<int> desc2 = RunJob(Value2);

	WaitFor(desc1);
	//WaitFor(desc2);

	int result = 0;

	//for (int i = 0; i < 22; ++i)
	//	result += desc1.Get() + desc2.Get();

	++counter;

	return result;
}

void main()
{
	Initializer::Create();
	Initializer::GetInstance()->Initialize(GigaByte * 3, L"Alllocators.data");

	Stack<int> st;
	st.Push(1);

	JobManager::Create(RootAllocator::GetInstance());

	char8 c;
	for (int i = 0; i < 100; ++i)
	{
		counter = 0;

		//Job<void> r1 = RunJob(ReadFile, L"D:/1.mkv");

		auto a = RunJob(NewAdd);


		//r1.Wait();

		a.Wait();

		const int count = 10;
		Job<int> list[count];

		for (int i = 0; i < count; ++i)
		{
			list[i] = RunJob(NewAdd);
		}

		for (int i = 0; i < count; ++i)
		{
			list[i].Wait();
		}

		std::cout << "Done " << counter << std::endl;

		getchar();
	}

	JobManager::Destroy();
}