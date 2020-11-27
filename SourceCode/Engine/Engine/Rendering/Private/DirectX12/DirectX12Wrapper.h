// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DIRECTX12_WRAPPER_H
#define DIRECTX12_WRAPPER_H

#include <Common\PrimitiveTypes.h>
#include <Common\BitwiseUtils.h>
#include <Common\CharacterUtility.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Rendering\Private\ShaderCompiler\Compiler.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <d3dcompiler.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace Rendering
	{
		namespace Private
		{
			using namespace ShaderCompiler;

			namespace DirectX12
			{
				class DirectX12Wrapper
				{
				public:
					INLINE static bool EnableDebugLayer(void)
					{
						ID3D12Debug* debug = nullptr;
						if (!SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
							return false;

						debug->EnableDebugLayer();

						return true;
					}

					INLINE static bool ReleaseResource(IUnknown* Resource)
					{
						uint64 reference = Resource->Release();

						return (reference == 0);
					}

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

							if (!SUCCEEDED(D3D12CreateDevice(tempAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)))
								continue;

							if (desc.DedicatedVideoMemory < maxMemory)
								continue;

							maxMemory = desc.DedicatedVideoMemory;
							tempAdapter->QueryInterface<IDXGIAdapter3>(Adapter);
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

					INLINE static bool GetInfoQueue(ID3D12Device5* Device, ID3D12InfoQueue** InfoQueue)
					{
						return SUCCEEDED(Device->QueryInterface<ID3D12InfoQueue>(InfoQueue));
					}

					INLINE static bool CreateFence(ID3D12Device5* Device, ID3D12Fence** Fence)
					{
						return SUCCEEDED(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(Fence)));
					}

					INLINE static bool CheckTearingSupport(IDXGIFactory5* Factory)
					{
						bool result = false;
						if (SUCCEEDED(Factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &result, sizeof(result))))
							return result;

						return false;
					}

					INLINE static bool CompileShader(cstr Source, cstr Target, D3D12_SHADER_BYTECODE* ByteCode, cstr* ErrorMessage)
					{
						ID3DBlob* byteCodeBlob = nullptr;
						ID3DBlob* messageBlob = nullptr;

						uint32 flags = 0;
						flags |= D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY;

						if (!SUCCEEDED(D3DCompile2(Source, CharacterUtility::GetLength(Source), nullptr, nullptr, nullptr, Compiler::ENTRY_POINT_NAME, Target, flags, 0, 0, nullptr, 0, &byteCodeBlob, &messageBlob)))
						{
							if (ErrorMessage != nullptr)
								*ErrorMessage = ReinterpretCast(cstr, messageBlob->GetBufferPointer());

							return false;
						}

						ByteCode->BytecodeLength = byteCodeBlob->GetBufferSize();
						ByteCode->pShaderBytecode = ReinterpretCast(const void*, byteCodeBlob->GetBufferPointer());

						return true;
					}

					INLINE static bool MapResource(ID3D12Resource* Resource, byte** Buffer)
					{
						return SUCCEEDED(Resource->Map(0, nullptr, ReinterpretCast(void**, Buffer)));
					}

					INLINE static bool UnmapResource(ID3D12Resource* Resource)
					{
						Resource->Unmap(0, nullptr);

						return true;
					}

					INLINE static bool CreateResource(ID3D12Device5* Device, D3D12_HEAP_TYPE HeapType, D3D12_RESOURCE_DIMENSION DimensionType, D3D12_TEXTURE_LAYOUT Layout, uint16 Width, uint16 Height, DXGI_FORMAT Format, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES State, ID3D12Resource** Resource)
					{
						D3D12_HEAP_PROPERTIES heapProperties = {};
						heapProperties.Type = HeapType;
						heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
						heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
						heapProperties.CreationNodeMask = 0;
						heapProperties.VisibleNodeMask = 0;

						D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE;

						D3D12_RESOURCE_DESC resourceDesc = {};
						resourceDesc.Dimension = DimensionType;
						resourceDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
						resourceDesc.Width = Width;
						resourceDesc.Height = Height;
						resourceDesc.DepthOrArraySize = 1;
						resourceDesc.MipLevels = 1;
						resourceDesc.Format = Format;

						//HITODO: should be configurable
						resourceDesc.SampleDesc.Quality = 0;
						resourceDesc.SampleDesc.Count = 1;

						resourceDesc.Layout = Layout;
						resourceDesc.Flags = Flags;

						return SUCCEEDED(Device->CreateCommittedResource(&heapProperties, flags, &resourceDesc, State, nullptr, IID_PPV_ARGS(Resource)));
					}

					INLINE static bool CreateTexture(ID3D12Device5* Device, D3D12_RESOURCE_DIMENSION Type, uint16 Width, uint16 Height, DXGI_FORMAT Format, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES State, ID3D12Resource** Texture)
					{
						return CreateResource(Device, D3D12_HEAP_TYPE_DEFAULT, Type, D3D12_TEXTURE_LAYOUT_UNKNOWN, Width, Height, Format, Flags, State, Texture);
					}

					INLINE static bool CreateCommandQueue(ID3D12Device5* Device, D3D12_COMMAND_LIST_TYPE Type, ID3D12CommandQueue** CommandQueue)
					{
						D3D12_COMMAND_QUEUE_DESC desc = {};
						desc.Type = Type;
						desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
						desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
						desc.NodeMask = 0;

						return SUCCEEDED(Device->CreateCommandQueue(&desc, IID_PPV_ARGS(CommandQueue)));
					}

					INLINE static bool CreateSwapChain(IDXGIFactory5* Factory, ID3D12CommandQueue* CommandQueue, PlatformWindow::WindowHandle Handle, uint8 BackBufferCount, IDXGISwapChain4** SwapChain)
					{
						DXGI_SWAP_CHAIN_DESC1 desc = {};
						desc.Width = 0;
						desc.Height = 0;
						desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
						desc.Stereo = false;

						//HITODO: should be configurable
						desc.SampleDesc.Quality = 0;
						desc.SampleDesc.Count = 1;

						desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
						desc.BufferCount = BackBufferCount;
						desc.Scaling = DXGI_SCALING_STRETCH;
						desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
						desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
						desc.Flags = CheckTearingSupport(Factory) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

						IDXGISwapChain1* swapChain = nullptr;
						if (!SUCCEEDED(Factory->CreateSwapChainForHwnd(CommandQueue, (HWND)Handle, &desc, nullptr, nullptr, &swapChain)))
							return false;

						if (!SUCCEEDED(Factory->MakeWindowAssociation((HWND)Handle, DXGI_MWA_NO_ALT_ENTER)))
							return false;

						return SUCCEEDED(swapChain->QueryInterface<IDXGISwapChain4>(SwapChain));
					}

					INLINE static bool GetSwapChainBackBuffers(IDXGISwapChain4* SwapChain, uint8 BackBufferCount, ID3D12Resource** BackBuffers)
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

					INLINE static bool CreateDescriptorHeap(ID3D12Device5* Device, D3D12_DESCRIPTOR_HEAP_TYPE Type, uint8 Count, ID3D12DescriptorHeap** DescriptorHeap)
					{
						D3D12_DESCRIPTOR_HEAP_DESC desc = {};
						desc.Type = Type;
						desc.NumDescriptors = Count;

						return SUCCEEDED(Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(DescriptorHeap)));
					}

					INLINE static bool CreateRenderTargetView(ID3D12Device5* Device, ID3D12Resource* BackBuffer, ID3D12DescriptorHeap* DescriptorHeap, uint8 Index, uint32 RenderTargetViewDescriptorSize)
					{
						D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle(DescriptorHeap->GetCPUDescriptorHandleForHeapStart());

						cpuHandle.ptr += Index * RenderTargetViewDescriptorSize;

						Device->CreateRenderTargetView(BackBuffer, nullptr, cpuHandle);

						return true;
					}

					INLINE static bool CreateDepthStencilView(ID3D12Device5* Device, ID3D12Resource* BackBuffer, ID3D12DescriptorHeap* DescriptorHeap, uint8 Index, uint32 RenderTargetViewDescriptorSize)
					{
						D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle(DescriptorHeap->GetCPUDescriptorHandleForHeapStart());

						cpuHandle.ptr += Index * RenderTargetViewDescriptorSize;

						Device->CreateDepthStencilView(BackBuffer, nullptr, cpuHandle);

						return true;
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
						if (!SUCCEEDED(Device->CreateCommandList(0, Type, CommandAllocator, nullptr, IID_PPV_ARGS(CommandList))))
							return false;

						if (!SUCCEEDED((*CommandList)->Close()))
							return false;

						return ResetCommandList(*CommandList, CommandAllocator);
					}

					INLINE static bool ResetCommandList(ID3D12GraphicsCommandList4* CommandList, ID3D12CommandAllocator* CommandAllocator)
					{
						return SUCCEEDED(CommandList->Reset(CommandAllocator, nullptr));
					}

					INLINE static bool AddTransitionResourceBarrier(ID3D12GraphicsCommandList4* CommandList, ID3D12Resource* Resource, D3D12_RESOURCE_STATES BeforeState, D3D12_RESOURCE_STATES AfterState)
					{
						D3D12_RESOURCE_BARRIER barrier = {};
						barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
						barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;

						barrier.Transition.pResource = Resource;
						barrier.Transition.StateBefore = BeforeState;
						barrier.Transition.StateAfter = AfterState;
						barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

						CommandList->ResourceBarrier(1, &barrier);

						return true;
					}

					INLINE static bool AddCopyResourceCommand(ID3D12GraphicsCommandList4* CommandList, ID3D12Resource* Source, ID3D12Resource* Destination)
					{
						CommandList->CopyResource(Destination, Source);

						return true;
					}

					INLINE static bool AddClearRenderTargetCommand(ID3D12GraphicsCommandList4* CommandList, ID3D12DescriptorHeap* DescriptorHeap, uint32 BackBufferIndex, uint32 DescriptorSize, float32* Color)
					{
						D3D12_CPU_DESCRIPTOR_HANDLE desc = DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
						desc.ptr += BackBufferIndex * DescriptorSize;

						CommandList->ClearRenderTargetView(desc, Color, 0, nullptr);

						return true;
					}

					INLINE static bool AddClearDepthStencilCommand(ID3D12GraphicsCommandList4* CommandList, ID3D12DescriptorHeap* DescriptorHeap, uint32 BackBufferIndex, uint32 DescriptorSize, D3D12_CLEAR_FLAGS Flags, float32 Depth, uint8 Stencil)
					{
						D3D12_CPU_DESCRIPTOR_HANDLE desc = DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
						desc.ptr += BackBufferIndex * DescriptorSize;

						CommandList->ClearDepthStencilView(desc, Flags, Depth, Stencil, 0, nullptr);

						return true;
					}

					INLINE static bool ExecuteCommandList(ID3D12CommandQueue* CommandQueue, ID3D12GraphicsCommandList4* CommandList)
					{
						if (!SUCCEEDED(CommandList->Close()))
							return false;

						CommandQueue->ExecuteCommandLists(1, ReinterpretCast(ID3D12CommandList**, &CommandList));

						return true;
					}

					INLINE static bool IncrementFence(ID3D12CommandQueue* CommandQueue, ID3D12Fence* Fence, uint64& Value, uint64& WaitValue)
					{
						WaitValue = ++Value;
						if (!SUCCEEDED(CommandQueue->Signal(Fence, WaitValue)))
							return false;

						return true;
					}

					INLINE static bool WaitForFence(ID3D12Fence* Fence, const uint64& Value, HANDLE Event)
					{
						if (Fence->GetCompletedValue() >= Value)
							return true;

						if (!SUCCEEDED(Fence->SetEventOnCompletion(Value, Event)))
							return false;

						::WaitForSingleObject(Event, INFINITE);

						return true;
					}

					INLINE static bool IterateOverDebugMessages(ID3D12InfoQueue* InfoQueue, std::function<void(D3D12_MESSAGE_ID, D3D12_MESSAGE_CATEGORY, cstr, D3D12_MESSAGE_SEVERITY)> Callback)
					{
						uint64 count = InfoQueue->GetNumStoredMessages();
						if (count == 0)
							return false;

						FrameAllocator allocator("Debug Message Allocator", RenderingAllocators::RenderingSystemAllocator, MegaByte);
						for (uint64 i = 0; i < count; ++i)
						{
							uint64 size = 0;
							InfoQueue->GetMessage(i, nullptr, &size);

							D3D12_MESSAGE* message = ReinterpretCast(D3D12_MESSAGE*, AllocateMemory(&allocator, size));

							InfoQueue->GetMessage(i, message, &size);

							Callback(message->ID, message->Category, message->pDescription, message->Severity);
						}

						allocator.Reset();

						InfoQueue->ClearStoredMessages();

						return true;
					}
				};
			}
		}
	}
}

#endif