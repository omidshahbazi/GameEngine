// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef HEAP_ALLOCATOR_H
#define HEAP_ALLOCATOR_H

#include <Rendering\Private\DirectX12\DirectX12Wrapper.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace DirectX12
			{
				class HeapAllocator
				{
				public:
					HeapAllocator(void) :
						m_Device(nullptr),
						m_Heap(nullptr),
						m_BlockSize(0),
						m_BlockCount(0),
						m_BlockStates(nullptr)
					{
					}

					virtual ~HeapAllocator(void)
					{
						if (m_Heap == nullptr)
							return;

						Deinitialize();
					}

					INLINE bool Deinitialize(void)
					{
						Assert(m_Heap != nullptr, "Allocator already deinitialized");

						if (!DirectX12Wrapper::ReleaseInstance(m_Heap))
							return false;

						m_Heap = nullptr;
						m_BlockSize = 0;
						m_BlockCount = 0;

						RenderingAllocators::RenderingSystemAllocator_Deallocate(m_BlockStates);

						return true;
					}

					INLINE bool Deallocate(ID3D12Resource1* Resource)
					{
						//Assert(Resource != nullptr, "Resource cannot be null");

						//D3D12_GPU_VIRTUAL_ADDRESS address = Resource->GetGPUVirtualAddress();

						//Assert(m_BeginBufferAddress <= address && address < m_EndBufferAddress, "Resource doesn't belong to this allocator");

						//uint32 index = (address - m_BeginBufferAddress) / m_BlockSize;

						//D3D12_RESOURCE_DESC desc = Resource->GetDesc();
						//uint16 requiredBlockCount = Mathematics::Ceil(DirectX12Wrapper::GetRequiredBufferSize(m_Device, desc.Dimension, desc.Width, desc.Height, desc.Format, desc.Layout) / (float64)m_BlockSize);

						//for (uint32 j = 0; j < requiredBlockCount; ++j)
						//	m_BlockStates[index + j] = false;

						//return DirectX12Wrapper::ReleaseInstance(Resource);

						return true;
					}

					INLINE bool DoesOwn(ID3D12Resource1* Resource)
					{
						return true;
					}

					INLINE bool Reset(void)
					{
						PlatformMemory::Set(m_BlockStates, 0, m_BlockCount);

						return true;
					}

				protected:
					INLINE bool Initialize(ID3D12Device5* Device, uint32 BlockSize, uint32 BlockCount, bool IsCPUAccessible, D3D12_HEAP_FLAGS Flags)
					{
						Assert(m_Heap == nullptr, "Allocator already initialized");

						m_Device = Device;
						m_BlockSize = BlockSize;
						m_BlockCount = BlockCount;

						m_BlockStates = RenderingAllocators::RenderingSystemAllocator_AllocateArray<bool>(m_BlockCount);
						PlatformMemory::Set(m_BlockStates, 0, m_BlockCount);

						uint64 totalSize = m_BlockSize * m_BlockCount;

						return DirectX12Wrapper::Resource::CreateHeap(Device, totalSize, IsCPUAccessible, Flags, &m_Heap);
					}

					INLINE bool Allocate(D3D12_RESOURCE_DIMENSION Type, uint32 Width, uint32 Height, DXGI_FORMAT Format, D3D12_TEXTURE_LAYOUT Layout, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES State, ID3D12Resource1** Resource)
					{
						uint16 requiredBlockCount = Mathematics::Ceil(DirectX12Wrapper::Support::GetRequiredBufferSize(m_Device, Type, Width, Height, Format, Layout) / (float64)m_BlockSize);

						int32 index = -1;

						for (uint32 i = 0; i < m_BlockCount; ++i)
						{
							bool isFree = true;

							for (uint32 j = 0; j < requiredBlockCount; ++j)
							{
								if (!m_BlockStates[i + j])
									continue;

								i += j;

								isFree = false;
								break;
							}

							if (!isFree)
								continue;

							index = i;
							break;
						}

						if (index == -1)
							return false;

						for (uint32 j = 0; j < requiredBlockCount; ++j)
							m_BlockStates[index + j] = true;

						return DirectX12Wrapper::Resource::Create(m_Device, m_Heap, index * m_BlockSize, Type, m_BlockSize, Width, Height, Format, Layout, Flags, State, Resource);
					}

				private:
					ID3D12Device5* m_Device;
					ID3D12Heap1* m_Heap;
					uint64 m_BlockSize;
					uint32 m_BlockCount;
					bool* m_BlockStates;
				};

				class BufferHeapAllocator : public HeapAllocator
				{
				public:
					INLINE bool Initialize(ID3D12Device5* Device, uint32 BlockCount, bool IsCPUAccessible)
					{
						return HeapAllocator::Initialize(Device, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, BlockCount, IsCPUAccessible, D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
					}

					INLINE bool Allocate(uint32 Size, D3D12_RESOURCE_STATES State, ID3D12Resource1** Resource)
					{
						return HeapAllocator::Allocate(D3D12_RESOURCE_DIMENSION_BUFFER, Size, 1, DXGI_FORMAT_UNKNOWN, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE, State, Resource);
					}
				};

				class TextureHeapAllocator : public HeapAllocator
				{
				public:
					INLINE bool Initialize(ID3D12Device5* Device, uint32 BlockSize, uint32 BlockCount, bool IsCPUAccessible)
					{
						return HeapAllocator::Initialize(Device, BlockSize, BlockCount, IsCPUAccessible, D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES);
					}

					INLINE bool Allocate(uint32 Width, uint32 Height, DXGI_FORMAT Format, D3D12_RESOURCE_DIMENSION Dimension, D3D12_RESOURCE_STATES State, ID3D12Resource1** Resource)
					{
						return HeapAllocator::Allocate(Dimension, Width, Height, Format, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS, State, Resource);
					}
				};

				class RenderTargetHeapAllocator : public HeapAllocator
				{
				public:
					INLINE bool Initialize(ID3D12Device5* Device, uint32 BlockCount, bool IsCPUAccessible)
					{
						return HeapAllocator::Initialize(Device, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, BlockCount, IsCPUAccessible, D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);
					}

					INLINE bool Allocate(uint32 Width, uint32 Height, DXGI_FORMAT Format, bool IsColored, D3D12_RESOURCE_STATES State, ID3D12Resource1** Resource)
					{
						return HeapAllocator::Allocate(D3D12_RESOURCE_DIMENSION_TEXTURE2D, Width, Height, Format, D3D12_TEXTURE_LAYOUT_UNKNOWN, (IsColored ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL), State, Resource);
					}
				};
			}
		}
	}
}

#endif