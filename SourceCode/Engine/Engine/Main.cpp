#include <MemoryManagerment\SharedMemory.h>
#include <MemoryManagerment\Allocator\DefaultAllocator.h>
#include <MemoryManagerment\Allocator\FixedSizeAllocator.h>
#include <Common\PrimitiveTypes.h>
#include <ctime>
#include <iostream>

#include <vector>

using namespace Engine::MemoryManagement;
using namespace Engine::Common;
using namespace Engine::MemoryManagement::Allocator;

class A
{

public:
	int m_x;
	A(int x) :
		m_x(x)
	{

	}
};

void main()
{
	uint64 desiredSize = sizeof(A);
	uint64 testCount = 100000000;

	FixedSizeAllocator fixedAllocator(desiredSize, testCount);

	int64 startTime = std::time(nullptr);

	for (uint64 i = 0; i < testCount; ++i)
	{
		byte *handle = fixedAllocator.Allocate(1);
		fixedAllocator.Deallocate(handle);
	}

	std::cout << (std::time(nullptr) - startTime) << "\n";

	DefaultAllocator defaultAllocator;

	startTime = std::time(nullptr);

	for (uint64 i = 0; i < testCount; ++i)
	{
		byte *handle = defaultAllocator.Allocate(desiredSize);
		defaultAllocator.Deallocate(handle);
	}

	std::cout << (std::time(nullptr) - startTime);

	_mm_pause();


	//handle = fixedAllocator.Allocate(1);
	//new (handle) A(2);
	//fixedAllocator.Deallocate(handle);

	//handle = fixedAllocator.Allocate(1);
	//new (handle) A(3);

	//handle = fixedAllocator.Allocate(1);
	//new (handle) A(4);
	//fixedAllocator.Deallocate(handle);

	//handle = fixedAllocator.Allocate(1);
	//new (handle) A(5);

	//handle = fixedAllocator.Allocate(1);
	//new (handle) A(6);

	//byte *handle1 = fixedAllocator.Allocate(1);
	//new (handle1) A(7);

	//byte *handle2 = fixedAllocator.Allocate(1);
	//new (handle2) A(8);

	//fixedAllocator.Deallocate(handle1);
	//fixedAllocator.Deallocate(handle2);

	//byte *handle = fixedAllocator.Allocate(1);
	//new (handle) A(9);

	//handle = fixedAllocator.Allocate(1);
	//new (handle) A(19);

	//FixedSizeAllocator fixedAllocator1(sizeof(A), 1000);

	//handle = fixedAllocator1.Allocate(1);

}