// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\OpenGL\OpenGLDevice.h>
#include <Debugging\Debug.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Platform\PlatformMemory.h>
#include <Common\BitwiseUtils.h>

#include <GL\glew.h>
#include <glfw\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

namespace Engine
{
	using namespace Common;
	using namespace Platform;
	using namespace MemoryManagement::Allocator;

	namespace Rendering
	{
		namespace Private
		{
			namespace OpenGL
			{
				const uint8 LAST_ERROR_SIZE = 255;

				DynamicSizeAllocator allocator("OpenGL Device System Allocator", RootAllocator::GetInstance(), MegaByte);

				template<typename BaseType>
				BaseType *Allocate(uint32 Count)
				{
					return ReinterpretCast(BaseType*, AllocateMemory(&allocator, Count * sizeof(BaseType)));
				}

				template<typename BaseType>
				void Deallocate(BaseType *Ptr)
				{
					DeallocateMemory(&allocator, Ptr);
				}

				uint32 GetClearFlags(IDevice::ClearFlags Flags)
				{
					uint32 flags = 0;

					if (BitwiseUtils::IsEnabled(Flags, IDevice::ClearFlags::ColorBuffer))
						flags |= GL_COLOR_BUFFER_BIT;

					if (BitwiseUtils::IsEnabled(Flags, IDevice::ClearFlags::DepthBuffer))
						flags |= GL_DEPTH_BUFFER_BIT;

					if (BitwiseUtils::IsEnabled(Flags, IDevice::ClearFlags::AccumulationBuffer))
						flags |= GL_ACCUM_BUFFER_BIT;

					if (BitwiseUtils::IsEnabled(Flags, IDevice::ClearFlags::StencilBuffer))
						flags |= GL_STENCIL_BUFFER_BIT;

					return flags;
				}

				uint32 GetBufferUsageFlags(IDevice::BufferUsages Flag)
				{
					switch (Flag)
					{
					case  IDevice::BufferUsages::StreamDraw:
						return GL_STREAM_DRAW;
					case  IDevice::BufferUsages::StreamRead:
						return GL_STREAM_READ;
					case  IDevice::BufferUsages::StreamCopy:
						return GL_STREAM_COPY;
					case IDevice::BufferUsages::StaticDraw:
						return GL_STATIC_DRAW;
					case IDevice::BufferUsages::StaticRead:
						return GL_STATIC_READ;
					case IDevice::BufferUsages::StaticCopy:
						return GL_STATIC_COPY;
					case IDevice::BufferUsages::DynamicDraw:
						return GL_DYNAMIC_DRAW;
					case IDevice::BufferUsages::DynamicRead:
						return GL_DYNAMIC_READ;
					case IDevice::BufferUsages::DynamicCopy:
						return GL_DYNAMIC_COPY;
					}

					return GL_READ_ONLY;
				}

				uint32 GetDrawMode(IDevice::DrawModes Mode)
				{
					switch (Mode)
					{
					case IDevice::DrawModes::Lines:
						return GL_LINES;
					case IDevice::DrawModes::LineLoop:
						return GL_LINE_LOOP;
					case IDevice::DrawModes::LineStrip:
						return GL_LINE_STRIP;

					case IDevice::DrawModes::Triangles:
						return GL_TRIANGLES;
					case IDevice::DrawModes::TriangleStrip:
						return GL_TRIANGLE_STRIP;

					case IDevice::DrawModes::TriangleFan:
						return GL_TRIANGLE_FAN;
					case IDevice::DrawModes::Quads:
						return GL_QUADS;
					case IDevice::DrawModes::QuadStrip:
						return GL_QUAD_STRIP;
					case IDevice::DrawModes::Polygon:
						return GL_POLYGON;
					}

					return GL_LINES;
				}

				OpenGLDevice::OpenGLDevice(void) :
					m_ClearFlags(IDevice::ClearFlags::ColorBuffer)
				{
					m_LastError = Allocate<char8>(LAST_ERROR_SIZE + 1);
				}

				OpenGLDevice::~OpenGLDevice(void)
				{
					Deallocate(m_LastError);
				}

				bool OpenGLDevice::Initialize(void)
				{
					if (glfwInit() == GLFW_FALSE)
					{
						PlatformMemory::Copy("GLFW initialization failed", m_LastError, 26);

						return false;
					}

					glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
					glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

					return true;
				}

				void OpenGLDevice::SetSampleCount(uint8 Count)
				{
					glfwWindowHint(GLFW_SAMPLES, Count);
				}

				void OpenGLDevice::SetForwardCompatible(bool Value)
				{
					glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, (Value ? GL_TRUE : GL_FALSE));
				}

				void OpenGLDevice::SetProfilingEnabled(bool Value)
				{
					glfwWindowHint(GLFW_OPENGL_PROFILE, (Value ? GLFW_OPENGL_CORE_PROFILE : GLFW_OPENGL_ANY_PROFILE));
				}

				void OpenGLDevice::SetClearColor(Color Color)
				{
					glClearColor(Color.GetR() / 255.0F, Color.GetG() / 255.0F, Color.GetB() / 255.0F, Color.GetA() / 255.0F);
				}

				void OpenGLDevice::SetClearFlags(IDevice::ClearFlags Flags)
				{
					m_ClearFlags = Flags;
				}

				bool OpenGLDevice::CreateTexture2D(const byte * Data, uint32 Width, uint32 Height, Texture::Handle &Handle)
				{
					glGenTextures(1, &Handle);
					glBindTexture(GL_TEXTURE_2D, Handle);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_BGR, GL_UNSIGNED_BYTE, Data);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

					return true;
				}

				bool OpenGLDevice::DestroyTexture2D(Texture::Handle Handle)
				{
					glDeleteTextures(1, &Handle);

					return true;
				}

				bool OpenGLDevice::CreateProgram(cstr VertexShader, cstr FragmentShader, Program::Handle &Handle)
				{
					uint32 vertShaderID = glCreateShader(GL_VERTEX_SHADER);
					uint32 fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

					glShaderSource(vertShaderID, 1, &VertexShader, nullptr);
					glCompileShader(vertShaderID);

					glShaderSource(fragShaderID, 1, &FragmentShader, nullptr);
					glCompileShader(fragShaderID);

					int32 result;
					int32 infoLogLength;

					glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &result);
					glGetShaderiv(vertShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
					if (infoLogLength != 0)
					{
						glGetShaderInfoLog(vertShaderID, LAST_ERROR_SIZE, nullptr, m_LastError);

						return false;
					}

					glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &result);
					glGetShaderiv(fragShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
					if (infoLogLength != 0)
					{
						str info = Allocate<char8>(infoLogLength + 1);

						glGetShaderInfoLog(fragShaderID, LAST_ERROR_SIZE, nullptr, m_LastError);

						return false;
					}

					Handle = glCreateProgram();
					glAttachShader(Handle, vertShaderID);
					glAttachShader(Handle, fragShaderID);
					glLinkProgram(Handle);

					glGetProgramiv(Handle, GL_LINK_STATUS, &result);
					glGetProgramiv(Handle, GL_INFO_LOG_LENGTH, &infoLogLength);
					if (infoLogLength != 0)
					{
						glGetProgramInfoLog(Handle, LAST_ERROR_SIZE, nullptr, m_LastError);

						return false;
					}

					glDetachShader(Handle, vertShaderID);
					glDetachShader(Handle, fragShaderID);

					glDeleteShader(vertShaderID);
					glDeleteShader(fragShaderID);

					return true;
				}

				bool OpenGLDevice::DestroyProgram(Program::Handle Handle)
				{
					glDeleteProgram(Handle);

					return true;
				}

				bool OpenGLDevice::CreateBuffer(const float32 *Data, uint32 DataCount, BufferUsages Usage, GPUBuffer::Handle &Handle)
				{
					glGenBuffers(1, &Handle);
					glBindBuffer(GL_ARRAY_BUFFER, Handle);
					glBufferData(GL_ARRAY_BUFFER, DataCount * sizeof(float32), Data, GetBufferUsageFlags(Usage));

					return true;
				}

				bool OpenGLDevice::DestroyBuffer(GPUBuffer::Handle Handle)
				{
					glDeleteBuffers(1, &Handle);

					return true;
				}

				bool OpenGLDevice::CreateWindow(uint16 Width, uint16 Height, cstr Title, Window::Handle &Handle)
				{
					GLFWwindow *window = glfwCreateWindow(Width, Height, Title, nullptr, nullptr);

					if (window == nullptr)
					{
						PlatformMemory::Copy("Window creation failed", m_LastError, 22);

						return false;
					}

					glfwMakeContextCurrent(window);
					glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

					glewExperimental = true;
					if (glewInit() != GLEW_OK)
					{
						PlatformMemory::Copy("GLEW initialization failed", m_LastError, 26);

						return false;
					}

					Handle = ReinterpretCast(Window::Handle, window);

					return true;
				}

				bool OpenGLDevice::DestroyWindow(Window::Handle Handle)
				{
					//glfwDestroyWindow(reinterpret_cast<GLFWwindow*>(Handle));

					return true;
				}

				void OpenGLDevice::Clear(void)
				{
					glClear(GetClearFlags(m_ClearFlags));
				}

				bool OpenGLDevice::BindProgram(Program::Handle Handle)
				{
					glUseProgram(Handle);

					return true;
				}

				bool OpenGLDevice::BindBuffer(GPUBuffer::Handle Handle, uint32 Size, uint32 Index, bool Normalized, uint32 Stride)
				{
					glEnableVertexAttribArray(Index);
					glBindBuffer(GL_ARRAY_BUFFER, Handle);
					glVertexAttribPointer(0, Size, GL_FLOAT, Normalized, Stride, nullptr);

					return true;
				}

				void OpenGLDevice::Draw(DrawModes Mode, uint32 FirstIndex, uint32 Count)
				{
					glDrawArrays(GetDrawMode(Mode), FirstIndex, Count);
				}

				void OpenGLDevice::SwapBuffers(Window::Handle Handle)
				{
					glfwSwapBuffers(ReinterpretCast(GLFWwindow*, Handle));
				}

				void OpenGLDevice::PollEvents(void)
				{
					glfwPollEvents();
				}

				bool OpenGLDevice::WindowShouldClose(Window::Handle Handle)
				{
					return (glfwWindowShouldClose(ReinterpretCast(GLFWwindow*, Handle)) == GLFW_TRUE);
				}
			}
		}
	}
}
