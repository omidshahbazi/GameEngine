// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef OPEN_GL_DEVICE_H
#define OPEN_GL_DEVICE_H

#include <Containers\Map.h>
#include <Rendering\IDevice.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace OpenGL
			{
				class OpenGLDevice : public IDevice
				{
				private:
					struct MeshBufferHandles
					{
					public:
						GPUBuffer::Handle VertexArrayObject;
						GPUBuffer::Handle VertexBufferObject;
						GPUBuffer::Handle ElementBufferObject;
					};

					typedef Map<uint32, MeshBufferHandles> MeshBuffersMap;

				public:
					OpenGLDevice(void);
					~OpenGLDevice(void);

					bool Initialize(void) override;

					uint8 GetSampleCount(void) const override
					{
						return m_SampleCount;
					}
					void SetSampleCount(uint8 Count) override;

					bool GetForwardCompatible(void) const override
					{
						return m_ForwardCompatible;
					}
					void SetForwardCompatible(bool Value)override;

					Color GetClearColor(void) const override
					{
						return m_ClearColor;
					}
					void SetClearColor(Color Color) override;

					ClearFlags GetClearFlags(void) const override
					{
						return m_ClearFlags;
					}
					void SetClearFlags(ClearFlags Flags) override;

					FaceOrders GetFaceOrder(void) const override
					{
						return m_FaceOrder;
					}
					void SetFaceOrder(FaceOrders Order) override;

					CullModes GetCullMode(void) const override
					{
						return m_CullMode;
					}
					void SetCullMode(CullModes Mode) override;

					TestFunctions GetDepthTestFunction(void) const override
					{
						return m_DepthTestFunction;
					}
					void SetDepthTestFunction(TestFunctions Function) override;

					void GetStencilTestFunction(TestFunctions &Function, int32 &Reference, uint32 &Mask) const override
					{
						Function = m_StencilTestFunction;
						Reference = m_StencilTestFunctionReference;
						Mask = m_StencilTestFunctionMask;
					}
					void SetStencilTestFunction(TestFunctions Function, int32 Reference, uint32 Mask) override;

					uint32 GetStencilMask(void) const override
					{
						return m_StencilMask;
					}
					void SetStencilMask(uint32 Mask) override;

					void GetStencilOperation(StencilOperations &StencilFailed, StencilOperations &DepthFailed, StencilOperations &DepthPassed) const override
					{
						StencilFailed = m_StencilOperationStencilFailed;
						DepthFailed = m_StencilOperationDepthFailed;
						DepthPassed = m_StencilOperationDepthPassed;
					}
					void SetStencilOperation(StencilOperations StencilFailed, StencilOperations DepthFailed, StencilOperations DepthPassed) override;

					void GetBlendFunction(BlendFunctions &SourceFactor, BlendFunctions &DestinationFactor) const override
					{
						SourceFactor = m_BlendFunctionSourceFactor;
						DestinationFactor = m_BlendFunctionDestinationFactor;
					}
					void SetBlendFunction(BlendFunctions SourceFactor, BlendFunctions DestinationFactor) override;

					void GetPolygonMode(CullModes &CullMode, PolygonModes &PolygonMode) const override
					{
						CullMode = m_PolygonModeCullMode;
						PolygonMode = m_PolygonMode;
					}
					void SetPolygonMode(CullModes CullMode, PolygonModes PolygonMode) override;

					bool CreateProgram(cstr VertexShader, cstr FragmentShader, Program::Handle &Handle) override;
					bool DestroyProgram(Program::Handle Handle) override;
					bool BindProgram(Program::Handle Handle) override;
					bool QueryProgramActiveConstants(Program::Handle Handle, Program::ConstantDataList &Constants) override;
					bool GetProgramConstantHandle(Program::Handle Handle, const String &Name, Program::ConstantHandle &ConstantHandle) override;
					bool SetProgramFloat32(Program::ConstantHandle Handle, float32 Value) override;
					bool SetProgramVector2(Program::ConstantHandle Handle, const Vector2F &Value) override;
					bool SetProgramVector3(Program::ConstantHandle Handle, const Vector3F &Value) override;
					bool SetProgramVector4(Program::ConstantHandle Handle, const Vector4F &Value) override;
					bool SetProgramMatrix4(Program::ConstantHandle Handle, const Matrix4F &Value) override;
					bool SetProgramTexture(Program::ConstantHandle Handle, Texture::Handle Value) override;

					bool CreateTexture2D(const byte *Data, uint32 Width, uint32 Height, uint8 ComponentCount, TextureFormats Format, Texture::Handle &Handle) override;
					bool DestroyTexture(Texture::Handle Handle) override;
					bool BindTexture2D(Program::Handle Handle) override;
					bool SetTexture2DVerticalWrapping(Texture::Handle Handle, TextureWrapModes Mode) override;
					bool SetTexture2DHorizontalWrapping(Texture::Handle Handle, TextureWrapModes Mode) override;
					bool SetTexture2DMinifyFilter(Texture::Handle Handle, MinifyFilters Filter) override;
					bool SetTexture2DMagnifyFilter(Texture::Handle Handle, MagnfyFilters Filter) override;

					bool CreateMesh(const SubMeshInfo *Info, BufferUsages Usage, GPUBuffer::Handle &Handle) override;
					bool DestroyMesh(GPUBuffer::Handle Handle) override;
					bool BindBuffer(GPUBuffer::Handle Handle) override;

					bool CreateWindow(uint16 Width, uint16 Height, cstr Title, Window::Handle &Handle) override;
					bool DestroyWindow(Window::Handle Handle) override;

					void Clear(void) override;

					void Draw(DrawModes Mode, uint32 Count) override;

					void SwapBuffers(Window::Handle Handle) override;

					void PollEvents(void) override;
					bool WindowShouldClose(Window::Handle Handle) override;

					INLINE cstr GetLastError(void) const override
					{
						return m_LastError;
					}

				private:
					str m_LastError;

					uint8 m_SampleCount;
					bool m_ForwardCompatible;
					Color m_ClearColor;
					ClearFlags m_ClearFlags;
					FaceOrders m_FaceOrder;
					CullModes m_CullMode;
					TestFunctions m_DepthTestFunction;
					TestFunctions m_StencilTestFunction;
					int32 m_StencilTestFunctionReference;
					uint32 m_StencilTestFunctionMask;
					uint32 m_StencilMask;
					StencilOperations m_StencilOperationStencilFailed;
					StencilOperations m_StencilOperationDepthFailed;
					StencilOperations m_StencilOperationDepthPassed;
					BlendFunctions m_BlendFunctionSourceFactor;
					BlendFunctions m_BlendFunctionDestinationFactor;
					PolygonModes m_PolygonMode;
					CullModes m_PolygonModeCullMode;

					Program::Handle m_LastProgram;
					GPUBuffer::Handle m_LastBuffer;

					MeshBuffersMap m_MeshBuffers;
					uint32 m_LastMeshNumber;
					uint8 m_LastActiveTextureUnitIndex;
				};
			}
		}
	}
}

#endif