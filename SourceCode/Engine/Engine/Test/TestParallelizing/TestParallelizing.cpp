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

class A
{
private:
	DECLARE_AND_DEFINE_JOB(A, Do)(int start)
	{
		for (int i = 0; i < 1000000; ++i)
			std::cout << start << "\n";
	}

	DECLARE_JOB(A, Do1)

	DEFINE_JOB(Do1)(int a)
	{

	}

	DEFINE_JOB(Do1)(float a)
	{

	}

public:
	void RunJob()
	{
		job = new JobDescription*[1000];

		for (int i = 0; i < 1000; ++i)
		{
			job[i] = RUN_JOB(Do, 1.0F);


			//RUN_JOB(Do1, 1.0F);
			//RUN_JOB(Do1, 1);
		}
	}

	JobDescription **job = nullptr;
};

typedef Engine::Containers::DynamicString<char> String;

//void main()
//{
//
//	A t;
//	t.RunJob();
//
//	while (true)
//	{
//		Engine::Platform::PlatformThread::Sleep(1000);
//	}
//}