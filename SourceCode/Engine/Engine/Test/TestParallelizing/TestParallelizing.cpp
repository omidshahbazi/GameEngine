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
	JobDescription *desc1 = RunJob(Value1);
	JobDescription *desc2 = RunJob(Value2);

	while (!desc1->IsFinished() || !desc2->IsFinished())
		continue;

	return 2;
}

void main()
{
	RunJob(NewAdd);

	while (true)
	{
		Engine::Platform::PlatformThread::Sleep(1000);
	}
}