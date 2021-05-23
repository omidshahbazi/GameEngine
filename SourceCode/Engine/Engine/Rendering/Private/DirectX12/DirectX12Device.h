// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DIRECTX12_DEVICE_H
#define DIRECTX12_DEVICE_H

#include <Containers\Map.h>
#include <Rendering\IDevice.h>
#include <Rendering\Private\DirectX12\MemoryManager.h>
#include <Rendering\Private\DirectX12\DescriptorViewAllocator.h>

namespace Engine
{
	using namespace Platform;

	namespace Rendering
	{
		namespace Private
		{
			namespace DirectX12
			{
				class HeapAllocator;

				class RENDERING_API DirectX12Device : public IDevice
				{
				private:
					struct CommandSet
					{
					public:
						ID3D12CommandQueue* Queue;
						ID3D12CommandAllocator* Allocator;
						ID3D12GraphicsCommandList4* List;

						ID3D12Fence* Fence;
						uint64 FenceValue;
						HANDLE FenceEvent;
					};

					struct ProgramInfos : public CompiledShaders
					{
					public:
						ID3D12RootSignature* RootSignature;
						Map<uint32, ID3D12PipelineState*> Pipelines;
					};

					struct ResourceInfo
					{
					public:
						ID3D12Resource1* Resource;
						DescriptorViewAllocator::ViewHandle View;
						D3D12_RESOURCE_STATES PrevState;
					};

					struct BufferInfo : public ResourceInfo
					{
					public:
						uint32 Size;
						uint32 Stride;
					};

					struct BoundBuffersInfo
					{
					public:
						ResourceInfo* Resource;
						BufferInfo Buffer;
					};

					struct ViewInfo : public ResourceInfo
					{
					public:
						RenderTarget::AttachmentPoints Point;
					};

					struct RenderTargetInfos
					{
					public:
						typedef Vector<ViewInfo> ViewList;

						ViewList Views;
					};

					struct MeshBufferInfo
					{
					public:
						BufferInfo VertexBuffer;
						BufferInfo IndexBuffer;
						SubMesh::VertexLayouts Layout;
					};

					class RenderContextInfo
					{
					public:
						static const uint8 MAX_BACK_BUFFER_COUNT = 3;
						static const uint8 RENDER_TARGET_VIEW_INDEX = 0;
						static const uint8 DEPTH_STENCIL_VIEW_INDEX = 1;

					public:
						INLINE ViewInfo* GetRenderTargetViews(void)
						{
							return &Views[CurrentBackBufferIndex][RENDER_TARGET_VIEW_INDEX];
						}

						INLINE ViewInfo* GetDepthStencilViews(void)
						{
							return &Views[CurrentBackBufferIndex][DEPTH_STENCIL_VIEW_INDEX];
						}

						IDXGISwapChain4* SwapChain;

						ViewInfo Views[MAX_BACK_BUFFER_COUNT][2];
						uint8 BackBufferCount;
						uint8 CurrentBackBufferIndex;
					};

					typedef Map<RenderContext::Handle, RenderContextInfo*> RenderContextMap;

				public:
					DirectX12Device(void);
					~DirectX12Device(void);

					bool Initialize(void) override;

					cstr GetVersion(void) override;
					cstr GetVendorName(void) override;
					cstr GetRendererName(void) override;
					cstr GetShadingLanguageVersion(void) override;

					bool CreateContext(PlatformWindow::WindowHandle WindowHandle, RenderContext::Handle& Handle) override;
					bool DestroyContext(RenderContext::Handle Handle) override;
					bool SetContext(RenderContext::Handle Handle) override;

					bool SetViewport(const Vector2I& Position, const Vector2I& Size) override;

					bool SetClearColor(const ColorUI8& Color) override;

					bool SetFaceOrder(FaceOrders Order) override;

					bool SetCullMode(CullModes Mode) override;

					bool SetDepthTestFunction(TestFunctions Function) override;

					bool SetStencilTestFunction(CullModes CullMode, TestFunctions Function, int32 Reference, uint32 Mask) override;

					bool SetStencilMask(CullModes CullMode, uint32 Mask) override;

					bool SetStencilOperation(CullModes CullMode, StencilOperations StencilFailed, StencilOperations DepthFailed, StencilOperations DepthPassed) override;

					bool SetBlendEquation(BlendEquations Equation) override;

					bool SetBlendFunction(BlendFunctions SourceFactor, BlendFunctions DestinationFactor) override;

					bool SetPolygonMode(CullModes CullMode, PolygonModes PolygonMode) override;

					bool SetResourceName(NativeType::Handle Handle, ResourceTypes Type, cwstr Name) override;

					bool ResetState(void) override
					{
						State state;

						SetFaceOrderInternal(state.FaceOrder);
						SetCullModeInternal(state.CullMode);
						SetDepthTestFunctionInternal(state.DepthTestFunction);

						SetStencilTestFunctionInternal(CullModes::Front, state.FrontFaceState.StencilTestFunction, state.FrontFaceState.StencilTestFunctionReference, state.FrontFaceState.StencilTestFunctionMask);
						SetStencilTestFunctionInternal(CullModes::Back, state.BackFaceState.StencilTestFunction, state.BackFaceState.StencilTestFunctionReference, state.BackFaceState.StencilTestFunctionMask);

						SetStencilMaskInternal(CullModes::Front, state.FrontFaceState.StencilMask);
						SetStencilMaskInternal(CullModes::Back, state.BackFaceState.StencilMask);

						SetStencilOperationInternal(CullModes::Front, state.FrontFaceState.StencilOperationStencilFailed, state.FrontFaceState.StencilOperationDepthFailed, state.FrontFaceState.StencilOperationDepthPassed);
						SetStencilOperationInternal(CullModes::Back, state.BackFaceState.StencilOperationStencilFailed, state.BackFaceState.StencilOperationDepthFailed, state.BackFaceState.StencilOperationDepthPassed);

						SetBlendEquationInternal(state.BlendEquation);
						SetBlendFunctionInternal(state.BlendFunctionSourceFactor, state.BlendFunctionDestinationFactor);

						SetPolygonModeInternal(CullModes::Front, state.FrontFaceState.PolygonMode);
						SetPolygonModeInternal(CullModes::Back, state.BackFaceState.PolygonMode);

						//m_LastShader = 0;
						//m_LastFrameBuffer = 0;

						return true;
					}

					const State& GetState(void) const override
					{
						return m_State;
					}
					void SetState(const State& State) override
					{
						SetFaceOrder(State.FaceOrder);
						SetCullMode(State.CullMode);
						SetDepthTestFunction(State.DepthTestFunction);

						SetStencilTestFunction(CullModes::Front, State.FrontFaceState.StencilTestFunction, State.FrontFaceState.StencilTestFunctionReference, State.FrontFaceState.StencilTestFunctionMask);
						SetStencilTestFunction(CullModes::Back, State.BackFaceState.StencilTestFunction, State.BackFaceState.StencilTestFunctionReference, State.BackFaceState.StencilTestFunctionMask);

						SetStencilMask(CullModes::Front, State.FrontFaceState.StencilMask);
						SetStencilMask(CullModes::Back, State.BackFaceState.StencilMask);

						SetStencilOperation(CullModes::Front, State.FrontFaceState.StencilOperationStencilFailed, State.FrontFaceState.StencilOperationDepthFailed, State.FrontFaceState.StencilOperationDepthPassed);
						SetStencilOperation(CullModes::Back, State.BackFaceState.StencilOperationStencilFailed, State.BackFaceState.StencilOperationDepthFailed, State.BackFaceState.StencilOperationDepthPassed);

						SetBlendEquation(State.BlendEquation);
						SetBlendFunction(State.BlendFunctionSourceFactor, State.BlendFunctionDestinationFactor);

						SetPolygonMode(CullModes::Front, State.FrontFaceState.PolygonMode);
						SetPolygonMode(CullModes::Back, State.BackFaceState.PolygonMode);
					}

					bool CreateBuffer(GPUBuffer::Handle& Handle) override;
					bool DestroyBuffer(GPUBuffer::Handle Handle) override;
					bool BindBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type) override;
					bool InitializeConstantBuffer(GPUBuffer::Handle Handle, const byte* Data, uint32 Size) override;
					bool CopyFromVertexToBuffer(GPUBuffer::Handle Handle, SubMesh::Handle FromMeshHandle, uint32 Size) override;
					bool CopyFromBufferToVertex(GPUBuffer::Handle Handle, Texture::Handle ToMeshHandle, uint32 Size) override;
					bool CopyFromIndexToBuffer(GPUBuffer::Handle Handle, SubMesh::Handle FromMeshHandle, uint32 Size) override;
					bool CopyFromBufferToIndex(GPUBuffer::Handle Handle, Texture::Handle ToMeshHandle, uint32 Size) override;
					bool CopyFromTextureToBuffer(GPUBuffer::Handle Handle, Texture::Handle FromTextureHandle, uint32 Size, Texture::Types TextureType, Formats TextureFormat, uint32 Level) override;
					bool CopyFromBufferToTexture(GPUBuffer::Handle Handle, Texture::Handle ToTextureHandle, Texture::Types TextureType, uint32 Width, uint32 Height, Formats TextureFormat) override;
					bool LockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer) override;
					bool UnlockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type) override;

					static bool CompileProgramAPI(const Shaders* Shaders, CompiledShaders* CompiledShaders, cstr* ErrorMessage);

					bool CompileProgram(const Shaders* Shaders, CompiledShaders* CompiledShaders, cstr* ErrorMessage) override;
					bool CreateProgram(const CompiledShaders* Shaders, Program::Handle& Handle, cstr* ErrorMessage) override;
					bool DestroyProgram(Program::Handle Handle) override;
					bool BindProgram(Program::Handle Handle) override;
					bool QueryProgramActiveConstants(Program::Handle Handle, Program::ConstantDataList& Constants) override;
					bool SetProgramConstantBuffer(Program::ConstantHandle Handle, ConstantBuffer::Handle Value) override;
					bool SetProgramTexture(Program::ConstantHandle Handle, Texture::Types Type, Texture::Handle Value) override;

					bool CreateTexture(const TextureInfo* Info, Texture::Handle& Handle) override;
					bool DestroyTexture(Texture::Handle Handle) override;
					bool BindTexture(Texture::Handle Handle, Texture::Types Type)override;
					bool SetTextureVerticalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode) override;
					bool SetTextureHorizontalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode) override;
					bool SetTextureMinifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MinifyFilters Filter) override;
					bool SetTextureMagnifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MagnfyFilters Filter) override;
					bool GenerateTextureMipMap(Texture::Handle Handle, Texture::Types Type) override;

					bool CreateRenderTarget(const RenderTargetInfo* Info, RenderTarget::Handle& Handle, TextureList& Textures) override;
					bool DestroyRenderTarget(RenderTarget::Handle Handle) override;
					bool BindRenderTarget(RenderTarget::Handle Handle) override;

					bool CreateMesh(const SubMeshInfo* Info, SubMesh::Handle& Handle) override;
					bool DestroyMesh(SubMesh::Handle Handle) override;
					bool BindMesh(SubMesh::Handle Handle) override;

					bool Clear(ClearFlags Flags) override;

					bool DrawIndexed(SubMesh::PolygonTypes PolygonType, uint32 IndexCount) override;
					bool DrawArray(SubMesh::PolygonTypes PolygonType, uint32 VertexCount) override;

					bool BeginExecute(void) override;
					bool EndExecute(void) override;

					bool SwapBuffers(void) override;

					bool BeginEvent(cwstr Label) override;
					bool EndEvent(void) override;
					bool SetMarker(cwstr Label) override;

					bool SetDebugCallback(DebugFunction Callback) override;

					DebugFunction GetDebugCallback(void) const
					{
						return m_DebugCallback;
					}

				private:
					bool SetFaceOrderInternal(FaceOrders Order);

					bool SetCullModeInternal(CullModes Mode);

					bool SetDepthTestFunctionInternal(TestFunctions Function);

					bool SetStencilTestFunctionInternal(CullModes CullMode, TestFunctions Function, int32 Reference, uint32 Mask);

					bool SetStencilMaskInternal(CullModes CullMode, uint32 Mask);

					bool SetStencilOperationInternal(CullModes CullMode, StencilOperations StencilFailed, StencilOperations DepthFailed, StencilOperations DepthPassed);

					bool SetBlendEquationInternal(BlendEquations Equation);

					bool SetBlendFunctionInternal(BlendFunctions SourceFactor, BlendFunctions DestinationFactor);

					bool SetPolygonModeInternal(CullModes CullMode, PolygonModes PolygonMode);

					bool AddTransitionResourceBarrier(CommandSet& Set, ResourceInfo* Info, D3D12_RESOURCE_STATES AfterState);

					bool CreateBuffer(uint32 Size, BufferInfo* Buffer);

					bool CreateCommandSet(CommandSet& Set, D3D12_COMMAND_LIST_TYPE Type);

					bool DestroyCommandSet(CommandSet& Set);

					bool ExecuteCommands(CommandSet& Set);

					bool CopyBuffer(GPUBuffer::Types Type, ResourceInfo* Source, bool SourceIsABuffer, ResourceInfo* Destination, bool DestinationIsABuffer);

					void FillGraphicsPipelineState(const IDevice::State& State, DirectX12Wrapper::PipelineStateObject::GraphicsPipelineStateDesc& Desc);

					uint32 GetStateHash(void);

				private:
					bool m_Initialized;

					IDXGIFactory5* m_Factory;
					IDXGIAdapter3* m_Adapter;
					DXGI_ADAPTER_DESC2 m_AdapterDesc;
					ID3D12Device5* m_Device;
					ID3D12InfoQueue* m_InfoQueue;
					CommandSet m_CopyCommandSet;
					CommandSet m_RenderCommandSet;

					MemoryManager m_MemoryManager;
					DescriptorViewAllocator m_RenderTargetViewAllocator;
					DescriptorViewAllocator m_DepthStencilViewAllocator;
					DescriptorViewAllocator m_ResourceViewAllocator;
					BufferInfo m_UploadBuffer;

					ResourceInfo m_DefaultTexture;

					RenderContextMap m_Contexts;
					RenderContext::Handle m_CurrentContextHandle;
					RenderContextInfo* m_CurrentContext;

					RenderTargetInfos* m_CurrentRenderTarget;

					ViewInfo* m_CurrentRenderTargetViews[(uint8)RenderTarget::AttachmentPoints::Color8 - (uint8)RenderTarget::AttachmentPoints::Color0];
					uint8 m_CurrentRenderTargetViewCount;
					ViewInfo* m_CurrentDepthStencilView;

					D3D12_VIEWPORT m_Viewport;
					ColorUI8 m_ClearColor;
					State m_State;

					D3D12_INPUT_ELEMENT_DESC* m_InputLayout;
					uint8 m_InputLayoutCount;

					DebugFunction m_DebugCallback;
				};
			}
		}
	}
}

#endif