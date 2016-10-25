#include "SharedMemory.h"

void main()
{
	SharedMemory<int> a;
	SharedMemory<int> b;
	a = 2;
	b = 3;

	bool flag = a == b;
	bool flag1 = a == 3;
	bool flag2 = b == 3;
	SharedMemory<int> bb = a;

	int *aa = a;
}