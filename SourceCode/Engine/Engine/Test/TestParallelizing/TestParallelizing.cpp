#include <Common\BitwiseUtils.h>
#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformFile.h>
#include <MemoryManagement\Allocator\Initializer.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Parallelizing\JobManager.h>
#include <iostream>

using namespace Engine::Common;
using namespace Engine::Platform;
using namespace Engine::MemoryManagement::Allocator;
using namespace Engine::Parallelizing;

#include <Containers/Queue.h>
using namespace Engine::Containers;

const uint32 COUNT = 1000000;
int32 arr1[COUNT];
int32 arr2[COUNT];
int32 res[COUNT];

void CalculateSumPartial(int32* Array1, int32* Array2, int32* Result, uint32 Index, uint32 Count)
{
	for (uint32 i = Index; i < Index + Count; ++i)
		Result[i] = Array1[i] + Array2[i];
}

void CalculateSum(int32* Array1, int32* Array2, int32* Result, uint32 Count)
{
	WaitFor(RunJob([](int32* Array1, int32* Array2, int32* Result, uint32 Count)
		{
			for (uint32 i = 0; i < Count; ++i)
			{
				Array1[i] = i;
				Array2[i] = (Count - i);
				Result[i] = 0;
			}
		}, Array1, Array2, Result, Count));

	const uint8 SUB_PART_COUNT = 10;

	const uint32 PARTIAL_COUNT = Count / SUB_PART_COUNT;

	Job<void> jobs[SUB_PART_COUNT];

	for (uint8 i = 0; i < SUB_PART_COUNT; ++i)
	{
		uint8 index = i;
		jobs[i] = RunJob(CalculateSumPartial, Array1, Array2, Result, index * PARTIAL_COUNT, PARTIAL_COUNT);
	}

	WaitFor(jobs, SUB_PART_COUNT);
}

void ReadFile(cwstr Path)
{
	WaitFor(RunJob([]()
		{
			for (uint64 i = 0; i < 10000; ++i);

			std::cout << "Counting Done" << std::endl;
		}));

	PlatformFile::Handle handle = PlatformFile::Open(Path, PlatformFile::OpenModes::Binary | PlatformFile::OpenModes::Input);

	if (handle == 0)
		return;

	uint64 size = PlatformFile::Size(handle);

	byte* buffer = new byte[size];

	PlatformFile::Read(handle, buffer, size);

	PlatformFile::Close(handle);

	delete[] buffer;

	std::cout << "ReadFile" << std::endl;
}

void main()
{
	Initializer::Create();
	Initializer::GetInstance()->Initialize(GigaByte * 3, L"Alllocators.data");

	JobManager::Create(RootAllocator::GetInstance());

	for (int i = 0; i < 100; ++i)
	{
		Job<void> readFileJob = RunJob(ReadFile, L"D:/1.mkv");

		auto sumJob = RunJob(CalculateSum, arr1, arr2, res, COUNT);

		WaitFor(readFileJob);

		WaitFor(sumJob);

		std::cout << "Done " << i << std::endl;

		getchar();
	}

	JobManager::Destroy();
}