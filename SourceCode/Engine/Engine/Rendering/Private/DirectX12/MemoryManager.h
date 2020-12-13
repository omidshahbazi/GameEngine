// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <Rendering\Private\DirectX12\HeapAllocator.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace DirectX12
			{
#define IMPLEMENT_ALLOCATE(Infos, InfoCount) \
				AllocatorsInfo* info = FindBestFitAllocatorsInfo(Infos, InfoCount, Size, IsCPUAccessible); \
				Assert(info != nullptr, "Couldn't find best fit AllocatorsInfo"); \
				for (uint8 n = info->InitializedAllocatorCount; n > 0; --n) \
				{ \
					HeapAllocator& allocator = info->HeapAllocators[n - 1]; \
					if (allocator.IMPLEMENT_INTERNAL_ALLOCATE()) \
						return true; \
				} \
				if (!InitializeNewHeapAllocator(*info)) \
					return false; \
				return info->HeapAllocators[info->InitializedAllocatorCount - 1].IMPLEMENT_INTERNAL_ALLOCATE();

#define IMPLEMENT_DEALLOCATE(Infos, InfoCount) \
				HeapAllocator* allocator = nullptr; \
				for (uint8 i = 0; i < InfoCount; ++i) \
				{ \
					AllocatorsInfo& info = Infos[i]; \
					for (uint8 j = 0; j < info.InitializedAllocatorCount; ++j) \
					{ \
						if (!info.HeapAllocators[j].DoesOwn(Resource)) \
							continue; \
						allocator = &info.HeapAllocators[j]; \
						break; \
					} \
					if (allocator != nullptr) \
						continue; \
				} \
				Assert(allocator != nullptr, "This resource doesn't allocated by MemoryManager"); \
				return allocator->Deallocate(Resource);

				const uint8 BUFFER_ALLOCATOR_INFO_COUNT = 4;
				const uint8 TEXTURE_ALLOCATOR_INFO_COUNT = 4;
				const uint8 RENDER_TARGET_ALLOCATOR_INFO_COUNT = 2;
				const uint8 RESERVED_HEAP_ALLOCATORS_SLOT_COUNT = 255;
				const uint16 HEAP_ALLOCATOR_BLOCK_COUNT = 1000;

				class MemoryManager
				{
				private:
					struct AllocatorsInfo
					{
					public:
						uint64 Alignment;
						bool IsCPUAccessible;
						D3D12_HEAP_FLAGS Flags;
						HeapAllocator HeapAllocators[RESERVED_HEAP_ALLOCATORS_SLOT_COUNT];
						uint8 InitializedAllocatorCount;
					};

				public:
					MemoryManager(void) :
						m_IsInitialized(false),
						m_Device(nullptr)
					{
						PlatformMemory::Set(m_BufferInfos, 0, BUFFER_ALLOCATOR_INFO_COUNT);
						PlatformMemory::Set(m_TextureInfos, 0, TEXTURE_ALLOCATOR_INFO_COUNT);
						PlatformMemory::Set(m_RenderTargetInfos, 0, RENDER_TARGET_ALLOCATOR_INFO_COUNT);
					}

					~MemoryManager(void)
					{
						if (!m_IsInitialized)
							return;

						Deinitialize();
					}

					INLINE bool Initialize(ID3D12Device5* Device)
					{
						Assert(!m_IsInitialized, "MemoryManager is already initialized");

						m_Device = Device;

						// Buffer
						{
							D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS;

							m_BufferInfos[0].Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
							m_BufferInfos[0].IsCPUAccessible = false;
							m_BufferInfos[0].Flags = flags;

							m_BufferInfos[1].Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
							m_BufferInfos[1].IsCPUAccessible = true;
							m_BufferInfos[1].Flags = flags;
						}

						// Texture
						{
							D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES;

							m_TextureInfos[0].Alignment = D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT;
							m_TextureInfos[0].IsCPUAccessible = false;
							m_TextureInfos[0].Flags = flags;

							m_TextureInfos[1].Alignment = D3D12_SMALL_RESOURCE_PLACEMENT_ALIGNMENT;
							m_TextureInfos[1].IsCPUAccessible = true;
							m_TextureInfos[1].Flags = flags;

							m_TextureInfos[2].Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
							m_TextureInfos[2].IsCPUAccessible = false;
							m_TextureInfos[2].Flags = flags;

							m_TextureInfos[3].Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
							m_TextureInfos[3].IsCPUAccessible = true;
							m_TextureInfos[3].Flags = flags;
						}

						// Render Target
						{
							D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES;

							m_RenderTargetInfos[0].Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
							m_RenderTargetInfos[0].IsCPUAccessible = false;
							m_RenderTargetInfos[0].Flags = flags;

							m_RenderTargetInfos[1].Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
							m_RenderTargetInfos[1].IsCPUAccessible = true;
							m_RenderTargetInfos[1].Flags = flags;
						}

						return true;
					}

					INLINE bool Deinitialize(void)
					{
#define IMPLEMENT_DEINITIALIZE(Infos, InfoCount) \
						for (uint8 i = 0; i < InfoCount; ++i) \
						{ \
							AllocatorsInfo& info = Infos[i]; \
							for (uint8 j = 0; j < info.InitializedAllocatorCount; ++j) \
							{ \
								if (!info.HeapAllocators[j].Deinitialize()) \
									return false; \
							} \
						}

						Assert(m_IsInitialized, "MemoryManager is not initialized");

						IMPLEMENT_DEINITIALIZE(m_BufferInfos, BUFFER_ALLOCATOR_INFO_COUNT);
						IMPLEMENT_DEINITIALIZE(m_TextureInfos, TEXTURE_ALLOCATOR_INFO_COUNT);
						IMPLEMENT_DEINITIALIZE(m_RenderTargetInfos, RENDER_TARGET_ALLOCATOR_INFO_COUNT);

						m_Device = nullptr;

						PlatformMemory::Set(m_BufferInfos, 0, BUFFER_ALLOCATOR_INFO_COUNT);
						PlatformMemory::Set(m_TextureInfos, 0, TEXTURE_ALLOCATOR_INFO_COUNT);
						PlatformMemory::Set(m_RenderTargetInfos, 0, RENDER_TARGET_ALLOCATOR_INFO_COUNT);

						return true;

#undef IMPLEMENT_DEINITIALIZE;
					}

					INLINE bool AllocateBuffer(uint64 Size, D3D12_RESOURCE_STATES State, bool IsCPUAccessible, ID3D12Resource1** Resource)
					{
#define IMPLEMENT_INTERNAL_ALLOCATE() Allocate(D3D12_RESOURCE_DIMENSION_BUFFER, Size, 1, DXGI_FORMAT_UNKNOWN, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE, State, Resource)

						IMPLEMENT_ALLOCATE(m_BufferInfos, BUFFER_ALLOCATOR_INFO_COUNT);

#undef IMPLEMENT_INTERNAL_ALLOCATE
					}

					INLINE bool DeallocateBuffer(ID3D12Resource1* Resource)
					{
						IMPLEMENT_DEALLOCATE(m_BufferInfos, BUFFER_ALLOCATOR_INFO_COUNT);
					}

					INLINE bool AllocateTexture2D(uint32 Width, uint32 Height, DXGI_FORMAT Format, D3D12_RESOURCE_STATES State, bool IsCPUAccessible, ID3D12Resource1** Resource)
					{
						const uint32 Size = DirectX12Wrapper::GetRequiredBufferSize(m_Device, D3D12_RESOURCE_DIMENSION_TEXTURE2D, Width, Height, Format, D3D12_TEXTURE_LAYOUT_UNKNOWN);

#define IMPLEMENT_INTERNAL_ALLOCATE() Allocate(D3D12_RESOURCE_DIMENSION_TEXTURE2D, Width, Height, Format, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_NONE, State, Resource)

						IMPLEMENT_ALLOCATE(m_TextureInfos, TEXTURE_ALLOCATOR_INFO_COUNT);

#undef IMPLEMENT_INTERNAL_ALLOCATE
					}

					INLINE bool DeallocateTexture2D(ID3D12Resource1* Resource)
					{
						IMPLEMENT_DEALLOCATE(m_TextureInfos, TEXTURE_ALLOCATOR_INFO_COUNT);
					}

					INLINE bool AllocateRenderTarget(uint32 Width, uint32 Height, DXGI_FORMAT Format, bool IsColored, D3D12_RESOURCE_STATES State, bool IsCPUAccessible, ID3D12Resource1** Resource)
					{
						const uint32 Size = DirectX12Wrapper::GetRequiredBufferSize(m_Device, D3D12_RESOURCE_DIMENSION_TEXTURE2D, Width, Height, Format, D3D12_TEXTURE_LAYOUT_UNKNOWN);

#define IMPLEMENT_INTERNAL_ALLOCATE() Allocate(D3D12_RESOURCE_DIMENSION_TEXTURE2D, Width, Height, Format, D3D12_TEXTURE_LAYOUT_UNKNOWN, (IsColored ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL), State, Resource)

						IMPLEMENT_ALLOCATE(m_RenderTargetInfos, RENDER_TARGET_ALLOCATOR_INFO_COUNT);

#undef IMPLEMENT_INTERNAL_ALLOCATE
					}

					INLINE bool DeallocateRenderTarget(ID3D12Resource1* Resource)
					{
						IMPLEMENT_DEALLOCATE(m_RenderTargetInfos, RENDER_TARGET_ALLOCATOR_INFO_COUNT);
					}

				private:
					INLINE bool InitializeNewHeapAllocator(AllocatorsInfo& Info)
					{
						Assert(Info.InitializedAllocatorCount < RESERVED_HEAP_ALLOCATORS_SLOT_COUNT, "No more HeapAllocator available");

						HeapAllocator& heapAllocator = Info.HeapAllocators[Info.InitializedAllocatorCount];

						if (!heapAllocator.Initialize(m_Device, Info.Alignment, HEAP_ALLOCATOR_BLOCK_COUNT, Info.IsCPUAccessible, Info.Flags))
							return false;

						++Info.InitializedAllocatorCount;

						return true;
					}

					INLINE static AllocatorsInfo* FindBestFitAllocatorsInfo(AllocatorsInfo* Infos, uint8 InfoCount, uint64 Size, bool IsCPUAccessible)
					{
						AllocatorsInfo* maxAlignmentInfo = nullptr;

						for (uint8 i = 0; i < InfoCount; ++i)
						{
							AllocatorsInfo& info = Infos[i];

							if (info.IsCPUAccessible != IsCPUAccessible)
								continue;

							if (maxAlignmentInfo == nullptr || maxAlignmentInfo->Alignment < info.Alignment)
								maxAlignmentInfo = &info;

							if (info.Alignment < Size)
								continue;

							return &info;
						}

						return maxAlignmentInfo;
					}

				private:
					bool m_IsInitialized;
					ID3D12Device5* m_Device;
					AllocatorsInfo m_BufferInfos[BUFFER_ALLOCATOR_INFO_COUNT];
					AllocatorsInfo m_TextureInfos[TEXTURE_ALLOCATOR_INFO_COUNT];
					AllocatorsInfo m_RenderTargetInfos[RENDER_TARGET_ALLOCATOR_INFO_COUNT];
				};

#undef IMPLEMENT_ALLOCATE
			}
		}
	}
}

#endif