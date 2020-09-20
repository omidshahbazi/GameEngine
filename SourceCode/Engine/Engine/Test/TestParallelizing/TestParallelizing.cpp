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

int NewAdd(int a, int b)
{
	Job<int> desc1 = RunJob([](int a, int b) { return Add(a, b); }, a, b);
	Job<int> desc2 = RunJob(Value2);

	WaitFor(desc1);
	WaitFor(desc2);

	int result = 0;

	for (int i = 0; i < 22; ++i)
		result += desc1.Get() + desc2.Get();

	std::cout << "NewAdd" << std::endl;

	return result;
}

void ReadFile(cwstr Path)
{
	PlatformFile::Handle handle = PlatformFile::Open(Path, PlatformFile::OpenModes::Binary | PlatformFile::OpenModes::Input);

	uint64 size = PlatformFile::Size(handle);

	byte* buffer = new byte[size];

	PlatformFile::Read(handle, buffer, size);

	std::cout << "ReadFile" << std::endl;
}

void main()
{
	Initializer::Create();
	Initializer::GetInstance()->Initialize(GigaByte * 3, L"Alllocators.data");

	Stack<int> st;
	st.Push(1);


	JobManager::Create(RootAllocator::GetInstance());

	//Job<void> r1 = RunJob(ReadFile, L"D:/1.mkv");

	auto a = RunJob(NewAdd, 1, 2);


	//r1.Wait();

	a.Wait();

	for (int i = 0; i < 10; ++i)
	{
		RunJob(NewAdd, i, i * 2);
	}

	getchar();

	JobManager::Destroy();
}