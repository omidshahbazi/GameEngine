#include <MemoryManagement\SharedMemory.h>
#include <MemoryManagement\Allocator\DefaultAllocator.h>
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <Parallelizing\JobManager.h>
#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformThread.h>
#include <Containers\DynamicString.h>
#include <chrono>
#include <iostream>
#include <future>

#include <vector>

using namespace Engine::MemoryManagement;
using namespace Engine::Common;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Parallelizing;

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

int NewAdd()
{
	Job<int> *desc1 = RunJob(Add, 1, 2);
	Job<int> *desc2 = RunJob(Value2);

	while (!desc1->IsFinished() || !desc2->IsFinished())
		continue;

	return 2;
}

void Do()
{
	int a = 0;
	a++;
}

void main()
{
	std::future<void> a1 = std::async(Do);
	std::future<int> b1 = std::async(NewAdd);


	Job<int> *a = RunJob(NewAdd);
	Job<void> *b = RunJob(Do);


	while (true)
	{
		Engine::Platform::PlatformThread::Sleep(1000);
	}
}