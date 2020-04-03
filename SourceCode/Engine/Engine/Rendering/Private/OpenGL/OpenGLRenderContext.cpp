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
				OpenGLRenderContext::OpenGLRenderContext(OpenGLDevice* Device, PlatformWindow::WindowHandle WindowHandle, PlatformWindow::ContextHandle ContextHandle, PlatformWindow::WGLContextHandle WGLContextHandle) :
					RenderContext(WindowHandle),
					m_Device(Device),
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

				bool OpenGLRenderContext::DestroyVertexArray(GPUBuffer::Handle MeshHandle)
				{
					Assert(GetIsActive(), "Context is not active");

					if (!m_VertexArrays.Contains(MeshHandle))
						return false;

					if (!m_Device->DestroyVertexArray(m_VertexArrays[MeshHandle]))
						return false;

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
						if (!m_Device->CreateVertexArray(Info, vao))
							return false;

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