// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DESCRIPTOR_VIEW_ALLOCATOR_H
#define DESCRIPTOR_VIEW_ALLOCATOR_H

#include <Rendering\Private\DirectX12\DirectX12Wrapper.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace DirectX12
			{
				const uint8 RESERVED_DESCRIPTOR_HEAP_COUNT = 10;
				const uint32 DESCRIPTOR_HEAP_COUNT[]{ 1'000'000, 2048, 2048, 2488 };

#define GET_DESCRIPTOR_MAX_COUNT() DESCRIPTOR_HEAP_COUNT[(uint32)m_Type]

#define IMPLETEMENT_ALLOCATE() \
				Assert(Resource != nullptr, "Resource cannot be null"); \
				Assert(Handle != nullptr, "Handle cannot be null"); \
				DescriptorInfo* info = nullptr; \
				uint32 index = 0; \
				if (!FindAFreeSlot(&info, &index)) \
					return false; \
				uint64 increment = index * m_HandleIncrementSize; \
				Handle->CPUHandle.ptr = info->DescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + increment; \
				bool result = ALLOCATE_INTERNAL(); \
				if (result) \
				{ \
					info->AllocatedStatus[index] = true; \
					Handle->GPUHandle.ptr = info->DescriptorHeap->GetGPUDescriptorHandleForHeapStart().ptr + increment; \
				} \
				return result;

				class DescriptorViewAllocator
				{
				private:
					struct DescriptorInfo
					{
					public:
						ID3D12DescriptorHeap* DescriptorHeap;
						bool* AllocatedStatus;
						D3D12_GPU_DESCRIPTOR_HANDLE StartHandle;
						D3D12_GPU_DESCRIPTOR_HANDLE EndHandle;
					};

				public:
					struct ViewHandle
					{
					public:
						D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle;
						D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle;
					};

				public:
					DescriptorViewAllocator(void) :
						m_IsInitialized(false),
						m_Device(nullptr),
						m_HandleIncrementSize(0),
						m_DescriptInfoCount(0)
					{
					}

					~DescriptorViewAllocator(void)
					{
						if (!m_IsInitialized)
							return;

						Deinitialize();
					}

					INLINE bool Initialize(ID3D12Device5* Device, D3D12_DESCRIPTOR_HEAP_TYPE Type)
					{
						Assert(!m_IsInitialized, "DescriptorViewAllocator is already initialized");

						m_Device = Device;
						m_Type = Type;

						m_HandleIncrementSize = m_Device->GetDescriptorHandleIncrementSize(m_Type);

						return true;
					}

					INLINE bool Deinitialize(void)
					{
						for (uint8 i = 0; i < m_DescriptInfoCount; ++i)
						{
							DescriptorInfo& info = m_DescriptorInfos[i];

							if (!DirectX12Wrapper::ReleaseInstance(info.DescriptorHeap))
								return false;

							RenderingAllocators::RenderingSystemAllocator_Deallocate(info.AllocatedStatus);
						}

						return true;
					}

					INLINE bool AllocateConstantBufferView(ViewTypes Type, ID3D12Resource1* Resource, ViewHandle* Handle)
					{
#define ALLOCATE_INTERNAL() DirectX12Wrapper::CreateConstantBufferView(m_Device, Resource, Handle->CPUHandle)

						IMPLETEMENT_ALLOCATE();

#undef ALLOCATE_INTERNAL
					}

					INLINE bool AllocateShaderResourceView(ViewTypes Type, ID3D12Resource1* Resource, ViewHandle* Handle)
					{
#define ALLOCATE_INTERNAL() DirectX12Wrapper::CreateShaderResourceView(m_Device, Resource, Handle->CPUHandle)

						IMPLETEMENT_ALLOCATE();

#undef ALLOCATE_INTERNAL
					}

					INLINE bool AllocateUnorderedAccessView(ViewTypes Type, ID3D12Resource1* Resource, ViewHandle* Handle)
					{
#define ALLOCATE_INTERNAL() DirectX12Wrapper::CreateUnorderedAccessView(m_Device, Resource, Handle->CPUHandle)

						IMPLETEMENT_ALLOCATE();

#undef ALLOCATE_INTERNAL
					}

//					INLINE bool AllocateSampler(ViewTypes Type, ViewHandle* Handle)
//					{
//#define ALLOCATE_INTERNAL() DirectX12Wrapper::CreateSampler(m_Device, Handle->CPUHandle)
//
//						IMPLETEMENT_ALLOCATE();
//
//#undef ALLOCATE_INTERNAL
//					}

					INLINE bool AllocateRenderTargetView(ViewTypes Type, ID3D12Resource1* Resource, ViewHandle* Handle)
					{
#define ALLOCATE_INTERNAL() DirectX12Wrapper::CreateRenderTargetView(m_Device, Resource, Handle->CPUHandle)

						IMPLETEMENT_ALLOCATE();

#undef ALLOCATE_INTERNAL
					}

					INLINE bool AllocateDepthStencilView(ViewTypes Type, ID3D12Resource1* Resource, ViewHandle* Handle)
					{
#define ALLOCATE_INTERNAL() DirectX12Wrapper::CreateDepthStencilView(m_Device, Resource, Handle->CPUHandle)

						IMPLETEMENT_ALLOCATE();

#undef ALLOCATE_INTERNAL
					}

					INLINE bool DestroyView(ViewHandle Handle)
					{
						for (uint8 i = 0; i < m_DescriptInfoCount; ++i)
						{
							DescriptorInfo& info = m_DescriptorInfos[i];

							if (Handle.GPUHandle.ptr < info.StartHandle.ptr || info.EndHandle.ptr <= Handle.GPUHandle.ptr)
								continue;

							info.AllocatedStatus[i] = false;

							return true;
						}

						return false;
					}

				private:
					INLINE bool FindAFreeSlot(DescriptorInfo** Info, uint32* Index)
					{
						uint32 descriptorCount = GET_DESCRIPTOR_MAX_COUNT();

						for (uint8 c = m_DescriptInfoCount; c > 0; ++c)
						{
							DescriptorInfo& info = m_DescriptorInfos[c - 1];

							for (uint32 i = 0; i < descriptorCount; ++i)
							{
								if (info.AllocatedStatus[i])
									continue;

								*Info = &info;
								*Index = i;

								return true;
							}
						}

						if (!InitializeNewDescriptorHeap())
							return false;

						return FindAFreeSlot(Info, Index);
					}

					INLINE bool InitializeNewDescriptorHeap(void)
					{
						Assert(m_DescriptInfoCount < RESERVED_DESCRIPTOR_HEAP_COUNT, "Reached max of reserved descriptor heaps");

						DescriptorInfo& info = m_DescriptorInfos[m_DescriptInfoCount];

						uint32 descriptorCount = GET_DESCRIPTOR_MAX_COUNT();

						if (!DirectX12Wrapper::CreateDescriptorHeap(m_Device, m_Type, descriptorCount, &info.DescriptorHeap))
							return false;

						info.StartHandle = info.DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
						info.EndHandle.ptr = info.StartHandle.ptr + (descriptorCount * m_HandleIncrementSize);

						info.AllocatedStatus = RenderingAllocators::RenderingSystemAllocator_AllocateArray<bool>(descriptorCount);
						PlatformMemory::Set(info.AllocatedStatus, 0, descriptorCount);

						++m_DescriptInfoCount;

						return true;
					}

				private:
					bool m_IsInitialized;
					ID3D12Device5* m_Device;
					D3D12_DESCRIPTOR_HEAP_TYPE m_Type;
					uint32 m_HandleIncrementSize;

					DescriptorInfo m_DescriptorInfos[RESERVED_DESCRIPTOR_HEAP_COUNT];
					uint8 m_DescriptInfoCount;
				};

#undef GET_DESCRIPTOR_MAX_COUNT
#undef IMPLETEMENT_ALLOCATE
			}
		}
	}
}

#endif