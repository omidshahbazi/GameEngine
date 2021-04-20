// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DIRECTX12_WRAPPER_H
#define DIRECTX12_WRAPPER_H

#include <Common\PrimitiveTypes.h>
#include <Common\BitwiseUtils.h>
#include <Common\CharacterUtility.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Rendering\Private\ProgramCompiler\Compiler.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <d3d12.h>
#include <d3dcompiler.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace Rendering
	{
		namespace Private
		{
			using namespace ProgramCompiler;

			namespace DirectX12
			{
				class DirectX12Wrapper
				{
				public:
					struct RootSignatureDesc
					{
					public:
						static const uint8 MAX_PARAMETER_COUNT = 128;

						enum class ParameterTypes
						{
							DescriptorTable = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
							Constants = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
							ConstantBufferView = D3D12_ROOT_PARAMETER_TYPE_CBV,
							ProgramResourceView = D3D12_ROOT_PARAMETER_TYPE_SRV,
							UnorderedAccessView = D3D12_ROOT_PARAMETER_TYPE_UAV
						};

						enum class ShaderVisibilities
						{
							All = D3D12_SHADER_VISIBILITY_ALL,
							Vertex = D3D12_SHADER_VISIBILITY_VERTEX,
							Hull = D3D12_SHADER_VISIBILITY_HULL,
							Domain = D3D12_SHADER_VISIBILITY_DOMAIN,
							Geometry = D3D12_SHADER_VISIBILITY_GEOMETRY,
							Pixel = D3D12_SHADER_VISIBILITY_PIXEL
						};

						enum class DescriptorRangeTypes
						{
							ProgramResourceView = D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
							UnorderedAccessView = D3D12_DESCRIPTOR_RANGE_TYPE_UAV,
							ConstantBufferView = D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
							Sampler = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER
						};

						struct DescriptorRange
						{
						public:
							DescriptorRangeTypes Type;
							uint32 DescriptorCount;
							uint32 BaseShaderRegister;
							uint32 RegisterSpace;
						};

						struct DescriptorTable
						{
							static const uint8 MAX_DESCRIPTOR_RANGE_COUNT = 128;

						public:
							DescriptorRange DescriptorRanges[MAX_DESCRIPTOR_RANGE_COUNT];
							uint32 DescriptorRangeCount;
						};

						struct Constants
						{
						public:
							uint32 ShaderRegister;
							uint32 ValueCount;
						};

						struct Descriptor
						{
						public:
							uint32 ShaderRegister;
						};

						struct ParameterDesc
						{
						public:
							ParameterTypes ParameterType;
							ShaderVisibilities ShaderVisibility;

							union
							{
							public:
								DescriptorTable DescriptorTable;
								Constants Constants;
								Descriptor Descriptor;
							};
						};

					public:
						uint8 ParameterCount;
						ParameterDesc Parameters[MAX_PARAMETER_COUNT];
					};

					struct DefaultValue
					{
					};

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
					typedef PipelineStateSubobject<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_PS> PipelineStateSubobjectPixelShader;
					typedef PipelineStateSubobject<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_DS> PipelineStateSubobjectDomainShader;
					typedef PipelineStateSubobject<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_HS> PipelineStateSubobjectHullShader;
					typedef PipelineStateSubobject<D3D12_SHADER_BYTECODE, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_GS> PipelineStateSubobjectGeometryShader;
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
					typedef PipelineStateSubobject<D3D12_VIEW_INSTANCING_DESC, D3D12_PIPELINE_STATE_SUBOBJECT_TYPE_VIEW_INSTANCING, DefaultType>	PipelineStateSubobjectViewInstancing;

					struct GraphicsPipelineStateDesc
					{
						PipelineStateSubobjectRootSignature RootSignature;
						PipelineStateSubobjectVertexShader VertexShader;
						PipelineStateSubobjectPixelShader PixelShader;

						PipelineStateSubobjectRasterizerState RasterizerState;
						PipelineStateSubobjectDepthStencil DepthStencil;
						PipelineStateSubobjectBlendState BlendState;

						PipelineStateSubobjectInputLayout InputLayout;
						PipelineStateSubobjectPrimitiveToplogy PrimitiveToplogy;
						PipelineStateSubobjectDepthStencilFormat DepthStencilFormat;
					};

					//struct GraphicsPipelineStateDesc1
					//{
					//	PipelineStateSubobjectRootSignature RootSignature;
					//	PipelineStateSubobjectInputLayout InputLayout;
					//	PipelineStateSubobjectPrimitiveToplogy PrimitiveToplogy;
					//	PipelineStateSubobjectVertexShader VertexShader;
					//	PipelineStateSubobjectPixelShader PixelShader;
					//	PipelineStateSubobjectDepthStencilFormat DepthStencilFormat;
					//	PipelineStateSubobjectRenderTargetFormats RenderTargetFormats;
					//};

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

					INLINE static bool ReleaseInstance(IUnknown* Resource)
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

							if (!SUCCEEDED(D3D12CreateDevice(tempAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device5), nullptr)))
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

					INLINE static bool CreateRootSignature(ID3D12Device5* Device, RootSignatureDesc* Desc, ID3D12RootSignature** RootSignature, cstr* ErrorMessage)
					{
						D3D12_FEATURE_DATA_ROOT_SIGNATURE featureDataRootSignature = {};
						featureDataRootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
						if (!SUCCEEDED(Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureDataRootSignature, sizeof(featureDataRootSignature))))
							featureDataRootSignature.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

						D3D12_ROOT_PARAMETER1 rootParameters[RootSignatureDesc::MAX_PARAMETER_COUNT];
						PlatformMemory::Set(rootParameters, 0, RootSignatureDesc::MAX_PARAMETER_COUNT);

						D3D12_DESCRIPTOR_RANGE1 descriptorRanges[RootSignatureDesc::DescriptorTable::MAX_DESCRIPTOR_RANGE_COUNT];
						PlatformMemory::Set(descriptorRanges, 0, RootSignatureDesc::DescriptorTable::MAX_DESCRIPTOR_RANGE_COUNT);

						for (uint8 i = 0; i < Desc->ParameterCount; ++i)
						{
							RootSignatureDesc::ParameterDesc& paramDesc = Desc->Parameters[i];
							D3D12_ROOT_PARAMETER1& rootParam = rootParameters[i];

							rootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)paramDesc.ParameterType;
							rootParam.ShaderVisibility = (D3D12_SHADER_VISIBILITY)paramDesc.ShaderVisibility;

							switch (paramDesc.ParameterType)
							{
							case RootSignatureDesc::ParameterTypes::DescriptorTable:
							{
								rootParam.DescriptorTable.NumDescriptorRanges = paramDesc.DescriptorTable.DescriptorRangeCount;
								rootParam.DescriptorTable.pDescriptorRanges = descriptorRanges;

								for (uint32 i = 0; i < paramDesc.DescriptorTable.DescriptorRangeCount; ++i)
								{
									RootSignatureDesc::DescriptorRange& descriptorRange = paramDesc.DescriptorTable.DescriptorRanges[i];
									D3D12_DESCRIPTOR_RANGE1& descriptorRange1 = descriptorRanges[i];

									descriptorRange1.RangeType = (D3D12_DESCRIPTOR_RANGE_TYPE)descriptorRange.Type;
									descriptorRange1.NumDescriptors = descriptorRange.DescriptorCount;
									descriptorRange1.BaseShaderRegister = descriptorRange.BaseShaderRegister;
									descriptorRange1.RegisterSpace = descriptorRange.RegisterSpace;
									descriptorRange1.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
								}
							} break;

							case RootSignatureDesc::ParameterTypes::Constants:
							{
								rootParam.Constants.ShaderRegister = paramDesc.Constants.ShaderRegister;
								rootParam.Constants.Num32BitValues = paramDesc.Constants.ValueCount;
							} break;

							case RootSignatureDesc::ParameterTypes::ConstantBufferView:
							case RootSignatureDesc::ParameterTypes::ProgramResourceView:
							case RootSignatureDesc::ParameterTypes::UnorderedAccessView:
							{
								rootParam.Descriptor.ShaderRegister = paramDesc.Descriptor.ShaderRegister;
							} break;
							}
						}

						D3D12_VERSIONED_ROOT_SIGNATURE_DESC versionedRootSignatureDesc = {};
						versionedRootSignatureDesc.Version = featureDataRootSignature.HighestVersion;
						versionedRootSignatureDesc.Desc_1_1.NumParameters = Desc->ParameterCount;
						versionedRootSignatureDesc.Desc_1_1.pParameters = rootParameters;
						versionedRootSignatureDesc.Desc_1_1.NumStaticSamplers = 0;
						versionedRootSignatureDesc.Desc_1_1.pStaticSamplers = nullptr;
						versionedRootSignatureDesc.Desc_1_1.Flags =
							D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
							D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
							D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
							D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

						ID3DBlob* versionedRootSignatureBlob = nullptr;
						ID3DBlob* messageBlob = nullptr;
						if (!SUCCEEDED(D3D12SerializeVersionedRootSignature(&versionedRootSignatureDesc, &versionedRootSignatureBlob, &messageBlob)))
						{
							if (ErrorMessage != nullptr)
								*ErrorMessage = ReinterpretCast(cstr, messageBlob->GetBufferPointer());

							return false;
						}

						if (!SUCCEEDED(Device->CreateRootSignature(0, versionedRootSignatureBlob->GetBufferPointer(), versionedRootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(RootSignature))))
							return false;

						return true;
					}

					template<typename PipelineStateDescType>
					INLINE static bool CreatePipelineState(ID3D12Device5* Device, PipelineStateDescType* Desc, ID3D12PipelineState** PipelineState)
					{
						//https://microsoft.github.io/DirectX-Specs/d3d/DepthBoundsTest.html

						D3D12_PIPELINE_STATE_STREAM_DESC stream;
						stream.pPipelineStateSubobjectStream = Desc;
						stream.SizeInBytes = sizeof(PipelineStateDescType);

						return SUCCEEDED(Device->CreatePipelineState(&stream, IID_PPV_ARGS(PipelineState)));
					}

					INLINE static bool CompileShader(cstr Source, cstr Target, bool DebugMode, D3D12_SHADER_BYTECODE* ByteCode, cstr* ErrorMessage)
					{
						ID3DBlob* byteCodeBlob = nullptr;
						ID3DBlob* messageBlob = nullptr;

						uint32 flags = 0;
						flags |= D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY;

						if (DebugMode)
							flags |= D3DCOMPILE_DEBUG;

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

					INLINE static bool ReflectShaderConstants(const D3D12_SHADER_BYTECODE* Code, D3D12_SHADER_VARIABLE_DESC* ShaderVariableDescs, uint8 VariablesLength, uint8* Count)
					{
						ID3D12ShaderReflection* shaderReflection = nullptr;
						if (!SUCCEEDED(D3DReflect(Code->pShaderBytecode, Code->BytecodeLength, IID_PPV_ARGS(&shaderReflection))))
							return false;

						D3D12_SHADER_DESC shaderReflectionDesc = {};
						if (!SUCCEEDED(shaderReflection->GetDesc(&shaderReflectionDesc)))
							return false;

						for (uint8 i = 0; i < shaderReflectionDesc.ConstantBuffers; ++i)
						{
							ID3D12ShaderReflectionConstantBuffer* shaderReflectionConstantBuffer = shaderReflection->GetConstantBufferByIndex(i);
							if (shaderReflectionConstantBuffer == nullptr)
								return false;

							D3D12_SHADER_BUFFER_DESC shaderReflectionConstantBufferDesc = {};
							if (!SUCCEEDED(shaderReflectionConstantBuffer->GetDesc(&shaderReflectionConstantBufferDesc)))
								return false;

							for (uint8 j = 0; j < shaderReflectionConstantBufferDesc.Variables; ++j)
							{
								ID3D12ShaderReflectionVariable* shaderReflectionVariable = shaderReflectionConstantBuffer->GetVariableByIndex(j);
								if (shaderReflectionVariable == nullptr)
									return false;

								D3D12_SHADER_VARIABLE_DESC shaderReflectionVariableDesc = {};
								if (!SUCCEEDED(shaderReflectionVariable->GetDesc(&shaderReflectionVariableDesc)))
									return false;

								if (!BitwiseUtils::IsEnabled((D3D_SHADER_VARIABLE_FLAGS)shaderReflectionVariableDesc.uFlags, D3D_SVF_USED))
									continue;

								if (*Count >= VariablesLength)
									return false;

								ShaderVariableDescs[(*Count)++] = shaderReflectionVariableDesc;
							}
						}

						return ReleaseInstance(shaderReflection);
					}

					INLINE static bool SetObjectName(ID3D12Object* Object, cwstr Name)
					{
						return SUCCEEDED(Object->SetName(Name));
					}

					INLINE static bool MapResource(ID3D12Resource1* Resource, byte** Buffer)
					{
						return SUCCEEDED(Resource->Map(0, nullptr, ReinterpretCast(void**, Buffer)));
					}

					INLINE static bool UnmapResource(ID3D12Resource1* Resource)
					{
						Resource->Unmap(0, nullptr);

						return true;
					}

					INLINE static bool CreatePlacedResource(ID3D12Device5* Device, ID3D12Heap1* Heap, uint64 Offset, D3D12_RESOURCE_DIMENSION Type, uint64 Alignment, uint32 Width, uint32 Height, DXGI_FORMAT Format, D3D12_TEXTURE_LAYOUT Layout, D3D12_RESOURCE_FLAGS Flags, D3D12_RESOURCE_STATES State, ID3D12Resource1** Resource)
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

						return SUCCEEDED(Device->CreatePlacedResource(Heap, Offset, &resourceDesc, State, nullptr, IID_PPV_ARGS(Resource)));
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

					INLINE static bool GetSwapChainBackBuffers(IDXGISwapChain4* SwapChain, uint8 BackBufferCount, ID3D12Resource1** BackBuffers)
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

					INLINE static bool CreateDescriptorHeap(ID3D12Device5* Device, D3D12_DESCRIPTOR_HEAP_TYPE Type, uint32 Count, ID3D12DescriptorHeap** DescriptorHeap)
					{
						D3D12_DESCRIPTOR_HEAP_DESC desc = {};
						desc.Type = Type;
						desc.NumDescriptors = Count;

						return SUCCEEDED(Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(DescriptorHeap)));
					}

					INLINE static bool CreateConstantBufferView(ID3D12Device5* Device, ID3D12Resource1* Resource, D3D12_CPU_DESCRIPTOR_HANDLE Handle)
					{
						D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
						desc.BufferLocation = Resource->GetGPUVirtualAddress();
						desc.SizeInBytes = Resource->GetDesc().Width;

						Device->CreateConstantBufferView(&desc, Handle);

						return true;
					}

					INLINE static bool CreateProgramResourceView(ID3D12Device5* Device, ID3D12Resource1* Resource, D3D12_CPU_DESCRIPTOR_HANDLE Handle)
					{
						Device->CreateShaderResourceView(Resource, nullptr, Handle);

						return true;
					}

					INLINE static bool CreateUnorderedAccessView(ID3D12Device5* Device, ID3D12Resource1* Resource, D3D12_CPU_DESCRIPTOR_HANDLE Handle)
					{
						Device->CreateUnorderedAccessView(Resource, nullptr, nullptr, Handle);

						return true;
					}

					INLINE static bool CreateSampler(ID3D12Device5* Device, D3D12_CPU_DESCRIPTOR_HANDLE Handle)
					{
						D3D12_SAMPLER_DESC desc = {};

						Device->CreateSampler(&desc, Handle);

						return true;
					}

					INLINE static bool CreateRenderTargetView(ID3D12Device5* Device, ID3D12Resource1* Resource, D3D12_CPU_DESCRIPTOR_HANDLE Handle)
					{
						Device->CreateRenderTargetView(Resource, nullptr, Handle);

						return true;
					}

					INLINE static bool CreateDepthStencilView(ID3D12Device5* Device, ID3D12Resource1* Resource, D3D12_CPU_DESCRIPTOR_HANDLE Handle)
					{
						Device->CreateDepthStencilView(Resource, nullptr, Handle);

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

					INLINE static bool AddSetGraphicsRootSignature(ID3D12GraphicsCommandList4* CommandList, ID3D12RootSignature* RootSignature)
					{
						CommandList->SetGraphicsRootSignature(RootSignature);

						return true;
					}

					INLINE static bool AddSetComputeRootSignature(ID3D12GraphicsCommandList4* CommandList, ID3D12RootSignature* RootSignature)
					{
						CommandList->SetComputeRootSignature(RootSignature);

						return true;
					}

					INLINE static bool AddSetPipelineState(ID3D12GraphicsCommandList4* CommandList, ID3D12PipelineState* PipelineState)
					{
						CommandList->SetPipelineState(PipelineState);

						return true;
					}

					INLINE static bool AddTransitionResourceBarrier(ID3D12GraphicsCommandList4* CommandList, ID3D12Resource1* Resource, D3D12_RESOURCE_STATES BeforeState, D3D12_RESOURCE_STATES AfterState)
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

					INLINE static bool AddCopyBufferCommand(ID3D12GraphicsCommandList4* CommandList, ID3D12Resource1* Source, ID3D12Resource1* Destination, uint32 Size)
					{
						CommandList->CopyBufferRegion(Destination, 0, Source, 0, Size);

						return true;
					}

					INLINE static bool AddCopyTextureToBufferCommand(ID3D12GraphicsCommandList4* CommandList, ID3D12Resource1* Source, ID3D12Resource1* Destination)
					{
						D3D12_RESOURCE_DESC desc = Source->GetDesc();

						D3D12_TEXTURE_COPY_LOCATION srceLoc = {};
						srceLoc.pResource = Source;
						srceLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
						srceLoc.SubresourceIndex = 0;

						D3D12_TEXTURE_COPY_LOCATION destLoc = {};
						destLoc.pResource = Destination;
						destLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

						ID3D12Device5* device = nullptr;
						if (!SUCCEEDED(CommandList->GetDevice(IID_PPV_ARGS(&device))))
							return false;
						GetCopyableFootprint(device, Source, &destLoc.PlacedFootprint);
						ReleaseInstance(device);

						CommandList->CopyTextureRegion(&destLoc, 0, 0, 0, &srceLoc, nullptr);

						return true;
					}

					INLINE static bool AddCopyBufferToTextureCommand(ID3D12GraphicsCommandList4* CommandList, ID3D12Resource1* Source, ID3D12Resource1* Destination)
					{
						D3D12_RESOURCE_DESC desc = Destination->GetDesc();

						D3D12_TEXTURE_COPY_LOCATION srceLoc = {};
						srceLoc.pResource = Source;
						srceLoc.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

						ID3D12Device5* device = nullptr;
						if (!SUCCEEDED(CommandList->GetDevice(IID_PPV_ARGS(&device))))
							return false;
						GetCopyableFootprint(device, Destination, &srceLoc.PlacedFootprint);
						ReleaseInstance(device);

						D3D12_TEXTURE_COPY_LOCATION destLoc = {};
						destLoc.pResource = Destination;
						destLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
						destLoc.SubresourceIndex = 0;

						CommandList->CopyTextureRegion(&destLoc, 0, 0, 0, &srceLoc, nullptr);

						return true;
					}

					INLINE static bool AddSetViewportCommand(ID3D12GraphicsCommandList4* CommandList, const D3D12_VIEWPORT* Viewport)
					{
						CommandList->RSSetViewports(1, Viewport);

						D3D12_RECT rect = { 0, 0, LONG_MAX , LONG_MAX };
						CommandList->RSSetScissorRects(1, &rect);

						return true;
					}

					INLINE static bool AddClearRenderTargetCommand(ID3D12GraphicsCommandList4* CommandList, D3D12_CPU_DESCRIPTOR_HANDLE Handle, float32* Color)
					{
						CommandList->ClearRenderTargetView(Handle, Color, 0, nullptr);

						return true;
					}

					INLINE static bool AddClearDepthStencilCommand(ID3D12GraphicsCommandList4* CommandList, D3D12_CPU_DESCRIPTOR_HANDLE Handle, D3D12_CLEAR_FLAGS Flags, float32 Depth, uint8 Stencil)
					{
						CommandList->ClearDepthStencilView(Handle, Flags, Depth, Stencil, 0, nullptr);

						return true;
					}

					INLINE static bool AddSetVertexBufferCommand(ID3D12GraphicsCommandList4* CommandList, ID3D12Resource1* Resource, uint32 Size, uint32 Stride)
					{
						D3D12_VERTEX_BUFFER_VIEW view = {};
						view.BufferLocation = Resource->GetGPUVirtualAddress();
						view.SizeInBytes = Size;
						view.StrideInBytes = Stride;

						CommandList->IASetVertexBuffers(0, 1, &view);

						return true;
					}

					INLINE static bool AddSetIndexBufferCommand(ID3D12GraphicsCommandList4* CommandList, ID3D12Resource1* Resource, uint32 Size)
					{
						D3D12_INDEX_BUFFER_VIEW view = {};
						view.BufferLocation = Resource->GetGPUVirtualAddress();
						view.SizeInBytes = Size;
						view.Format = DXGI_FORMAT_R32_UINT;

						CommandList->IASetIndexBuffer(&view);

						return true;
					}

					INLINE static bool AddSetPrimitiveTopologyCommand(ID3D12GraphicsCommandList4* CommandList, D3D12_PRIMITIVE_TOPOLOGY Topology)
					{
						CommandList->IASetPrimitiveTopology(Topology);

						return true;
					}

					INLINE static bool AddDrawIndexedCommand(ID3D12GraphicsCommandList4* CommandList, uint32 IndexCount)
					{
						CommandList->DrawIndexedInstanced(IndexCount, 1, 0, 0, 0);

						return true;
					}

					INLINE static bool AddDrawCommand(ID3D12GraphicsCommandList4* CommandList, uint32 VertexCount)
					{
						CommandList->DrawInstanced(VertexCount, 1, 0, 0);

						return true;
					}

					INLINE static bool ExecuteCommandList(ID3D12CommandQueue* CommandQueue, ID3D12GraphicsCommandList4* CommandList)
					{
						if (!SUCCEEDED(CommandList->Close()))
							return false;

						CommandQueue->ExecuteCommandLists(1, ReinterpretCast(ID3D12CommandList**, &CommandList));

						//CommandQueue->Wait() ???

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

#undef DECLARE_PIPELINE_SUBOBJECT
				};
			}
		}
	}
}

#endif