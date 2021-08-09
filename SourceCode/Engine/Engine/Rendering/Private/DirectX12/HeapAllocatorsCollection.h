// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef HEAP_ALLOCATORS_COLLECTION_H
#define HEAP_ALLOCATORS_COLLECTION_H

#include <Rendering\Private\DirectX12\HeapAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace DirectX12
			{
				class BufferHeapAllocatorsCollection
				{
				protected:
					static const uint8 ALLOCATOR_COUNT = 2;

				public:
					~BufferHeapAllocatorsCollection(void)
					{
						Deinitialize();
					}

					INLINE bool Initialize(ID3D12Device5* Device)
					{
						m_Allocators[0].Initialize(Device, false);
						m_Allocators[1].Initialize(Device, true);

						return true;
					}

					INLINE bool Deinitialize(void)
					{
						for (uint8 i = 0; i < ALLOCATOR_COUNT; ++i)
						{
							if (!m_Allocators[i].Deinitialize())
								return false;
						}

						return true;
					}

					INLINE bool Allocate(uint64 Size, D3D12_RESOURCE_STATES State, bool IsCPUAccessible, HeapAllocator::ResourceHandle* Handle)
					{
						BufferHeapAllocators* allocators = FindBestFitAllocators(Size, IsCPUAccessible);
						Assert(allocators != nullptr, "Couldn't find best fit HeapAllocators");

						return allocators->Allocate(Size, State, Handle);
					}

					INLINE bool Deallocate(const HeapAllocator::ResourceHandle& Handle)
					{
						for (uint8 i = 0; i < ALLOCATOR_COUNT; ++i)
						{
							BufferHeapAllocators& allocators = m_Allocators[i];

							if (allocators.Deallocate(Handle))
								return true;
						}

						Assert(false, "This resource doesn't allocated by this HeapAllocatorsCollection");

						return false;
					}

				private:
					INLINE BufferHeapAllocators* FindBestFitAllocators(uint64 Size, bool IsCPUAccessible)
					{
						for (uint8 i = 0; i < ALLOCATOR_COUNT; ++i)
						{
							BufferHeapAllocators& allocators = m_Allocators[i];

							if (allocators.GetIsCPUAccessible() != IsCPUAccessible)
								continue;

							return &allocators;
						}

						return nullptr;
					}

				private:
					BufferHeapAllocators m_Allocators[ALLOCATOR_COUNT];
				};

				class TextureHeapAllocatorsCollection
				{
				protected:
					static const uint8 ALLOCATOR_COUNT = 4;

				public:
					~TextureHeapAllocatorsCollection(void)
					{
						Deinitialize();
					}

					INLINE bool Initialize(ID3D12Device5* Device)
					{
						m_Device = Device;
						m_Allocators[0].Initialize(m_Device, D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT, false);
						m_Allocators[1].Initialize(m_Device, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, false);
						m_Allocators[2].Initialize(m_Device, D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT, true);
						m_Allocators[3].Initialize(m_Device, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, true);

						return true;
					}

					INLINE bool Deinitialize(void)
					{
						m_Device = nullptr;

						for (uint8 i = 0; i < ALLOCATOR_COUNT; ++i)
						{
							if (!m_Allocators[i].Deinitialize())
								return false;
						}

						return true;
					}

					INLINE bool Allocate(uint32 Width, uint32 Height, DXGI_FORMAT Format, D3D12_RESOURCE_DIMENSION Dimension, D3D12_RESOURCE_STATES State, bool IsCPUAccessible, HeapAllocator::ResourceHandle* Handle)
					{
						const uint32 Size = DirectX12Wrapper::Support::GetRequiredBufferSize(m_Device, D3D12_RESOURCE_DIMENSION_TEXTURE2D, Width, Height, Format, D3D12_TEXTURE_LAYOUT_UNKNOWN);

						TextureHeapAllocators* allocators = FindBestFitAllocators(Size, IsCPUAccessible);
						Assert(allocators != nullptr, "Couldn't find best fit HeapAllocators");

						return allocators->Allocate(Width, Height, Format, Dimension, State, Handle);
					}

					INLINE bool Deallocate(const HeapAllocator::ResourceHandle& Handle)
					{
						for (uint8 i = 0; i < ALLOCATOR_COUNT; ++i)
						{
							TextureHeapAllocators& allocators = m_Allocators[i];

							if (allocators.Deallocate(Handle))
								return true;
						}

						Assert(false, "This resource doesn't allocated by this HeapAllocatorsCollection");

						return false;
					}

				private:
					INLINE TextureHeapAllocators* FindBestFitAllocators(uint64 Size, bool IsCPUAccessible)
					{
						TextureHeapAllocators* maxAlignmentAllocators = nullptr;

						for (uint8 i = 0; i < ALLOCATOR_COUNT; ++i)
						{
							TextureHeapAllocators& allocators = m_Allocators[i];

							if (allocators.GetIsCPUAccessible() != IsCPUAccessible)
								continue;

							if (maxAlignmentAllocators == nullptr || maxAlignmentAllocators->GetAlignment() < allocators.GetAlignment())
								maxAlignmentAllocators = &allocators;

							if (allocators.GetAlignment() < Size)
								continue;

							return &allocators;
						}

						return maxAlignmentAllocators;
					}

				private:
					ID3D12Device5* m_Device;
					TextureHeapAllocators m_Allocators[ALLOCATOR_COUNT];
				};

				class RenderTargetHeapAllocatorsCollection
				{
				protected:
					static const uint8 ALLOCATOR_COUNT = 2;

				public:
					~RenderTargetHeapAllocatorsCollection(void)
					{
						Deinitialize();
					}

					INLINE bool Initialize(ID3D12Device5* Device)
					{
						m_Device = Device;

						m_Allocators[0].Initialize(m_Device, false);
						m_Allocators[1].Initialize(m_Device, true);

						return true;
					}

					INLINE bool Deinitialize(void)
					{
						m_Device = nullptr;

						for (uint8 i = 0; i < ALLOCATOR_COUNT; ++i)
						{
							if (!m_Allocators[i].Deinitialize())
								return false;
						}

						return true;
					}

					INLINE bool Allocate(uint32 Width, uint32 Height, DXGI_FORMAT Format, bool IsColored, D3D12_RESOURCE_STATES State, bool IsCPUAccessible, HeapAllocator::ResourceHandle* Handle)
					{
						const uint32 Size = DirectX12Wrapper::Support::GetRequiredBufferSize(m_Device, D3D12_RESOURCE_DIMENSION_TEXTURE2D, Width, Height, Format, D3D12_TEXTURE_LAYOUT_UNKNOWN);

						RenderTargetHeapAllocators* allocators = FindBestFitAllocators(Size, IsCPUAccessible);
						Assert(allocators != nullptr, "Couldn't find best fit HeapAllocators");

						return allocators->Allocate(Width, Height, Format, IsColored, State, Handle);
					}

					INLINE bool Deallocate(const HeapAllocator::ResourceHandle& Handle)
					{
						for (uint8 i = 0; i < ALLOCATOR_COUNT; ++i)
						{
							RenderTargetHeapAllocators& allocators = m_Allocators[i];

							if (allocators.Deallocate(Handle))
								return true;
						}

						Assert(false, "This resource doesn't allocated by this HeapAllocatorsCollection");

						return false;
					}

					INLINE RenderTargetHeapAllocators* FindBestFitAllocators(uint64 Size, bool IsCPUAccessible)
					{
						for (uint8 i = 0; i < ALLOCATOR_COUNT; ++i)
						{
							RenderTargetHeapAllocators& allocators = m_Allocators[i];

							if (allocators.GetIsCPUAccessible() != IsCPUAccessible)
								continue;

							return &allocators;
						}

						return nullptr;
					}

				private:
					ID3D12Device5* m_Device;
					RenderTargetHeapAllocators m_Allocators[ALLOCATOR_COUNT];
				};
			}
		}
	}
}

#endif