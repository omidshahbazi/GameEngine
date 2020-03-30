// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GL_RENDER_CONTEXT_H
#define GL_RENDER_CONTEXT_H

#include <Containers\Map.h>
#include <Rendering\GPUBuffer.h>
#include <Rendering\RenderContext.h>
#include <Rendering\Private\OpenGL\OpenGLDevice.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace OpenGL
			{
				class RENDERING_API OpenGLRenderContext : public RenderContext
				{
				private:
					typedef Map<GPUBuffer::Handle, GPUBuffer::Handle> MeshVertexArrayMap;

				public:
					OpenGLRenderContext(PlatformWindow::WindowHandle WindowHandle, PlatformWindow::ContextHandle ContextHandle, PlatformWindow::WGLContextHandle WGLContextHandle);

					void Activate(void) override;

					bool DestroyVertexArray(GPUBuffer::Handle MeshHandle);
					bool BindVertextArray(GPUBuffer::Handle MeshHandle, const OpenGLDevice::MeshBufferInfo &Info);

					PlatformWindow::ContextHandle GetContextHandle(void) const
					{
						return m_ContextHandle;
					}

					PlatformWindow::WGLContextHandle GetWGLContextHandle(void) const
					{
						return m_WGLContextHandle;
					}

				private:
					PlatformWindow::ContextHandle m_ContextHandle;
					PlatformWindow::WGLContextHandle m_WGLContextHandle;

					MeshVertexArrayMap m_VertexArrays;
					GPUBuffer::Handle m_LastMeshHandle;
				};
			}
		}
	}
}

#endif