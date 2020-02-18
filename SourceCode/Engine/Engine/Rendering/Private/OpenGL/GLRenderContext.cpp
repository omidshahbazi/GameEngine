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

				void GLRenderContext::Activate(void)
				{
					RenderContext::Activate();

					m_LastMeshHandle = 0;
				}

				bool GLRenderContext::DeleteVertexArray(GPUBuffer::Handle MeshHandle)
				{
					Assert(GetIsActive(), "Context is not active");

					if (!m_VertexArrays.Contains(MeshHandle))
						return false;

					GPUBuffer::Handle vao = m_VertexArrays[MeshHandle];

					glDeleteVertexArrays(1, &vao);

					m_VertexArrays.Remove(MeshHandle);

					return true;
				}

				bool GLRenderContext::BindVertextArray(GPUBuffer::Handle MeshHandle, GPUBuffer::Handle VertexBufferObject, GPUBuffer::Handle ElementBufferObject)
				{
					Assert(GetIsActive(), "Context is not active");

					if (m_LastMeshHandle == MeshHandle)
						return true;

					m_LastMeshHandle = MeshHandle;

					uint32 vao = 0;
					if (!m_VertexArrays.Contains(m_LastMeshHandle))
					{
						glGenVertexArrays(1, &vao);
						glBindVertexArray(vao);

						glBindBuffer(GL_ARRAY_BUFFER, VertexBufferObject);
						{ 
							GLenum __error = glGetError();
							Assert(__error == 0, "OpenGL call failed");
						}

						//TODO: These two command should be generalized and get here by the callee
						{
							glVertexAttribPointer(0, 3, GL_FLOAT, false, 32, (void*)0);
							{
								GLenum __error = glGetError();
								Assert(__error == 0, "OpenGL call failed");
							}

							glEnableVertexAttribArray(0);
							{
								GLenum __error = glGetError();
								Assert(__error == 0, "OpenGL call failed");
							}
						}

						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ElementBufferObject);
						{
							GLenum __error = glGetError();
							Assert(__error == 0, "OpenGL call failed");
						}

						glBindVertexArray(0);
						{
							GLenum __error = glGetError();
							Assert(__error == 0, "OpenGL call failed");
						}

						m_VertexArrays[m_LastMeshHandle] = vao;
					}
					else
						vao = m_VertexArrays[m_LastMeshHandle];

					glBindVertexArray(vao);
					{
						GLenum __error = glGetError();
						Assert(__error == 0, "OpenGL call failed");
					}

					return true;
				}
			}
		}
	}
}