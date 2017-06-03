#include <MemoryManagement\SharedMemory.h>
#include <MemoryManagement\Allocator\DefaultAllocator.h>
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagement\SharedMemory.h>
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
using namespace Engine::MemoryManagement;
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

extern template class MEMORYMANAGEMENT_API SharedMemory<Job<int>>;

int NewAdd()
{
	Job<int>* desc1 = RunJob(Add, 1, 2);
	Job<int>* desc2 = RunJob(Value2);

	while (!desc1->IsFinished() || !desc2->IsFinished())
		continue;

	int result = 0;

	for (int i = 0; i < 999999999; ++i)
		result += desc1->Get() + desc2->Get();

	return result;
}

void Do()
{
	int a = 0;
	a++;
}

struct aaa
{
	int b;
};

void main()
{
	//std::vector<aaa*> vec;

	//SharedMemory<aaa> bbb = NewSharedMemory< aaa>(Allocators::JobAllocator);

	//vec.push_back(&*bbb);


	//std::future<void> a1 = std::async(Do);
	//std::future<int> b1 = std::async(NewAdd);




	Job<int> *a = RunJob(NewAdd)->Then(Do);
	Job<void>* b = RunJob(Do);


	while (!a->IsFinished())
	{
		Engine::Platform::PlatformThread::Sleep(1000);
	}

	std::cout << a->Get();
}