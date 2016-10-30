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

	MemoryHandle *handle = fixedAllocator.Allocate();
	
	new (handle->Get()) A(1);

	handle = fixedAllocator.Allocate();

	new (handle->Get()) A(2);

	DefaultAllocator defaultAllocator;

	SharedMemory<A> bbb(123);


	bbb->m_x = 10;

	SharedMemory<int> a = 2;
	SharedMemory<int> b = 5;
	a = b;

	a = 10;
}