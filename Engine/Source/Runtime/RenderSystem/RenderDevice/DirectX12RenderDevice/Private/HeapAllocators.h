// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef HEAP_ALLOCATORS_H
#define HEAP_ALLOCATORS_H

#include <DirectX12RenderDevice\Private\HeapAllocator.h>

namespace Engine
{
	namespace DirectX12RenderDevice
	{
		namespace Private
		{
			class HeapAllocators
			{
			protected:
				static const uint8 RESERVED_HEAP_ALLOCATORS_SLOT_COUNT = 255;
				static const uint16 HEAP_ALLOCATOR_BLOCK_COUNT = 512;

			public:
				HeapAllocators(void) :
					m_IsInitialized(false),
					m_Device(nullptr),
					m_IsCPUAccessible(false),
					m_InitializedAllocatorCount(0)
				{
				}

				INLINE bool GetIsCPUAccessible(void) const
				{
					return m_IsCPUAccessible;
				}

			protected:
				INLINE bool Initialize(ID3D12Device5* Device, bool IsCPUAccessible)
				{
					CoreDebugAssert(Categories::RenderSystem, !m_IsInitialized, "MemoryManager is already initialized");

					m_Device = Device;
					m_IsCPUAccessible = IsCPUAccessible;
					m_InitializedAllocatorCount = 0;

					m_IsInitialized = true;

					return true;
				}

				template<typename T>
				INLINE bool Deinitialize(T* Allocators)
				{
					if (!m_IsInitialized)
						return true;

					for (uint8 i = 0; i < m_InitializedAllocatorCount; ++i)
					{
						if (!Allocators[i].Deinitialize())
							return false;
					}

					m_InitializedAllocatorCount = 0;

					m_IsInitialized = false;

					return true;
				}

				template<typename T>
				INLINE bool Reset(T* Allocators)
				{
					for (uint8 i = 0; i < m_InitializedAllocatorCount; ++i)
					{
						if (!Allocators[i].Reset())
							return false;
					}

					return true;
				}

			protected:
				bool m_IsInitialized;
				ID3D12Device5* m_Device;
				bool m_IsCPUAccessible;
				uint8 m_InitializedAllocatorCount;
			};

			class BufferHeapAllocators : public HeapAllocators
			{
			public:
				INLINE bool Initialize(ID3D12Device5* Device, bool IsCPUAccessible)
				{
					return HeapAllocators::Initialize(Device, IsCPUAccessible);
				}

				INLINE bool Deinitialize(void)
				{
					return HeapAllocators::Deinitialize(m_HeapAllocators);
				}

				INLINE bool Reset(void)
				{
					return HeapAllocators::Reset(m_HeapAllocators);
				}

				INLINE bool Allocate(uint64 Size, D3D12_RESOURCE_STATES State, HeapAllocator::ResourceHandle* Handle)
				{
					for (uint8 n = m_InitializedAllocatorCount; n > 0; --n)
					{
						BufferHeapAllocator& allocator = m_HeapAllocators[n - 1];
						if (allocator.Allocate(Size, State, Handle))
							return true;
					}

					if (!InitializeNewHeapAllocator())
						return false;

					return m_HeapAllocators[m_InitializedAllocatorCount - 1].Allocate(Size, State, Handle);
				}

				INLINE bool Deallocate(const HeapAllocator::ResourceHandle& Handle)
				{
					for (uint8 i = 0; i < m_InitializedAllocatorCount; ++i)
					{
						BufferHeapAllocator& allocator = m_HeapAllocators[i];

						if (!allocator.DidAllocate(Handle))
							continue;

						return allocator.Deallocate(Handle);
					}

					return false;
				}

			private:
				INLINE bool InitializeNewHeapAllocator(void)
				{
					CoreDebugAssert(Categories::RenderSystem, m_InitializedAllocatorCount < RESERVED_HEAP_ALLOCATORS_SLOT_COUNT, "No more BufferHeapAllocator available");

					BufferHeapAllocator& heapAllocator = m_HeapAllocators[m_InitializedAllocatorCount];

					if (!heapAllocator.Initialize(m_Device, HEAP_ALLOCATOR_BLOCK_COUNT, m_IsCPUAccessible))
						return false;

					++m_InitializedAllocatorCount;

					return true;
				}

			private:
				BufferHeapAllocator m_HeapAllocators[RESERVED_HEAP_ALLOCATORS_SLOT_COUNT];
			};

			class TextureHeapAllocators : public HeapAllocators
			{
			public:
				TextureHeapAllocators(void) :
					m_Alignment(0)
				{
				}

				INLINE bool Initialize(ID3D12Device5* Device, uint32 Alignment, bool IsCPUAccessible)
				{
					m_Alignment = Alignment;

					return HeapAllocators::Initialize(Device, IsCPUAccessible);
				}

				INLINE bool Deinitialize(void)
				{
					return HeapAllocators::Deinitialize(m_HeapAllocators);
				}

				INLINE bool Reset(void)
				{
					return HeapAllocators::Reset(m_HeapAllocators);
				}

				INLINE bool Allocate(uint32 Width, uint32 Height, DXGI_FORMAT Format, D3D12_RESOURCE_DIMENSION Dimension, D3D12_RESOURCE_STATES State, HeapAllocator::ResourceHandle* Handle)
				{
					for (uint8 n = m_InitializedAllocatorCount; n > 0; --n)
					{
						TextureHeapAllocator& allocator = m_HeapAllocators[n - 1];
						if (allocator.Allocate(Width, Height, Format, Dimension, State, Handle))
							return true;
					}

					if (!InitializeNewHeapAllocator())
						return false;

					return m_HeapAllocators[m_InitializedAllocatorCount - 1].Allocate(Width, Height, Format, Dimension, State, Handle);
				}

				INLINE bool Deallocate(const HeapAllocator::ResourceHandle& Handle)
				{
					for (uint8 i = 0; i < m_InitializedAllocatorCount; ++i)
					{
						TextureHeapAllocator& allocator = m_HeapAllocators[i];

						if (!allocator.DidAllocate(Handle))
							continue;

						return allocator.Deallocate(Handle);
					}

					return false;
				}

				INLINE uint64 GetAlignment(void) const
				{
					return m_Alignment;
				}

			private:
				INLINE bool InitializeNewHeapAllocator(void)
				{
					CoreDebugAssert(Categories::RenderSystem, m_InitializedAllocatorCount < RESERVED_HEAP_ALLOCATORS_SLOT_COUNT, "No more TextureHeapAllocator available");

					TextureHeapAllocator& heapAllocator = m_HeapAllocators[m_InitializedAllocatorCount];

					if (!heapAllocator.Initialize(m_Device, m_Alignment, HEAP_ALLOCATOR_BLOCK_COUNT, m_IsCPUAccessible))
						return false;

					++m_InitializedAllocatorCount;

					return true;
				}

			private:
				uint64 m_Alignment;
				TextureHeapAllocator m_HeapAllocators[RESERVED_HEAP_ALLOCATORS_SLOT_COUNT];
			};

			class RenderTargetHeapAllocators : public HeapAllocators
			{
			public:
				INLINE bool Initialize(ID3D12Device5* Device, bool IsCPUAccessible)
				{
					return HeapAllocators::Initialize(Device, IsCPUAccessible);
				}

				INLINE bool Deinitialize(void)
				{
					return HeapAllocators::Deinitialize(m_HeapAllocators);
				}

				INLINE bool Reset(void)
				{
					return HeapAllocators::Reset(m_HeapAllocators);
				}

				INLINE bool Allocate(uint32 Width, uint32 Height, DXGI_FORMAT Format, bool IsColored, D3D12_RESOURCE_STATES State, HeapAllocator::ResourceHandle* Handle)
				{
					for (uint8 n = m_InitializedAllocatorCount; n > 0; --n)
					{
						RenderTargetHeapAllocator& allocator = m_HeapAllocators[n - 1];
						if (allocator.Allocate(Width, Height, Format, IsColored, State, Handle))
							return true;
					}

					if (!InitializeNewHeapAllocator())
						return false;

					return m_HeapAllocators[m_InitializedAllocatorCount - 1].Allocate(Width, Height, Format, IsColored, State, Handle);
				}

				INLINE bool Deallocate(const HeapAllocator::ResourceHandle& Handle)
				{
					for (uint8 i = 0; i < m_InitializedAllocatorCount; ++i)
					{
						RenderTargetHeapAllocator& allocator = m_HeapAllocators[i];

						if (!allocator.DidAllocate(Handle))
							continue;

						return allocator.Deallocate(Handle);
					}

					return false;
				}

			private:
				INLINE bool InitializeNewHeapAllocator()
				{
					CoreDebugAssert(Categories::RenderSystem, m_InitializedAllocatorCount < RESERVED_HEAP_ALLOCATORS_SLOT_COUNT, "No more RenderTargetHeapAllocator available");

					RenderTargetHeapAllocator& heapAllocator = m_HeapAllocators[m_InitializedAllocatorCount];

					if (!heapAllocator.Initialize(m_Device, HEAP_ALLOCATOR_BLOCK_COUNT, m_IsCPUAccessible))
						return false;

					++m_InitializedAllocatorCount;

					return true;
				}

			private:
				RenderTargetHeapAllocator m_HeapAllocators[RESERVED_HEAP_ALLOCATORS_SLOT_COUNT];
			};
		}
	}
}

#endif