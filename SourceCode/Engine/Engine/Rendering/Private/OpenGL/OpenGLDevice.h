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

					void SetSampleCount(uint8 Count) override;

					void SetForwardCompatible(bool Value)override;

					void SetClearColor(Color Color) override;

					void SetClearFlags(IDevice::ClearFlags Flags) override;

					bool CreateProgram(cstr VertexShader, cstr FragmentShader, Program::Handle &Handle) override;
					bool DestroyProgram(Program::Handle Handle) override;
					bool BindProgram(Program::Handle Handle) override;
					bool GetProgramConstantHandle(Program::Handle Handle, const String &Name, Program::ConstantHandle &ConstantHandle) override;
					bool SetProgramFloat32(Program::ConstantHandle Handle, float32 Value) override;
					bool SetProgramColor(Program::ConstantHandle Handle, Color Value) override;
					bool SetProgramMatrix4(Program::ConstantHandle Handle, const Matrix4F &Value) override;
					bool SetProgramFloat32(Program::Handle Handle, const String &Name, float32 Value) override;
					bool SetProgramColor(Program::Handle Handle, const String &Name, Color Value) override;
					bool SetProgramMatrix4(Program::Handle Program, const String &Name, const Matrix4F &Value) override;

					bool CreateTexture2D(const byte *Data, uint32 Width, uint32 Height, Texture::Handle &Handle) override;
					bool DestroyTexture2D(Texture::Handle Handle) override;

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

					IDevice::ClearFlags m_ClearFlags;

					MeshBuffersMap m_MeshBuffers;
					uint32 m_LastMeshNumber;
				};
			}
		}
	}
}

#endif