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
				Assert(Handle != nullptr, "Handle cannot be null"); \
				DescriptorInfo* info = nullptr; \
				uint32 index = 0; \
				if (!FindAFreeSlot(&info, &index)) \
					return false; \
				uint64 increment = index * m_HandleIncrementSize; \
				Handle->DescriptorHeap = info->DescriptorHeap; \
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
						ID3D12DescriptorHeap* DescriptorHeap;
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

					INLINE bool Initialize(ID3D12Device5* Device, D3D12_DESCRIPTOR_HEAP_TYPE Type, D3D12_DESCRIPTOR_HEAP_FLAGS Flags)
					{
						Assert(!m_IsInitialized, "DescriptorViewAllocator is already initialized");

						m_Device = Device;
						m_Type = Type;
						m_Flags = Flags;

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

					INLINE bool AllocateConstantBufferView(ID3D12Resource1* Resource, ViewHandle* Handle)
					{
						Assert(Resource != nullptr, "Resource cannot be null");

#define ALLOCATE_INTERNAL() DirectX12Wrapper::Resource::CreateConstantBufferView(m_Device, Resource, Handle->CPUHandle)

						IMPLETEMENT_ALLOCATE();

#undef ALLOCATE_INTERNAL
					}

					INLINE bool AllocateTextureShaderResourceView(ID3D12Resource1* Resource, DXGI_FORMAT Format, D3D12_RESOURCE_DIMENSION Dimension, ViewHandle* Handle)
					{
						Assert(Resource != nullptr, "Resource cannot be null");

#define ALLOCATE_INTERNAL() DirectX12Wrapper::Resource::CreateTextureShaderResourceView(m_Device, Resource, Format, Dimension, Handle->CPUHandle)

						IMPLETEMENT_ALLOCATE();

#undef ALLOCATE_INTERNAL
					}

					INLINE bool AllocateUnorderedAccessView(ID3D12Resource1* Resource, ViewHandle* Handle)
					{
						Assert(Resource != nullptr, "Resource cannot be null");

#define ALLOCATE_INTERNAL() DirectX12Wrapper::Resource::CreateUnorderedAccessView(m_Device, Resource, Handle->CPUHandle)

						IMPLETEMENT_ALLOCATE();

#undef ALLOCATE_INTERNAL
					}

					INLINE bool AllocateSampler(const D3D12_SAMPLER_DESC& Description, ViewHandle* Handle)
					{
#define ALLOCATE_INTERNAL() DirectX12Wrapper::Resource::CreateSampler(m_Device, Description, Handle->CPUHandle)

						IMPLETEMENT_ALLOCATE();

#undef ALLOCATE_INTERNAL
					}

					INLINE bool AllocateRenderTargetView(ID3D12Resource1* Resource, ViewHandle* Handle)
					{
						Assert(Resource != nullptr, "Resource cannot be null");

#define ALLOCATE_INTERNAL() DirectX12Wrapper::Resource::CreateRenderTargetView(m_Device, Resource, Handle->CPUHandle)

						IMPLETEMENT_ALLOCATE();

#undef ALLOCATE_INTERNAL
					}

					INLINE bool AllocateDepthStencilView(ID3D12Resource1* Resource, DXGI_FORMAT Format, D3D12_DSV_FLAGS Flags, ViewHandle* Handle)
					{
						Assert(Resource != nullptr, "Resource cannot be null");

#define ALLOCATE_INTERNAL() DirectX12Wrapper::Resource::CreateDepthStencilView(m_Device, Resource, Format, Flags, Handle->CPUHandle)

						IMPLETEMENT_ALLOCATE();

#undef ALLOCATE_INTERNAL
					}

					INLINE bool DeallocateView(const ViewHandle& Handle)
					{
						for (uint8 i = 0; i < m_DescriptInfoCount; ++i)
						{
							DescriptorInfo& info = m_DescriptorInfos[i];

							if (Handle.DescriptorHeap != info.DescriptorHeap)
								continue;

							uint32 index = (Handle.CPUHandle.ptr - info.DescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr) / m_HandleIncrementSize;

							info.AllocatedStatus[index] = false;

							return true;
						}

						return false;
					}

				private:
					INLINE bool FindAFreeSlot(DescriptorInfo** Info, uint32* Index)
					{
						uint32 descriptorCount = GET_DESCRIPTOR_MAX_COUNT();

						for (uint8 c = m_DescriptInfoCount; c > 0; --c)
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

						if (!DirectX12Wrapper::Resource::CreateDescriptorHeap(m_Device, m_Type, descriptorCount, m_Flags, &info.DescriptorHeap))
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
					D3D12_DESCRIPTOR_HEAP_FLAGS m_Flags;
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