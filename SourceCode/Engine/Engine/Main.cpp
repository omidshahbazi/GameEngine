#include "SharedMemory.h"
#include "DefaultAllocator.h"

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
	SharedMemory<A> bbb = NewSharedMemory<A>(123);

	bbb->m_x = 10;

	SharedMemory<int> a = 2;
	SharedMemory<int> b = 5;
	a = b;

	a = 10;
}