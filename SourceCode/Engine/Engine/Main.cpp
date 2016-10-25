#include "SharedMemory.h"
#include "DefaultAllocator.h"

void main()
{
	SharedMemory<int> a = 2;
	SharedMemory<int> b = 5;
	a = b;

	a = 10;
}