#include <MemoryManagerment\SharedMemory.h>
#include <MemoryManagerment\Allocator\DefaultAllocator.h>
#include <MemoryManagerment\Allocator\FixedSizeAllocator.h>
#include <MemoryManagerment\Allocator\DynamicSizeAllocator.h>
#include <Parallelizing\JobManager.h>
#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformThread.h>
#include <chrono>
#include <iostream>

#include <vector>

using namespace Engine::MemoryManagement;
using namespace Engine::Common;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Parallelizing;


JobManager jobManager;

void job(void *Arguments)
{
	for (int i = 0; i < 100000; i++)
	{
		std::cout << 1;
	}

	std::cout << "\n";
}


void job1(void *Arguments)
{
	std::cout << 1 << "\n";

	for (int i = 0; i < 100; ++i)
		AddJob(job, 0);
}


void job2(void *Arguments)
{
	std::cout << 2 << "\n";
}



void job3(void *Arguments)
{
	std::cout << 3 << "\n";
}



void job4(void *Arguments)
{
	std::cout << 4 << "\n";
}



void job5(void *Arguments)
{
	std::cout << 5 << "\n";
}



void job6(void *Arguments)
{
	std::cout << 6 << "\n";
}



void job7(void *Arguments)
{
	std::cout << 7 << "\n";
}


void main()
{
	//for (int i = 0; i < 100; ++i)
	//	jobManager.Add(job);

	JobDescription * jobDesc1 = AddJob(job1, nullptr);
	JobDescription * jobDesc2 = AddJob(job2, nullptr);
	JobDescription * jobDesc3 = AddJob(job3, nullptr);
	JobDescription * jobDesc4 = AddJob(job4, nullptr);
	JobDescription * jobDesc5 = AddJob(job5, nullptr);
	JobDescription * jobDesc6 = AddJob(job6, nullptr);
	JobDescription * jobDesc7 = AddJob(job7, nullptr);

	while (true)
	{
		Engine::Platform::PlatformThread::Sleep(1000);
	}
}






//class A
//{
//
//public:
//	int m_x;
//	A(int x) :
//		m_x(x)
//	{
//
//	}
//};

//uint64 desiredSize = sizeof(A);
//uint64 testCount = 100000000;
//
//DefaultAllocator defaultAllocator;
//FixedSizeAllocator fixedAllocator(desiredSize, testCount);
//DynamicSizeAllocator dynamicAllocator(28 * testCount);


//void DoTest(AllocatorBase *Allocator, uint64 DesireSize, uint64 TestCount)
//{
//	std::chrono::nanoseconds startTime = std::chrono::high_resolution_clock::now().time_since_epoch();
//
//	for (uint64 i = 0; i < TestCount; ++i)
//	{
//		byte *handle = Allocator->Allocate(DesireSize);
//		//Allocator->Deallocate(handle);
//	}
//
//	std::chrono::nanoseconds endTime = std::chrono::high_resolution_clock::now().time_since_epoch();
//
//	std::cout << "For (" << TestCount << ") took " << ((endTime - startTime).count() / 1000000000.0F) << "s\n";
//}

//SharedMemory<A> GetTest()
//{
//	return NewSharedMemory<A>(defaultAllocator, 3);
//}

//void main()
//{
//	SharedMemory<int> a = NewSharedMemory<int>(defaultAllocator, 1);
//
//
//	//SharedMemory<A> b(A(1));
//	//SharedMemory<int> b(2);
//
//	//if (a != 1)
//	//	b = a;
//
//	//if (a == b)
//	//	a = b;
//
//	//a = 54;
//
//	//std::cout << "Start Test for DefaultAllocator" << "\n";
//	//DoTest(&defaultAllocator, desiredSize, testCount);
//	//std::cout << "Finish Test for DefaultAllocator" << "\n";
//
//	//std::cout << "Start Test for FixedSizeAllocator" << "\n";
//	//DoTest(&fixedAllocator, 1, testCount);
//	//std::cout << "Finish Test for FixedSizeAllocator" << "\n";
//
//	//std::cout << "Start Test for DynamicSizeAllocator" << "\n";
//	//DoTest(&dynamicAllocator, desiredSize, testCount);
//	//std::cout << "Finish Test for DynamicSizeAllocator" << "\n";
//
//
//	_mm_pause();
//}