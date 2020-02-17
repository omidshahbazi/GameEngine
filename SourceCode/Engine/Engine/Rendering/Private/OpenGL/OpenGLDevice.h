// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPEN_GL_DEVICE_H
#define OPEN_GL_DEVICE_H

#include <Containers\Map.h>
#include <Rendering\IDevice.h>
#include <Platform\PlatformMemory.h>

namespace Engine
{
	using namespace Platform;

	namespace Rendering
	{
		namespace Private
		{
			namespace OpenGL
			{
				class GLRenderContext;

				class RENDERING_API OpenGLDevice : public IDevice
				{
				private:
					struct RenderTargetHandles
					{
					public:
						TextureList Texture;
					};

					struct MeshBufferHandles
					{
					public:
						GPUBuffer::Handle VertexArrayObject;
						GPUBuffer::Handle VertexBufferObject;
						GPUBuffer::Handle ElementBufferObject;
					};

					typedef Map<uint32, MeshBufferHandles> MeshBuffersMap;
					typedef Map<Texture::Handle, RenderTargetHandles> RenderTargetMap;

				public:
					OpenGLDevice(void);
					~OpenGLDevice(void);

					bool Initialize(void) override;

					cstr GetVersion(void) override;
					cstr GetVendorName(void) override;
					cstr GetRendererName(void) override;
					cstr GetShadingLanguageVersion(void) override;

					RenderContext* CreateContext(PlatformWindow::WindowHandle Handle) override;
					bool DestroyContext(RenderContext* Context) override;
					bool SetContext(RenderContext* Context) override;

					bool SetViewport(const Vector2I& Position, const Vector2I& Size) override;

					bool SetClearColor(Color Color) override;

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
						SetStencilTestFunctionInternal(CullModes::Back, state.BackFaceStace.StencilTestFunction, state.BackFaceStace.StencilTestFunctionReference, state.BackFaceStace.StencilTestFunctionMask);
						SetStencilTestFunctionInternal(CullModes::Both, state.BothFaceState.StencilTestFunction, state.BothFaceState.StencilTestFunctionReference, state.BothFaceState.StencilTestFunctionMask);

						SetStencilMaskInternal(CullModes::Front, state.FrontFaceState.StencilMask);
						SetStencilMaskInternal(CullModes::Back, state.BackFaceStace.StencilMask);
						SetStencilMaskInternal(CullModes::Both, state.BothFaceState.StencilMask);

						SetStencilOperationInternal(CullModes::Front, state.FrontFaceState.StencilOperationStencilFailed, state.FrontFaceState.StencilOperationDepthFailed, state.FrontFaceState.StencilOperationDepthPassed);
						SetStencilOperationInternal(CullModes::Back, state.BackFaceStace.StencilOperationStencilFailed, state.BackFaceStace.StencilOperationDepthFailed, state.BackFaceStace.StencilOperationDepthPassed);
						SetStencilOperationInternal(CullModes::Both, state.BothFaceState.StencilOperationStencilFailed, state.BothFaceState.StencilOperationDepthFailed, state.BothFaceState.StencilOperationDepthPassed);

						SetBlendEquationInternal(state.BlendEquation);
						SetBlendFunctionInternal(state.BlendFunctionSourceFactor, state.BlendFunctionDestinationFactor);

						SetPolygonModeInternal(CullModes::Front, state.FrontFaceState.PolygonMode);
						SetPolygonModeInternal(CullModes::Back, state.BackFaceStace.PolygonMode);
						SetPolygonModeInternal(CullModes::Both, state.BothFaceState.PolygonMode);

						m_LastProgram = 0;
						m_LastMeshBuffer = 0;
						m_LastFrameBuffer = 0;

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
						SetStencilTestFunction(CullModes::Back, State.BackFaceStace.StencilTestFunction, State.BackFaceStace.StencilTestFunctionReference, State.BackFaceStace.StencilTestFunctionMask);
						SetStencilTestFunction(CullModes::Both, State.BothFaceState.StencilTestFunction, State.BothFaceState.StencilTestFunctionReference, State.BothFaceState.StencilTestFunctionMask);

						SetStencilMask(CullModes::Front, State.FrontFaceState.StencilMask);
						SetStencilMask(CullModes::Back, State.BackFaceStace.StencilMask);
						SetStencilMask(CullModes::Both, State.BothFaceState.StencilMask);

						SetStencilOperation(CullModes::Front, State.FrontFaceState.StencilOperationStencilFailed, State.FrontFaceState.StencilOperationDepthFailed, State.FrontFaceState.StencilOperationDepthPassed);
						SetStencilOperation(CullModes::Back, State.BackFaceStace.StencilOperationStencilFailed, State.BackFaceStace.StencilOperationDepthFailed, State.BackFaceStace.StencilOperationDepthPassed);
						SetStencilOperation(CullModes::Both, State.BothFaceState.StencilOperationStencilFailed, State.BothFaceState.StencilOperationDepthFailed, State.BothFaceState.StencilOperationDepthPassed);

						SetBlendEquation(State.BlendEquation);
						SetBlendFunction(State.BlendFunctionSourceFactor, State.BlendFunctionDestinationFactor);

						SetPolygonMode(CullModes::Front, State.FrontFaceState.PolygonMode);
						SetPolygonMode(CullModes::Back, State.BackFaceStace.PolygonMode);
						SetPolygonMode(CullModes::Both, State.BothFaceState.PolygonMode);
					}

					bool CreateProgram(cstr VertexShader, cstr FragmentShader, Program::Handle& Handle) override;
					bool DestroyProgram(Program::Handle Handle) override;
					bool BindProgram(Program::Handle Handle) override;
					bool QueryProgramActiveConstants(Program::Handle Handle, Program::ConstantDataList& Constants) override;
					bool GetProgramConstantHandle(Program::Handle Handle, const String& Name, Program::ConstantHandle& ConstantHandle) override;
					bool SetProgramFloat32(Program::ConstantHandle Handle, float32 Value) override;
					bool SetProgramVector2(Program::ConstantHandle Handle, const Vector2F& Value) override;
					bool SetProgramVector3(Program::ConstantHandle Handle, const Vector3F& Value) override;
					bool SetProgramVector4(Program::ConstantHandle Handle, const Vector4F& Value) override;
					bool SetProgramMatrix4(Program::ConstantHandle Handle, const Matrix4F& Value) override;
					bool SetProgramTexture(Program::ConstantHandle Handle, Texture::Handle Value) override;

					bool CreateTexture2D(const byte* Data, uint32 Width, uint32 Height, Texture::Formats Format, Texture::Handle& Handle) override;
					bool DestroyTexture(Texture::Handle Handle) override;
					bool BindTexture2D(Program::Handle Handle) override;
					bool SetTexture2DVerticalWrapping(Texture::Handle Handle, Texture::WrapModes Mode) override;
					bool SetTexture2DHorizontalWrapping(Texture::Handle Handle, Texture::WrapModes Mode) override;
					bool SetTexture2DMinifyFilter(Texture::Handle Handle, Texture::MinifyFilters Filter) override;
					bool SetTexture2DMagnifyFilter(Texture::Handle Handle, Texture::MagnfyFilters Filter) override;
					bool GenerateMipMap(Texture::Handle Handle) override;

					bool CreateRenderTarget(const RenderTargetInfo* Info, RenderTarget::Handle& Handle, TextureList& Textures) override;
					bool DestroyRenderTarget(RenderTarget::Handle Handle) override;
					bool BindRenderTarget(RenderTarget::Handle Handle) override;

					bool CreateMesh(const SubMeshInfo* Info, BufferUsages Usage, GPUBuffer::Handle& Handle) override;
					bool DestroyMesh(GPUBuffer::Handle Handle) override;
					bool BindMesh(GPUBuffer::Handle Handle) override;

					bool Clear(ClearFlags Flags) override;

					bool DrawIndexed(Mesh::SubMesh::PolygonTypes PolygonType, uint32 IndexCount) override;
					bool DrawArray(Mesh::SubMesh::PolygonTypes PolygonType, uint32 VertexCount)  override;

					bool SwapBuffers(void) override;

					bool SetDebugCallback(DebugProcedureType Callback) override;

					DebugProcedureType GetDebugCallback(void) const
					{
						return m_Callback;
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
					bool m_IsInitialized;

					GLRenderContext* m_BaseContext;
					GLRenderContext* m_CurrentContext;

					Color m_ClearColor;
					State m_State;

					Program::Handle m_LastProgram;
					GPUBuffer::Handle m_LastMeshBuffer;
					GPUBuffer::Handle m_LastFrameBuffer;

					RenderTargetMap m_RenderTargets;

					MeshBuffersMap m_MeshBuffers;
					uint32 m_LastMeshNumber;
					uint8 m_LastActiveTextureUnitIndex;

					DebugProcedureType m_Callback;
				};
			}
		}
	}
}

#endif