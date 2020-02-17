// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\OpenGL\GLRenderContext.h>
#include <GL\glew.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace OpenGL
			{
				GLRenderContext::GLRenderContext(PlatformWindow::WindowHandle WindowHandle, PlatformWindow::ContextHandle ContextHandle, PlatformWindow::WGLContextHandle WGLContextHandle) :
					RenderContext(WindowHandle),
					m_ContextHandle(ContextHandle),
					m_WGLContextHandle(WGLContextHandle),
					m_LastMeshHandle(0)
				{
				}

				void GLRenderContext::ResetState(void)
				{
					m_LastMeshHandle = 0;
				}

				bool GLRenderContext::DeleteVertexArray(GPUBuffer::Handle MeshHandle)
				{
					if (!m_VertexArrays.Contains(MeshHandle))
						return false;

					GPUBuffer::Handle vao = m_VertexArrays[MeshHandle];

					glDeleteVertexArrays(1, &vao);

					m_VertexArrays.Remove(MeshHandle);

					return true;
				}

				bool GLRenderContext::BindVertextArray(GPUBuffer::Handle MeshHandle, GPUBuffer::Handle VertexBufferObject, GPUBuffer::Handle ElementBufferObject)
				{
					if (m_LastMeshHandle == MeshHandle)
						return true;

					m_LastMeshHandle = MeshHandle;

					uint32 vao = 0;
					if (!m_VertexArrays.Contains(m_LastMeshHandle))
					{
						glGenVertexArrays(1, &vao);
						glBindVertexArray(vao);

						glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject);

						glBindVertexArray(0);

						m_VertexArrays[m_LastMeshHandle] = vao;
					}
					else
						vao = m_VertexArrays[m_LastMeshHandle];

					glBindVertexArray(vao);

					return true;
				}
			}
		}
	}
}