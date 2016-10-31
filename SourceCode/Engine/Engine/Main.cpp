#include <MemoryManagerment\SharedMemory.h>
#include <MemoryManagerment\Allocator\DefaultAllocator.h>
#include <MemoryManagerment\Allocator\FixedSizeAllocator.h>

#include <vector>

using namespace Engine::MemoryManagement;
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
	FixedSizeAllocator fixedAllocator(sizeof(A), 1000);

	MemoryHandle *handle = fixedAllocator.Allocate(1);
	new (handle->Get()) A(1);

	handle = fixedAllocator.Allocate(1);
	new (handle->Get()) A(2);
	handle->Drop();

	handle = fixedAllocator.Allocate(1);
	new (handle->Get()) A(3);

	handle = fixedAllocator.Allocate(1);
	new (handle->Get()) A(4);
	handle->Drop();

	handle = fixedAllocator.Allocate(1);
	new (handle->Get()) A(5);

	handle = fixedAllocator.Allocate(1);
	new (handle->Get()) A(6);

	handle = fixedAllocator.Allocate(1);
	new (handle->Get()) A(7);
	handle->Drop();

	handle = fixedAllocator.Allocate(1);
	new (handle->Get()) A(8);
	handle->Drop();

	handle = fixedAllocator.Allocate(1);
	new (handle->Get()) A(9);

	FixedSizeAllocator fixedAllocator1(sizeof(A), 1000);

	handle = fixedAllocator1.Allocate(1);



	fixedAllocator.Defragment();
}