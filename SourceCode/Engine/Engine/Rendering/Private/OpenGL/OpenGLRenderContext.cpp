// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\OpenGL\OpenGLRenderContext.h>
#include <Common\BitwiseUtils.h>
#include <GL\glew.h>

namespace Engine
{
	using namespace Common;

	namespace Rendering
	{
		namespace Private
		{
			namespace OpenGL
			{
				OpenGLRenderContext::OpenGLRenderContext(PlatformWindow::WindowHandle WindowHandle, PlatformWindow::ContextHandle ContextHandle, PlatformWindow::WGLContextHandle WGLContextHandle) :
					RenderContext(WindowHandle),
					m_ContextHandle(ContextHandle),
					m_WGLContextHandle(WGLContextHandle),
					m_LastMeshHandle(0)
				{
				}

				void OpenGLRenderContext::Activate(void)
				{
					RenderContext::Activate();

					m_LastMeshHandle = 0;
				}

				//TODO: move these gl function calls to OpenGLDevice
				bool OpenGLRenderContext::DestroyVertexArray(GPUBuffer::Handle MeshHandle)
				{
					Assert(GetIsActive(), "Context is not active");

					if (!m_VertexArrays.Contains(MeshHandle))
						return false;

					GPUBuffer::Handle vao = m_VertexArrays[MeshHandle];

					glDeleteVertexArrays(1, &vao);

					m_VertexArrays.Remove(MeshHandle);

					return true;
				}

				bool OpenGLRenderContext::BindVertextArray(GPUBuffer::Handle MeshHandle, const OpenGLDevice::MeshBufferInfo& Info)
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

						glBindBuffer(GL_ARRAY_BUFFER, Info.VertexBufferObject);
						{
							GLenum __error = glGetError();
							Assert(__error == 0, "OpenGL call failed");
						}

						uint32 vertexSize = sizeof(Vertex);

						if (BitwiseUtils::IsEnabled(Info.Layout, Mesh::SubMesh::VertexLayouts::Position))
						{
							uint16 index = SubMeshInfo::GetLayoutIndex(Mesh::SubMesh::VertexLayouts::Position);

							glVertexAttribPointer(index, 3, GL_FLOAT, false, vertexSize, (void*)OffsetOf(&Vertex::Position));
							glEnableVertexAttribArray(index++);
						}
						if (BitwiseUtils::IsEnabled(Info.Layout, Mesh::SubMesh::VertexLayouts::Normal))
						{
							uint16 index = SubMeshInfo::GetLayoutIndex(Mesh::SubMesh::VertexLayouts::Normal);

							glVertexAttribPointer(index, 3, GL_FLOAT, false, vertexSize, (void*)OffsetOf(&Vertex::Normal));
							glEnableVertexAttribArray(index++);
						}
						if (BitwiseUtils::IsEnabled(Info.Layout, Mesh::SubMesh::VertexLayouts::UV))
						{
							uint16 index = SubMeshInfo::GetLayoutIndex(Mesh::SubMesh::VertexLayouts::UV);

							glVertexAttribPointer(index, 2, GL_FLOAT, false, vertexSize, (void*)OffsetOf(&Vertex::UV));
							glEnableVertexAttribArray(index);
						}

						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Info.ElementBufferObject);

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