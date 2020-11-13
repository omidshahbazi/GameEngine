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

					INLINE static bool CheckTearingSupport(IDXGIFactory5* Factory)
					{
						bool result = false;
						if (SUCCEEDED(Factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &result, sizeof(result))))
							return result;

						return false;
					}

					INLINE static bool CreateCommandQueue(ID3D12Device5* Device, ID3D12CommandQueue** CommandQueue)
					{
						D3D12_COMMAND_QUEUE_DESC desc = {};
						desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
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

						Factory->MakeWindowAssociation((HWND)Handle, DXGI_MWA_NO_ALT_ENTER);

						return SUCCEEDED(swapChain->QueryInterface<IDXGISwapChain4>(SwapChain));
					}

					INLINE static bool CreateDescriptorHeap(ID3D12Device5* Device, D3D12_DESCRIPTOR_HEAP_TYPE Type, uint8 BackBufferCount, ID3D12DescriptorHeap** DescriptorHeap)
					{
						D3D12_DESCRIPTOR_HEAP_DESC desc = {};
						desc.Type = Type;
						desc.NumDescriptors = BackBufferCount;

						return SUCCEEDED(Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(DescriptorHeap)));
					}

					INLINE static bool CreateRenderTargetViews(ID3D12Device5* Device, IDXGISwapChain4* SwapChain, ID3D12DescriptorHeap* DescriptorHeap, uint8 BackBufferCount, uint32 RenderTargetViewDescriptorSize, ID3D12Resource** BackBuffers)
					{
						D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle(DescriptorHeap->GetCPUDescriptorHandleForHeapStart());

						for (uint8 i = 0; i < BackBufferCount; ++i)
						{
							if (!SUCCEEDED(SwapChain->GetBuffer(i, IID_PPV_ARGS(&BackBuffers[i]))))
								return false;

							Device->CreateRenderTargetView(BackBuffers[i], nullptr, cpuHandle);

							cpuHandle.ptr += RenderTargetViewDescriptorSize;
						}

						return true;
					}

					INLINE static bool CreateCommandAllocator(ID3D12Device5* Device, D3D12_COMMAND_LIST_TYPE Type, ID3D12CommandAllocator** CommandAllocator)
					{
						return SUCCEEDED(Device->CreateCommandAllocator(Type, IID_PPV_ARGS(CommandAllocator)));
					}

					INLINE static bool CreateCommandList(ID3D12CommandAllocator* CommandAllocator, ID3D12Device5* Device, D3D12_COMMAND_LIST_TYPE Type, ID3D12GraphicsCommandList** CommandList)
					{
						if (!SUCCEEDED(Device->CreateCommandList(0, Type, CommandAllocator, nullptr, IID_PPV_ARGS(CommandList))))
							return false;

						(*CommandList)->Close();

						return true;
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

					INLINE static bool CreateTexture(ID3D12Device5* Device, D3D12_RESOURCE_DIMENSION Type, uint16 Width, uint16 Height, DXGI_FORMAT Format, D3D12_RESOURCE_FLAGS Flags, bool HasCPUAccess, ID3D12Resource** Texture)
					{
						D3D12_RESOURCE_DESC desc = {};
						desc.Dimension = Type;
						desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
						desc.Width = Width;
						desc.Height = Height;
						desc.DepthOrArraySize = 1;
						desc.MipLevels = 1;
						desc.Format = Format;

						//HITODO: should be configurable
						desc.SampleDesc.Quality = 0;
						desc.SampleDesc.Count = 1;

						desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
						desc.Flags = D3D12_RESOURCE_FLAG_NONE;

						D3D12_HEAP_PROPERTIES heapProperties = {};
						heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
						heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
						heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
						heapProperties.CreationNodeMask = 0;
						heapProperties.VisibleNodeMask = 0;

						D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_NONE;

						return SUCCEEDED(Device->CreateCommittedResource(&heapProperties, flags, &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(Texture)));
					}




					INLINE static bool Clear(ID3D12GraphicsCommandList* CommandList, ID3D12DescriptorHeap* DescriptorHeap, uint32 BackBufferIndex, uint32 RenderTargetViewDescriptorSize, FLOAT* Color)
					{
						D3D12_CPU_DESCRIPTOR_HANDLE desc = DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
						desc.ptr += BackBufferIndex * RenderTargetViewDescriptorSize;

						CommandList->ClearRenderTargetView(desc, Color, 0, nullptr);

						return true;
					}


					INLINE static bool Present(IDXGISwapChain4* SwapChain, bool VSync = true, bool AllowTearing = false)
					{
						uint8 syncInterval = VSync ? 1 : 0;
						uint8 presentFlags = (AllowTearing && !VSync) ? DXGI_PRESENT_ALLOW_TEARING : 0;

						return SUCCEEDED(SwapChain->Present(syncInterval, presentFlags));
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