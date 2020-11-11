// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DIRECTX12_DEVICE_H
#define DIRECTX12_DEVICE_H

#include <Containers\Map.h>
#include <Rendering\IDevice.h>
#include <dxgi1_6.h>
#include <d3d12.h>

namespace Engine
{
	using namespace Platform;

	namespace Rendering
	{
		namespace Private
		{
			namespace DirectX12
			{
				class DirectX12RenderContext;

				class RENDERING_API DirectX12Device : public IDevice
				{
				private:
					struct ShaderHandles
					{
					public:
						D3D12_SHADER_BYTECODE VertexShader;
						D3D12_SHADER_BYTECODE FragmentShader;
					};

					struct RenderTargetHandles
					{
					public:
						TextureList Texture;
					};

					struct MeshBufferInfo
					{
					public:
						SubMesh::Handle VertexBufferObject;
						SubMesh::Handle IndexBufferObject;
						SubMesh::VertexLayouts Layout;
					};

					typedef Vector<DirectX12RenderContext*> RenderContextList;
					typedef Map<Shader::Handle, ShaderHandles> ShaderMap;
					typedef Map<Texture::Handle, RenderTargetHandles> RenderTargetMap;
					typedef Map<SubMesh::Handle, MeshBufferInfo> MeshBuffersMap;

				public:
					DirectX12Device(void);
					~DirectX12Device(void);

					bool Initialize(void) override;

					cstr GetVersion(void) override;
					cstr GetVendorName(void) override;
					cstr GetRendererName(void) override;
					cstr GetShadingLanguageVersion(void) override;

					RenderContext* CreateContext(PlatformWindow::WindowHandle Handle) override;
					bool DestroyContext(RenderContext* Context) override;
					bool SetContext(RenderContext* Context) override;
					RenderContext* GetContext(void) override;

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

					bool ResetState(void) override
					{
						State state;

						SetFaceOrderInternal(state.FaceOrder);
						SetCullModeInternal(state.CullMode);
						SetDepthTestFunctionInternal(state.DepthTestFunction);

						SetStencilTestFunctionInternal(CullModes::Front, state.FrontFaceState.StencilTestFunction, state.FrontFaceState.StencilTestFunctionReference, state.FrontFaceState.StencilTestFunctionMask);
						SetStencilTestFunctionInternal(CullModes::Back, state.BackFaceState.StencilTestFunction, state.BackFaceState.StencilTestFunctionReference, state.BackFaceState.StencilTestFunctionMask);
						SetStencilTestFunctionInternal(CullModes::Both, state.BothFaceState.StencilTestFunction, state.BothFaceState.StencilTestFunctionReference, state.BothFaceState.StencilTestFunctionMask);

						SetStencilMaskInternal(CullModes::Front, state.FrontFaceState.StencilMask);
						SetStencilMaskInternal(CullModes::Back, state.BackFaceState.StencilMask);
						SetStencilMaskInternal(CullModes::Both, state.BothFaceState.StencilMask);

						SetStencilOperationInternal(CullModes::Front, state.FrontFaceState.StencilOperationStencilFailed, state.FrontFaceState.StencilOperationDepthFailed, state.FrontFaceState.StencilOperationDepthPassed);
						SetStencilOperationInternal(CullModes::Back, state.BackFaceState.StencilOperationStencilFailed, state.BackFaceState.StencilOperationDepthFailed, state.BackFaceState.StencilOperationDepthPassed);
						SetStencilOperationInternal(CullModes::Both, state.BothFaceState.StencilOperationStencilFailed, state.BothFaceState.StencilOperationDepthFailed, state.BothFaceState.StencilOperationDepthPassed);

						SetBlendEquationInternal(state.BlendEquation);
						SetBlendFunctionInternal(state.BlendFunctionSourceFactor, state.BlendFunctionDestinationFactor);

						SetPolygonModeInternal(CullModes::Front, state.FrontFaceState.PolygonMode);
						SetPolygonModeInternal(CullModes::Back, state.BackFaceState.PolygonMode);
						SetPolygonModeInternal(CullModes::Both, state.BothFaceState.PolygonMode);

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
						SetStencilTestFunction(CullModes::Both, State.BothFaceState.StencilTestFunction, State.BothFaceState.StencilTestFunctionReference, State.BothFaceState.StencilTestFunctionMask);

						SetStencilMask(CullModes::Front, State.FrontFaceState.StencilMask);
						SetStencilMask(CullModes::Back, State.BackFaceState.StencilMask);
						SetStencilMask(CullModes::Both, State.BothFaceState.StencilMask);

						SetStencilOperation(CullModes::Front, State.FrontFaceState.StencilOperationStencilFailed, State.FrontFaceState.StencilOperationDepthFailed, State.FrontFaceState.StencilOperationDepthPassed);
						SetStencilOperation(CullModes::Back, State.BackFaceState.StencilOperationStencilFailed, State.BackFaceState.StencilOperationDepthFailed, State.BackFaceState.StencilOperationDepthPassed);
						SetStencilOperation(CullModes::Both, State.BothFaceState.StencilOperationStencilFailed, State.BothFaceState.StencilOperationDepthFailed, State.BothFaceState.StencilOperationDepthPassed);

						SetBlendEquation(State.BlendEquation);
						SetBlendFunction(State.BlendFunctionSourceFactor, State.BlendFunctionDestinationFactor);

						SetPolygonMode(CullModes::Front, State.FrontFaceState.PolygonMode);
						SetPolygonMode(CullModes::Back, State.BackFaceState.PolygonMode);
						SetPolygonMode(CullModes::Both, State.BothFaceState.PolygonMode);
					}

					bool CreateBuffer(GPUBuffer::Handle& Handle) override;
					bool DestroyBuffer(GPUBuffer::Handle Handle) override;
					bool BindBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type) override;
					bool AttachBufferData(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, uint32 Size, const void* Data) override;
					bool AttachBufferData(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, uint32 Size, Texture::Handle TextureHandle, Texture::Types TextureType, Texture::Formats TextureFormat, uint32 Level) override;
					bool ReadBufferData(GPUBuffer::Handle Handle, GPUBuffer::Types Type, Texture::Handle TextureHandle, Texture::Types TextureType, uint32 Width, uint32 Height, Texture::Formats TextureFormat) override;
					bool LockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer) override;
					bool UnlockBuffer(GPUBuffer::Types Type) override;

					bool CreateShader(const Shaders* Shaders, Shader::Handle& Handle, cstr* ErrorMessage) override;
					bool DestroyShader(Shader::Handle Handle) override;
					bool BindShader(Shader::Handle Handle) override;
					bool QueryShaderActiveConstants(Shader::Handle Handle, Shader::ConstantDataList& Constants) override;
					bool GetShaderConstantHandle(Shader::Handle Handle, const String& Name, Shader::ConstantHandle& ConstantHandle) override;
					bool SetShaderFloat32(Shader::ConstantHandle Handle, float32 Value) override;
					bool SetShaderColor(Shader::ConstantHandle Handle, const ColorUI8& Value) override;
					bool SetShaderVector2(Shader::ConstantHandle Handle, const Vector2F& Value) override;
					bool SetShaderVector3(Shader::ConstantHandle Handle, const Vector3F& Value) override;
					bool SetShaderVector4(Shader::ConstantHandle Handle, const Vector4F& Value) override;
					bool SetShaderMatrix4(Shader::ConstantHandle Handle, const Matrix4F& Value) override;
					bool SetShaderTexture(Shader::ConstantHandle Handle, Texture::Types Type, Texture::Handle Value) override;

					bool CreateTexture(const TextureInfo* Info, Texture::Handle& Handle) override;
					bool DestroyTexture(Texture::Handle Handle) override;
					bool BindTexture(Texture::Handle Handle, Texture::Types Type)override;
					bool SetTextureVerticalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode) override;
					bool SetTextureHorizontalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode) override;
					bool SetTextureMinifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MinifyFilters Filter) override;
					bool SetTextureMagnifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MagnfyFilters Filter)override;
					bool GenerateTextureMipMap(Texture::Handle Handle, Texture::Types Type) override;

					bool CreateRenderTarget(const RenderTargetInfo* Info, RenderTarget::Handle& Handle, TextureList& Textures) override;
					bool DestroyRenderTarget(RenderTarget::Handle Handle) override;
					bool BindRenderTarget(RenderTarget::Handle Handle) override;

					bool CreateMesh(const SubMeshInfo* Info, GPUBuffer::Usages Usage, SubMesh::Handle& Handle) override;
					bool GetMeshVertexBuffer(SubMesh::Handle Handle, GPUBuffer::Handle& BufferHandle) override;
					bool GetMeshElementBuffer(SubMesh::Handle Handle, GPUBuffer::Handle& BufferHandle) override;
					bool DestroyMesh(SubMesh::Handle Handle) override;
					bool CreateVertexArray(const MeshBufferInfo& Info, NativeType::Handle& Handle);
					bool DestroyVertexArray(NativeType::Handle Handle);
					bool BindMesh(SubMesh::Handle Handle) override;

					bool Clear(ClearFlags Flags) override;

					bool DrawIndexed(SubMesh::PolygonTypes PolygonType, uint32 IndexCount) override;
					bool DrawArray(SubMesh::PolygonTypes PolygonType, uint32 VertexCount)  override;

					bool SwapBuffers(void) override;

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

				private:
					bool m_Initialized;

					IDXGIFactory5* m_Factory;
					IDXGIAdapter3* m_Adapter;
					DXGI_ADAPTER_DESC2 m_AdapterDesc;
					ID3D12Device5* m_Device;
					ID3D12InfoQueue* m_InfoQueue;
					ID3D12CommandQueue* m_CommandQueue;
					ID3D12CommandAllocator** m_CommandAllocators;

					RenderContextList m_Contexts;
					DirectX12RenderContext* m_CurrentContext;

					ColorUI8 m_ClearColor;
					State m_State;

					ShaderMap m_Shaders;

					//Shader::Handle m_LastShader;
					//RenderTarget::Handle m_LastFrameBuffer;

					RenderTargetMap m_RenderTargets;

					//MeshBuffersMap m_MeshBuffers;
					//uint32 m_LastMeshNumber;
					//uint8 m_LastActiveTextureUnitIndex;

					DebugFunction m_DebugCallback;
				};
			}
		}
	}
}

#endif