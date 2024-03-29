// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DIRECTX12_WRAPPER_H
#define DIRECTX12_WRAPPER_H

#include <Common\PrimitiveTypes.h>
#include <Common\BitwiseUtils.h>
#include <Common\CharacterUtility.h>
#include <Common\TypeTraits.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <Allocators\FrameAllocator.h>
#include <Platform\PlatformWindow.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <d3d12.h>
#include <d3dcompiler.h>

namespace Engine
{
	using namespace Allocators;
	using namespace Platform;
	using namespace RenderCommon::Private;

	namespace DirectX12RenderDevice
	{
		namespace Private
		{
			//https://microsoft.github.io/DirectX-Specs/d3d/CPUEfficiency.html
			class DirectX12Wrapper
			{
			public:
				class Initialization
				{
				public:
					INLINE static bool CreateFactory(bool DebugMode, IDXGIFactory5** Factory)
					{
						uint32 flags = 0;

						if (DebugMode)
							flags = DXGI_CREATE_FACTORY_DEBUG;

						return SUCCEEDED(CreateDXGIFactory2(flags, IID_PPV_ARGS(Factory)));
					}

					INLINE static bool FindBestAdapter(IDXGIFactory5* Factory, IDXGIAdapter3** Adapter, DXGI_ADAPTER_DESC2* Desc)
					{
						IDXGIAdapter1* tempAdapter = nullptr;
						uint64 maxMemory = 0;

						for (uint8 i = 0; SUCCEEDED(Factory->EnumAdapters1(i, &tempAdapter)); ++i)
						{
							DXGI_ADAPTER_DESC1 desc;
							tempAdapter->GetDesc1(&desc);

							if (BitwiseUtils::IsEnabled(desc.Flags, (uint32)DXGI_ADAPTER_FLAG_SOFTWARE))
								continue;

							if (!SUCCEEDED(D3D12CreateDevice(tempAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device5), nullptr)))
								continue;

							if (desc.DedicatedVideoMemory < maxMemory)
								continue;

							maxMemory = desc.DedicatedVideoMemory;
							tempAdapter->QueryInterface<IDXGIAdapter3>(Adapter);

							ReleaseInstance(tempAdapter);
						}

						if (Adapter == nullptr)
							return false;

						(*Adapter)->GetDesc2(Desc);

						return true;
					}

					INLINE static bool CreateDevice(IDXGIAdapter3* Adapter, ID3D12Device5** Device)
					{
						return SUCCEEDED(D3D12CreateDevice(Adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(Device)));
					}
				};

				class Debugging
				{
				public:
					INLINE static bool GetDebugLayer(ID3D12Debug3** DebugLayer)
					{
						ID3D12Debug* debug = nullptr;
						if (!SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
							return false;

						if (!SUCCEEDED(debug->QueryInterface(DebugLayer)))
							return false;

						ReleaseInstance(debug);

						return true;
					}

					INLINE static bool EnableDebugLayer(void)
					{
						ID3D12Debug3* debug = nullptr;
						if (!GetDebugLayer(&debug))
							return false;

						debug->EnableDebugLayer();

						return ReleaseInstance(debug);
					}

					INLINE static bool EnableValidationLayer(void)
					{
						ID3D12Debug3* debug = nullptr;
						if (!GetDebugLayer(&debug))
							return false;

						debug->SetEnableGPUBasedValidation(true);

						return ReleaseInstance(debug);
					}

					INLINE static bool ReportLiveObjects(void)
					{
						IDXGIDebug1* debug = nullptr;
						if (!SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug))))
							return false;

						if (!SUCCEEDED(debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_IGNORE_INTERNAL)))
							return false;

						return ReleaseInstance(debug);
					}

					INLINE static bool GetInfoQueue(ID3D12Device5* Device, ID3D12InfoQueue** InfoQueue)
					{
						return SUCCEEDED(Device->QueryInterface<ID3D12InfoQueue>(InfoQueue));
					}

					INLINE static bool GetDebugCommandList(ID3D12GraphicsCommandList4* CommandList, ID3D12DebugCommandList2** DebugCommandList)
					{
						return SUCCEEDED(CommandList->QueryInterface(DebugCommandList));
					}

					INLINE static bool IterateOverDebugMessages(ID3D12InfoQueue* InfoQueue, std::function<void(D3D12_MESSAGE_ID, D3D12_MESSAGE_CATEGORY, cstr, D3D12_MESSAGE_SEVERITY)> Callback)
					{
						uint64 count = InfoQueue->GetNumStoredMessages();
						if (count == 0)
							return false;

						FrameAllocator allocator("Debug Message Allocator", RenderSystemAllocators::RenderSystemAllocator, MegaByte);
						for (uint64 i = 0; i < count; ++i)
						{
							SIZE_T size = 0;
							InfoQueue->GetMessage(i, nullptr, &size);

							D3D12_MESSAGE* message = ReinterpretCast(D3D12_MESSAGE*, AllocateMemory(&allocator, size));

							InfoQueue->GetMessage(i, message, &size);

							Callback(message->ID, message->Category, message->pDescription, message->Severity);
						}

						allocator.Reset();

						InfoQueue->ClearStoredMessages();

						return true;
					}

					INLINE static bool SetObjectName(ID3D12Object* Object, cwstr Name)
					{
						return SUCCEEDED(Object->SetName(Name));
					}
				};

				class Support
				{
				public:
					INLINE static bool CheckTearing(IDXGIFactory5* Factory)
					{
						BOOL result = false;
						if (SUCCEEDED(Factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &result, sizeof(result))))
							return result;

						return false;
					}

					INLINE static uint32 GetResourceRowPitch(ID3D12Device5* Device, ID3D12Resource1* Resource)
					{
						D3D12_RESOURCE_DESC desc = Resource->GetDesc();

						D3D12_PLACED_SUBRESOURCE_FOOTPRINT footPrint;
						Device->GetCopyableFootprints(&desc, 0, 1, 0, &footPrint, nullptr, nullptr, nullptr);

						return footPrint.Footprint.RowPitch;
					}

					INLINE static bool GetCopyableFootprint(ID3D12Device5* Device, ID3D12Resource1* Resource, D3D12_PLACED_SUBRESOURCE_FOOTPRINT* PlacedFootprint)
					{
						D3D12_RESOURCE_DESC desc = Resource->GetDesc();

						Device->GetCopyableFootprints(&desc, 0, 1, 0, PlacedFootprint, nullptr, nullptr, nullptr);

						return true;
					}

					INLINE static uint64 GetRequiredBufferSize(ID3D12Device5* Device, D3D12_RESOURCE_DIMENSION Type, uint32 Width, uint32 Height, DXGI_FORMAT Format, D3D12_TEXTURE_LAYOUT Layout)
					{
						D3D12_RESOURCE_DESC desc = {};
						desc.Dimension = Type;
						desc.Width = Width;
						desc.Height = Height;
						desc.DepthOrArraySize = 1;
						desc.MipLevels = 1;
						desc.Format = Format;

						desc.SampleDesc.Quality = 0;
						desc.SampleDesc.Count = 1;

						desc.Layout = Layout;
						desc.Flags = D3D12_RESOURCE_FLAG_NONE;

						uint64 result = 0;

						Device->GetCopyableFootprints(&desc, 0, 1, 0, nullptr, nullptr, nullptr, &result);

						return result;
					}
				};

				class SwapChain
				{
				public:
					INLINE static bool Create(IDXGIFactory5* Factory, ID3D12CommandQueue* CommandQueue, DXGI_FORMAT Format, uint8 BackBufferCount, PlatformWindow::WindowHandle Handle, IDXGISwapChain4** SwapChain)
					{
						DXGI_SWAP_CHAIN_DESC1 desc = {};
						desc.Width = 0;
						desc.Height = 0;
						desc.Format = Format;
						desc.Stereo = false;

						desc.SampleDesc.Quality = 0;
						desc.SampleDesc.Count = 1;

						desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
						desc.BufferCount = BackBufferCount;
						desc.Scaling = DXGI_SCALING_STRETCH;
						desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
						desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

						desc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
						desc.Flags |= Support::CheckTearing(Factory) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

						IDXGISwapChain1* swapChain = nullptr;
						if (!SUCCEEDED(Factory->CreateSwapChainForHwnd(CommandQueue, (HWND)Handle, &desc, nullptr, nullptr, &swapChain)))
							return false;

						if (!SUCCEEDED(Factory->MakeWindowAssociation((HWND)Handle, DXGI_MWA_NO_ALT_ENTER)))
							return false;

						if (!SUCCEEDED(swapChain->QueryInterface<IDXGISwapChain4>(SwapChain)))
							return false;

						ReleaseInstance(swapChain);

						return true;
					}

					INLINE static bool Resize(IDXGISwapChain4* SwapChain, uint8 BackBufferCount, uint16 Width, uint16 Height)
					{
						DXGI_SWAP_CHAIN_DESC1 desc = {};
						if (!SUCCEEDED(SwapChain->GetDesc1(&desc)))
							return false;

						return SUCCEEDED(SwapChain->ResizeBuffers(BackBufferCount, Width, Height, desc.Format, desc.Flags));
					}

					INLINE static bool GetBuffers(IDXGISwapChain4* SwapChain, uint8 BackBufferCount, ID3D12Resource1** BackBuffers)
					{
						for (uint8 i = 0; i < BackBufferCount; ++i)
							if (!SUCCEEDED(SwapChain->GetBuffer(i, IID_PPV_ARGS(&BackBuffers[i]))))
								return false;

						return true;
					}

					INLINE static bool Present(IDXGISwapChain4* SwapChain, bool VSync = true, bool AllowTearing = false)
					{
						uint8 syncInterval = VSync ? 1 : 0;
						uint8 presentFlags = (AllowTearing && !VSync) ? DXGI_PRESENT_ALLOW_TEARING : 0;

						return SUCCEEDED(SwapChain->Present(syncInterval, presentFlags));
					}
				};

				class Shader
				{
				public:
					INLINE static bool GetInlineRootSignature(const byte* Data, uint16 Length, ID3DBlob** SerializedRootSignature)
					{
						return SUCCEEDED(D3DGetBlobPart(Data, Length, D3D_BLOB_ROOT_SIGNATURE, 0, SerializedRootSignature));
					}
				};

				class RootSignature
				{
				public:
					INLINE static bool Create(ID3D12Device5* Device, ID3D10Blob* Data, ID3D12RootSignature** RootSignature)
					{
						return SUCCEEDED(Device->CreateRootSignature(0, Data->GetBufferPointer(), Data->GetBufferSize(), IID_PPV_ARGS(RootSignature)));
					}
				};

				class PipelineStateObject
				{
				public:
					template <typename DataType, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE SubobjectType, typename DefaultValueType = DataType>
					class alignas(void*) PipelineStateSubobject
					{
					private:
						D3D12_PIPELINE_STATE_SUBOBJECT_TYPE m_Type;
						DataType m_Data;

					public:
						PipelineStateSubobject(void) :
							m_Type(SubobjectType),
							m_Data(DefaultValueType())
						{
						}

						PipelineStateSubobject(const DataType& Data) :
							m_Type(SubobjectType),
							m_Data(Data)
						{
						}

						PipelineStateSubobject& operator=(const DataType& Data)
						{
							m_Data = Data;

							return *this;
						}

						operator DataType& (void)
						{
							return m_Data;
						}

						operator const DataType& (void) const
						{
							return m_Data;
						}
					};

					typedef PipelineStateSubobject<ID3D12RootSignature*, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_ROOT_SIGNATURE> PipelineStateSubobjectRootSignature;
					typedef PipelineStateSubobject<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VS> PipelineStateSubobjectVertexShader;
					typedef PipelineStateSubobject<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS> PipelineStateSubobjectTessellationShader;
					typedef PipelineStateSubobject<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS> PipelineStateSubobjectGeometryShader;
					typedef PipelineStateSubobject<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS> PipelineStateSubobjectFragmentShader;
					typedef PipelineStateSubobject<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CS> PipelineStateSubobjectComputeShader;
					typedef PipelineStateSubobject<D3D12_STREAM_OUTPUT_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_STREAM_OUTPUT> PipelineStateSubobjectStreamOutput;
					typedef PipelineStateSubobject<D3D12_BLEND_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_BLEND, DefaultType> PipelineStateSubobjectBlendState;
					typedef PipelineStateSubobject<D3D12_RASTERIZER_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RASTERIZER, DefaultType> PipelineStateSubobjectRasterizerState;
					typedef PipelineStateSubobject<D3D12_DEPTH_STENCIL_DESC1, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL1, DefaultType> PipelineStateSubobjectDepthStencil;
					typedef PipelineStateSubobject<D3D12_INPUT_LAYOUT_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_INPUT_LAYOUT> PipelineStateSubobjectInputLayout;
					typedef PipelineStateSubobject<D3D12_INDEX_BUFFER_STRIP_CUT_VALUE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_IB_STRIP_CUT_VALUE> PipelineStateSubobjectIndexBufferStripCut;
					typedef PipelineStateSubobject<D3D12_PRIMITIVE_TOPOLOGY_TYPE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PRIMITIVE_TOPOLOGY> PipelineStateSubobjectPrimitiveToplogy;
					typedef PipelineStateSubobject<D3D12_RT_FORMAT_ARRAY, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_RENDER_TARGET_FORMATS> PipelineStateSubobjectRenderTargetFormats;
					typedef PipelineStateSubobject<DXGI_FORMAT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DEPTH_STENCIL_FORMAT> PipelineStateSubobjectDepthStencilFormat;
					typedef PipelineStateSubobject<DXGI_SAMPLE_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_DESC, DefaultType> PipelineStateSubobjectSampleState;
					typedef PipelineStateSubobject<UINT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_SAMPLE_MASK, DefaultType> PipelineStateSubobjectSampleMask;
					typedef PipelineStateSubobject<UINT, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_NODE_MASK> PipelineStateSubobjectNodeMask;
					typedef PipelineStateSubobject<D3D12_CACHED_PIPELINE_STATE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_CACHED_PSO> PipelineStateSubobjectCachedPipelineStateObject;
					typedef PipelineStateSubobject<D3D12_PIPELINE_STATE_FLAGS, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_FLAGS> PipelineStateSubobjectFlags;
					typedef PipelineStateSubobject<D3D12_VIEW_INSTANCING_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VIEW_INSTANCING, DefaultType> PipelineStateSubobjectViewInstancing;

					struct GraphicsPipelineStateDesc
					{
						PipelineStateSubobjectVertexShader VertexShader;
						PipelineStateSubobjectTessellationShader TessellationShader;
						PipelineStateSubobjectGeometryShader GeometryShader;
						PipelineStateSubobjectFragmentShader FragmentShader;

						PipelineStateSubobjectRasterizerState RasterizerState;
						PipelineStateSubobjectDepthStencil DepthStencil;
						PipelineStateSubobjectBlendState BlendState;

						PipelineStateSubobjectInputLayout InputLayout;
						PipelineStateSubobjectPrimitiveToplogy PrimitiveToplogy;

						PipelineStateSubobjectDepthStencilFormat DepthStencilFormat;
						PipelineStateSubobjectRenderTargetFormats RenderTargetFormats;
					};

					struct ComputePipelineStateDesc
					{
						PipelineStateSubobjectComputeShader ComputeShader;
					};

				public:
					template<typename PipelineStateDescType>
					INLINE static bool Create(ID3D12Device5* Device, PipelineStateDescType* Desc, ID3D12PipelineState** PipelineState)
					{
						//https://microsoft.github.io/DirectX-Specs/d3d/DepthBoundsTest.html

						D3D12_PIPELINE_STATE_STREAM_DESC stream;
						stream.pPipelineStateSubobjectStream = Desc;
						stream.SizeInBytes = sizeof(PipelineStateDescType);

						HRESULT res = Device->CreatePipelineState(&stream, IID_PPV_ARGS(PipelineState));

						return SUCCEEDED(res);
					}
				};

				class Resource
				{
				public:
					INLINE static bool CreateHeap(ID3D12Device5* Device, uint64 Size, bool IsCPUAccessible, D3D12_HEAP_FLAGS Flags, ID3D12Heap1** Heap)
					{
						D3D12_HEAP_DESC desc = {};
						desc.SizeInBytes = Size;

						D3D12_HEAP_TYPE type = D3D12_HEAP_TYPE_DEFAULT;
						D3D12_CPU_PAGE_PROPERTY cpuPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
						D3D12_MEMORY_POOL memoryPool = D3D12_MEMORY_POOL_UNKNOWN;

						if (IsCPUAccessible)
						{
							type = D3D12_HEAP_TYPE_CUSTOM;
							cpuPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;
							memoryPool = D3D12_MEMORY_POOL_L0;
						}

						desc.Properties.Type = type;
						desc.Properties.CPUPageProperty = cpuPageProperty;
						desc.Properties.MemoryPoolPreference = memoryPool;
						desc.Properties.CreationNodeMask = 0;
						desc.Properties.VisibleNodeMask = 0;

						desc.Flags = Flags;

						return SUCCEEDED(Device->CreateHeap1(&desc, nullptr, IID_PPV_ARGS(Heap)));
					}

					INLINE static bool CreateDescriptorHeap(ID3D12Device5* Device, D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32 Count, D3D12_DESCRIPTOR_HEAP_FLAGS Flags, ID3D12DescriptorHeap** DescriptorHeap)
					{
						D3D12_DESCRIPTOR_HEAP_DESC desc = {};
						desc.Type = Type;
						desc.NumDescriptors = Count;
						desc.Flags = Flags;

						return SUCCEEDED(Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(DescriptorHeap)));
					}

					INLINE static bool Create(ID3D12Device5* Device, ID3D12Heap1* Heap, uint64 Offset, D3D12_RESOURCE_DIMENSION Type, uint64 Alignment, uint32 Width, uint32 Height, DXGI_FORMAT Format, D3D12_TEXTURE_LAYOUT Layout, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES State, ID3D12Resource1** Resource)
					{
						D3D12_RESOURCE_DESC resourceDesc = {};
						resourceDesc.Dimension = Type;
						resourceDesc.Alignment = Alignment;
						resourceDesc.Width = Width;
						resourceDesc.Height = Height;
						resourceDesc.DepthOrArraySize = 1;
						resourceDesc.MipLevels = 1;
						resourceDesc.Format = Format;

						resourceDesc.SampleDesc.Quality = 0;
						resourceDesc.SampleDesc.Count = 1;

						resourceDesc.Layout = Layout;
						resourceDesc.Flags = Flags;

						D3D12_CLEAR_VALUE clearValue = {};
						clearValue.Format = Format;

						bool supplyClearValue = BitwiseUtils::IsEnabled(Flags, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET) || BitwiseUtils::IsEnabled(Flags, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

						return SUCCEEDED(Device->CreatePlacedResource(Heap, Offset, &resourceDesc, State, (supplyClearValue ? &clearValue : nullptr), IID_PPV_ARGS(Resource)));
					}

					INLINE static bool Map(ID3D12Resource1* Resource, byte** Buffer)
					{
						return SUCCEEDED(Resource->Map(0, nullptr, ReinterpretCast(void**, Buffer)));
					}

					INLINE static bool Unmap(ID3D12Resource1* Resource)
					{
						Resource->Unmap(0, nullptr);

						return true;
					}

					INLINE static bool CreateConstantBufferView(ID3D12Device5* Device, ID3D12Resource1* Resource, D3D12_CPU_DESCRIPTOR_HANDLE Handle)
					{
						D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
						desc.BufferLocation = Resource->GetGPUVirtualAddress();
						desc.SizeInBytes = Resource->GetDesc().Width;

						Device->CreateConstantBufferView(&desc, Handle);

						return true;
					}

					INLINE static bool CreateTextureShaderResourceView(ID3D12Device5* Device, ID3D12Resource1* Resource, DXGI_FORMAT Format, D3D12_RESOURCE_DIMENSION Dimension, D3D12_CPU_DESCRIPTOR_HANDLE Handle)
					{
						D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
						desc.Format = Format;

						switch (Dimension)
						{
						case D3D12_RESOURCE_DIMENSION_UNKNOWN:
							desc.ViewDimension = D3D12_SRV_DIMENSION_UNKNOWN;
							break;
						case D3D12_RESOURCE_DIMENSION_BUFFER:
							desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
							break;
						case D3D12_RESOURCE_DIMENSION_TEXTURE1D:
							desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
							break;
						case D3D12_RESOURCE_DIMENSION_TEXTURE2D:
							desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
							break;
						case D3D12_RESOURCE_DIMENSION_TEXTURE3D:
							desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
							break;
						}

						desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
						desc.Texture2D.MipLevels = 1;

						Device->CreateShaderResourceView(Resource, &desc, Handle);

						return true;
					}

					INLINE static bool CreateUnorderedAccessView(ID3D12Device5* Device, ID3D12Resource1* Resource, D3D12_CPU_DESCRIPTOR_HANDLE Handle)
					{
						Device->CreateUnorderedAccessView(Resource, nullptr, nullptr, Handle);

						return true;
					}

					INLINE static bool CreateSampler(ID3D12Device5* Device, const D3D12_SAMPLER_DESC& Description, D3D12_CPU_DESCRIPTOR_HANDLE Handle)
					{
						Device->CreateSampler(&Description, Handle);

						return true;
					}

					INLINE static bool CreateRenderTargetView(ID3D12Device5* Device, ID3D12Resource1* Resource, D3D12_CPU_DESCRIPTOR_HANDLE Handle)
					{
						Device->CreateRenderTargetView(Resource, nullptr, Handle);

						return true;
					}

					INLINE static bool CreateDepthStencilView(ID3D12Device5* Device, ID3D12Resource1* Resource, DXGI_FORMAT Format, D3D12_DSV_FLAGS Flags, D3D12_CPU_DESCRIPTOR_HANDLE Handle)
					{
						D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
						desc.Format = Format;
						desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
						desc.Flags = Flags;
						desc.Texture2D.MipSlice = 0;

						Device->CreateDepthStencilView(Resource, &desc, Handle);

						return true;
					}
				};

				class Command
				{
				public:
					INLINE static bool CreateCommandQueue(ID3D12Device5* Device, D3D12_COMMAND_LIST_TYPE Type, ID3D12CommandQueue** CommandQueue)
					{
						D3D12_COMMAND_QUEUE_DESC desc = {};
						desc.Type = Type;
						desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
						desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
						desc.NodeMask = 0;

						return SUCCEEDED(Device->CreateCommandQueue(&desc, IID_PPV_ARGS(CommandQueue)));
					}

					INLINE static bool CreateCommandAllocator(ID3D12Device5* Device, D3D12_COMMAND_LIST_TYPE Type, ID3D12CommandAllocator** CommandAllocator)
					{
						if (!SUCCEEDED(Device->CreateCommandAllocator(Type, IID_PPV_ARGS(CommandAllocator))))
							return false;

						return ResetCommandAllocator(*CommandAllocator);
					}

					INLINE static bool ResetCommandAllocator(ID3D12CommandAllocator* CommandAllocator)
					{
						return SUCCEEDED(CommandAllocator->Reset());
					}

					INLINE static bool CreateCommandList(ID3D12Device5* Device, ID3D12CommandAllocator* CommandAllocator, D3D12_COMMAND_LIST_TYPE Type, ID3D12GraphicsCommandList4** CommandList)
					{
						return SUCCEEDED(Device->CreateCommandList(0, Type, CommandAllocator, nullptr, IID_PPV_ARGS(CommandList)));
					}

					INLINE static bool ResetCommandList(ID3D12GraphicsCommandList4* CommandList, ID3D12CommandAllocator* CommandAllocator)
					{
						return SUCCEEDED(CommandList->Reset(CommandAllocator, nullptr));
					}

					INLINE static bool CloseCommandList(ID3D12GraphicsCommandList4* CommandList)
					{
						return SUCCEEDED(CommandList->Close());
					}

					INLINE static void ExecuteCommandList(ID3D12CommandQueue* CommandQueue, ID3D12GraphicsCommandList4* CommandList)
					{
						CommandQueue->ExecuteCommandLists(1, ReinterpretCast(ID3D12CommandList**, &CommandList));
					}

					INLINE static void AddSetGraphicsRootSignature(ID3D12GraphicsCommandList4* CommandList, ID3D12RootSignature* RootSignature)
					{
						CommandList->SetGraphicsRootSignature(RootSignature);
					}

					INLINE static void AddSetComputeRootSignature(ID3D12GraphicsCommandList4* CommandList, ID3D12RootSignature* RootSignature)
					{
						CommandList->SetComputeRootSignature(RootSignature);
					}

					INLINE static void AddSetPipelineState(ID3D12GraphicsCommandList4* CommandList, ID3D12PipelineState* PipelineState)
					{
						CommandList->SetPipelineState(PipelineState);
					}

					INLINE static void AddSetDescriptorHeap(ID3D12GraphicsCommandList4* CommandList, ID3D12DescriptorHeap* const* DescriptorHeaps, uint8 DescriptHeapCount)
					{
						CommandList->SetDescriptorHeaps(DescriptHeapCount, DescriptorHeaps);
					}

					INLINE static void AddSetGraphicsConstantBuffer(ID3D12GraphicsCommandList4* CommandList, uint8 RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS Address)
					{
						CommandList->SetGraphicsRootConstantBufferView(RootParameterIndex, Address);
					}

					INLINE static void AddSetGraphicsRootDescriptorTable(ID3D12GraphicsCommandList4* CommandList, uint8 RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE Address)
					{
						CommandList->SetGraphicsRootDescriptorTable(RootParameterIndex, Address);
					}

					INLINE static void AddTransitionResourceBarrier(ID3D12GraphicsCommandList4* CommandList, ID3D12Resource1* Resource, D3D12_RESOURCE_STATES BeforeState, D3D12_RESOURCE_STATES AfterState)
					{
						D3D12_RESOURCE_BARRIER barrier = {};
						barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
						barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

						barrier.Transition.pResource = Resource;
						barrier.Transition.StateBefore = BeforeState;
						barrier.Transition.StateAfter = AfterState;
						barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

						CommandList->ResourceBarrier(1, &barrier);
					}

					INLINE static void AddCopyBufferCommand(ID3D12GraphicsCommandList4* CommandList, ID3D12Resource1* Source, ID3D12Resource1* Destination, uint32 Size)
					{
						CommandList->CopyBufferRegion(Destination, 0, Source, 0, Size);
					}

					INLINE static void AddCopyTextureCommand(ID3D12GraphicsCommandList4* CommandList, ID3D12Resource1* Source, uint32 SourcePositionX, uint32 SourcePositionY, ID3D12Resource1* Destination, uint32 DestinationPositionX, uint32 DestinationPositionY, uint32 Width, uint32 Height)
					{
						D3D12_RESOURCE_DESC desc = Source->GetDesc();

						D3D12_TEXTURE_COPY_LOCATION srcLoc = {};
						srcLoc.pResource = Source;
						srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
						srcLoc.SubresourceIndex = 0;

						D3D12_TEXTURE_COPY_LOCATION destLoc = {};
						destLoc.pResource = Destination;
						destLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
						destLoc.SubresourceIndex = 0;

						D3D12_BOX sourceRegion;
						sourceRegion.left = SourcePositionX;
						sourceRegion.top = SourcePositionY;
						sourceRegion.right = SourcePositionX + Width;
						sourceRegion.bottom = SourcePositionY + Height;
						sourceRegion.front = 0;
						sourceRegion.back = 1;

						CommandList->CopyTextureRegion(&destLoc, DestinationPositionX, DestinationPositionY, 0, &srcLoc, &sourceRegion);
					}

					INLINE static void AddCopyTextureToBufferCommand(ID3D12GraphicsCommandList4* CommandList, ID3D12Resource1* Source, ID3D12Resource1* Destination)
					{
						D3D12_RESOURCE_DESC desc = Source->GetDesc();

						D3D12_TEXTURE_COPY_LOCATION srcLoc = {};
						srcLoc.pResource = Source;
						srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
						srcLoc.SubresourceIndex = 0;

						D3D12_TEXTURE_COPY_LOCATION destLoc = {};
						destLoc.pResource = Destination;
						destLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

						ID3D12Device5* device = nullptr;
						if (!SUCCEEDED(CommandList->GetDevice(IID_PPV_ARGS(&device))))
							return;
						Support::GetCopyableFootprint(device, Source, &destLoc.PlacedFootprint);
						ReleaseInstance(device);

						CommandList->CopyTextureRegion(&destLoc, 0, 0, 0, &srcLoc, nullptr);
					}

					INLINE static void AddCopyBufferToTextureCommand(ID3D12GraphicsCommandList4* CommandList, ID3D12Resource1* Source, ID3D12Resource1* Destination)
					{
						D3D12_RESOURCE_DESC desc = Destination->GetDesc();

						D3D12_TEXTURE_COPY_LOCATION srcLoc = {};
						srcLoc.pResource = Source;
						srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

						ID3D12Device5* device = nullptr;
						if (!SUCCEEDED(CommandList->GetDevice(IID_PPV_ARGS(&device))))
							return;
						Support::GetCopyableFootprint(device, Destination, &srcLoc.PlacedFootprint);
						ReleaseInstance(device);

						D3D12_TEXTURE_COPY_LOCATION destLoc = {};
						destLoc.pResource = Destination;
						destLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
						destLoc.SubresourceIndex = 0;

						CommandList->CopyTextureRegion(&destLoc, 0, 0, 0, &srcLoc, nullptr);
					}

					INLINE static void AddSetViewportCommand(ID3D12GraphicsCommandList4* CommandList, const D3D12_VIEWPORT* Viewport)
					{
						CommandList->RSSetViewports(1, Viewport);

						D3D12_RECT scissor = { Viewport->TopLeftX, Viewport->TopLeftY, Viewport->Width, Viewport->Height };

						CommandList->RSSetScissorRects(1, &scissor);
					}

					INLINE static void AddClearRenderTargetCommand(ID3D12GraphicsCommandList4* CommandList, D3D12_CPU_DESCRIPTOR_HANDLE Handle, float32* Color)
					{
						CommandList->ClearRenderTargetView(Handle, Color, 0, nullptr);
					}

					INLINE static void AddClearDepthStencilCommand(ID3D12GraphicsCommandList4* CommandList, D3D12_CPU_DESCRIPTOR_HANDLE Handle, D3D12_CLEAR_FLAGS Flags, float32 Depth, uint8 Stencil)
					{
						CommandList->ClearDepthStencilView(Handle, Flags, Depth, Stencil, 0, nullptr);
					}

					INLINE static void AddSetTargets(ID3D12GraphicsCommandList4* CommandList, D3D12_CPU_DESCRIPTOR_HANDLE* ColorHandles, uint8 ColorHandlesCount, D3D12_CPU_DESCRIPTOR_HANDLE* DepthStencilHandle)
					{
						CommandList->OMSetRenderTargets(ColorHandlesCount, ColorHandles, false, DepthStencilHandle);
					}

					INLINE static void AddSetVertexBufferCommand(ID3D12GraphicsCommandList4* CommandList, ID3D12Resource1* Resource, uint32 Size, uint32 Stride)
					{
						D3D12_VERTEX_BUFFER_VIEW view = {};
						view.BufferLocation = Resource->GetGPUVirtualAddress();
						view.SizeInBytes = Size;
						view.StrideInBytes = Stride;

						CommandList->IASetVertexBuffers(0, 1, &view);
					}

					INLINE static void AddSetIndexBufferCommand(ID3D12GraphicsCommandList4* CommandList, ID3D12Resource1* Resource, uint32 Size)
					{
						D3D12_INDEX_BUFFER_VIEW view = {};
						view.BufferLocation = Resource->GetGPUVirtualAddress();
						view.SizeInBytes = Size;
						view.Format = DXGI_FORMAT_R32_UINT;

						CommandList->IASetIndexBuffer(&view);
					}

					INLINE static void AddSetPrimitiveTopologyCommand(ID3D12GraphicsCommandList4* CommandList, D3D12_PRIMITIVE_TOPOLOGY Topology)
					{
						CommandList->IASetPrimitiveTopology(Topology);
					}

					INLINE static void AddDrawIndexedCommand(ID3D12GraphicsCommandList4* CommandList, uint32 IndexCount)
					{
						CommandList->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);
					}

					INLINE static void AddDrawCommand(ID3D12GraphicsCommandList4* CommandList, uint32 VertexCount)
					{
						CommandList->DrawInstanced(VertexCount, 1, 0, 0);
					}

					INLINE static bool AddResourceStateAssertion(ID3D12DebugCommandList2* DebugCommandList, ID3D12Resource1* Resource, D3D12_RESOURCE_STATES State)
					{
						return DebugCommandList->AssertResourceState(Resource, D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, State);
					}
				};

				class Fence
				{
				public:
					INLINE static bool Create(ID3D12Device5* Device, ID3D12Fence** Fence)
					{
						return SUCCEEDED(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(Fence)));
					}

					INLINE static bool Signal(ID3D12CommandQueue* CommandQueue, ID3D12Fence* Fence, uint64 Value)
					{
						return SUCCEEDED(CommandQueue->Signal(Fence, Value));
					}

					INLINE static bool Wait(ID3D12CommandQueue* CommandQueue, ID3D12Fence* Fence, uint64 Value)
					{
						return SUCCEEDED(CommandQueue->Wait(Fence, Value));
					}

					INLINE static uint64 GetValue(ID3D12Fence* Fence)
					{
						return Fence->GetCompletedValue();
					}

					INLINE static bool SetEvent(ID3D12Fence* Fence, uint64 Value, PlatformOS::Handle Event)
					{
						return SUCCEEDED(Fence->SetEventOnCompletion(Value, Event));
					}
				};

			public:
				INLINE static bool ReleaseInstance(IUnknown* Resource)
				{
					Resource->Release();

					return true;
				}

				INLINE static bool DestroyInstance(IUnknown* Resource)
				{
					Resource->Release();

					return true;
				}
			};
		}
	}
}

#endif