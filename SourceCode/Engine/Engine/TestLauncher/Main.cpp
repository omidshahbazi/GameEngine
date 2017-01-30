#include <MemoryManagement\SharedMemory.h>
#include <MemoryManagement\Allocator\DefaultAllocator.h>
#include <MemoryManagement\Allocator\FixedSizeAllocator.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
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

//class Test1
//{
//private:
//	template <typename ...Parameters> struct __Job_worker
//	{
//	public:
//		__Job_worker(void)
//		{}
//
//		static void Worker(void *Arguments)
//		{
//			__Job_worker *arguments = reinterpret_cast<__Job_worker*>(Arguments);
//			arguments->Instance->worker(std::get<Parameters>(arguments->Arguments)...);
//		}
//
//		template <typename ...Parameters> static JobDescription *Execute(Test1 *Instance, Parameters... Params)
//		{
//			__Job_worker<Parameters...> *arguments = new __Job_worker<Parameters...>;
//			arguments->Instance = Instance;
//			arguments->Arguments = std::tuple<Parameters...>(Params...);
//
//			return AddJob(__Job_worker<Parameters...>::Worker, arguments);
//		}
//
//	public:
//		Test1 *Instance;
//		std::tuple<Parameters...> Arguments;
//	};
//
//	void worker(int start)
//	{
//		for (int i = 0; i < 100000; i++)
//		{
//			std::cout << 1;
//		}
//	}
//
//	JobDescription **job = nullptr;
//
//public:
//	void RunJob()
//	{
//		__Job_worker<>::Execute(this, 11);
//	}
//};


void main()
{
	A t;
	t.RunJob();

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