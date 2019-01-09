// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\OpenGL\OpenGLDevice.h>
#include <Debugging\Debug.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Platform\PlatformMemory.h>
#include <Common\BitwiseUtils.h>

#include <GL\glew.h>
#include <glfw\glfw3.h>

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
				const uint16 LAST_ERROR_SIZE = 512;

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
					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

				void OpenGLDevice::SetClearColor(Color Color)
				{
					glClearColor(Color.GetFloat32R(), Color.GetFloat32G(), Color.GetFloat32B(), Color.GetFloat32A());
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

				bool OpenGLDevice::BindProgram(Program::Handle Handle)
				{
					glUseProgram(Handle);

					return true;
				}

				bool OpenGLDevice::QueryProgramActiveConstants(Program::Handle Program, StringList &Names)
				{
					int32 count;

					glGetProgramiv(Program, GL_ACTIVE_UNIFORMS, &count);

					const uint8 bufferSize = 32;
					char8 name[bufferSize];
					int32 nameLength;
					int32 constantSize;
					uint32 type;

					for (int8 i = 0; i < count; i++)
					{
						glGetActiveUniform(Program, (GLuint)i, bufferSize, &nameLength, &constantSize, &type, name);

						name[nameLength] = CharacterUtility::Character<char8, '\0'>::Value;

						Names.Add(name);
					}

					return false;
				}

				bool OpenGLDevice::GetProgramConstantHandle(Program::Handle Handle, const String &Name, Program::ConstantHandle &ConstantHandle)
				{
					ConstantHandle = glGetUniformLocation(Handle, Name.GetValue());
					return true;
				}

				bool OpenGLDevice::SetProgramFloat32(Program::ConstantHandle Handle, float32 Value)
				{
					glUniform1f(Handle, Value);

					return true;
				}

				bool OpenGLDevice::SetProgramVector2(Program::ConstantHandle Handle, const Vector2F & Value)
				{
					glUniform2f(Handle, Value.X, Value.Y);

					return true;
				}

				bool OpenGLDevice::SetProgramVector3(Program::ConstantHandle Handle, const Vector3F & Value)
				{
					glUniform3f(Handle, Value.X, Value.Y, Value.Z);

					return true;
				}

				bool OpenGLDevice::SetProgramColor(Program::ConstantHandle Handle, Color Value)
				{
					glUniform4f(Handle, Value.GetFloat32R(), Value.GetFloat32G(), Value.GetFloat32B(), Value.GetFloat32A());

					return true;
				}

				bool OpenGLDevice::SetProgramMatrix4(Program::ConstantHandle Handle, const Matrix4F &Value)
				{
					glUniformMatrix4fv(Handle, 1, false, Value.GetValue());

					return true;
				}

				bool OpenGLDevice::SetProgramTexture(Program::ConstantHandle Handle, Texture::Handle Value)
				{
					return true;
				}

				bool OpenGLDevice::SetProgramFloat32(Program::Handle Program, const String &Name, float32 Value)
				{
					BindProgram(Program);

					Program::ConstantHandle constHandle;
					if (!GetProgramConstantHandle(Program, Name.GetValue(), constHandle))
						return false;

					return SetProgramFloat32(constHandle, Value);
				}

				bool OpenGLDevice::SetProgramVector2(Program::Handle Program, const String & Name, const Vector2F & Value)
				{
					BindProgram(Program);

					Program::ConstantHandle constHandle;
					if (!GetProgramConstantHandle(Program, Name.GetValue(), constHandle))
						return false;

					return SetProgramVector2(constHandle, Value);
				}

				bool OpenGLDevice::SetProgramVector3(Program::Handle Program, const String & Name, const Vector3F & Value)
				{
					BindProgram(Program);

					Program::ConstantHandle constHandle;
					if (!GetProgramConstantHandle(Program, Name.GetValue(), constHandle))
						return false;

					return SetProgramVector3(constHandle, Value);
				}

				bool OpenGLDevice::SetProgramColor(Program::Handle Program, const String &Name, Color Value)
				{
					BindProgram(Program);

					Program::ConstantHandle constHandle;
					if (!GetProgramConstantHandle(Program, Name.GetValue(), constHandle))
						return false;

					return SetProgramColor(constHandle, Value);
				}

				bool OpenGLDevice::SetProgramMatrix4(Program::Handle Program, const String &Name, const Matrix4F &Value)
				{
					BindProgram(Program);

					Program::ConstantHandle constHandle;
					if (!GetProgramConstantHandle(Program, Name.GetValue(), constHandle))
						return false;

					return SetProgramMatrix4(constHandle, Value);
				}

				bool OpenGLDevice::SetProgramTexture(Program::Handle Handle, const String & Name, Texture::Handle Value)
				{
					BindProgram(Handle);

					Program::ConstantHandle constHandle;
					if (!GetProgramConstantHandle(Handle, Name.GetValue(), constHandle))
						return false;

					return SetProgramTexture(constHandle, Value);
				}

				bool OpenGLDevice::CreateMesh(const SubMeshInfo *Info, BufferUsages Usage, GPUBuffer::Handle &Handle)
				{
					if (Info->Vertices.GetSize() == 0)
						return false;

					if (Info->Indices.GetSize() == 0)
						return false;

					uint32 vertexSize = sizeof(Vertex);

					uint32 vao;
					glGenVertexArrays(1, &vao);
					glBindVertexArray(vao);

					uint32 vbo;
					glGenBuffers(1, &vbo);
					glBindBuffer(GL_ARRAY_BUFFER, vbo);
					glBufferData(GL_ARRAY_BUFFER, Info->Vertices.GetSize() * vertexSize, Info->Vertices.GetData(), GL_STATIC_DRAW);

					uint32 ebo;
					glGenBuffers(1, &ebo);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, Info->Indices.GetSize() * sizeof(float), Info->Indices.GetData(), GL_STATIC_DRAW);

					if (BitwiseUtils::IsEnabled(Info->Layout, SubMeshInfo::VertexLayouts::Position))
					{
						uint16 index = SubMeshInfo::GetLayoutIndex(SubMeshInfo::VertexLayouts::Position);

						glVertexAttribPointer(index, 3, GL_FLOAT, false, vertexSize, (void*)OffsetOf(&Vertex::Position));
						glEnableVertexAttribArray(index++);
					}
					if (BitwiseUtils::IsEnabled(Info->Layout, SubMeshInfo::VertexLayouts::Normal))
					{
						uint16 index = SubMeshInfo::GetLayoutIndex(SubMeshInfo::VertexLayouts::Position);

						glVertexAttribPointer(index, 3, GL_FLOAT, false, vertexSize, (void*)OffsetOf(&Vertex::Normal));
						glEnableVertexAttribArray(index++);
					}
					if (BitwiseUtils::IsEnabled(Info->Layout, SubMeshInfo::VertexLayouts::UV))
					{
						uint16 index = SubMeshInfo::GetLayoutIndex(SubMeshInfo::VertexLayouts::UV);

						glVertexAttribPointer(index, 2, GL_FLOAT, false, vertexSize, (void*)OffsetOf(&Vertex::UV));
						glEnableVertexAttribArray(index);
					}

					glBindBuffer(GL_ARRAY_BUFFER, 0);
					//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
					glBindVertexArray(0);

					Handle = ++m_LastMeshNumber;
					m_MeshBuffers[m_LastMeshNumber] = { vao, vbo, ebo };

					return true;
				}

				bool OpenGLDevice::DestroyMesh(GPUBuffer::Handle Handle)
				{
					if (!m_MeshBuffers.Contains(Handle))
						return false;

					auto &info = m_MeshBuffers[Handle];

					glDeleteBuffers(1, &info.VertexArrayObject);

					m_MeshBuffers.Remove(Handle);

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

				bool OpenGLDevice::BindBuffer(GPUBuffer::Handle Handle)
				{
					if (!m_MeshBuffers.Contains(Handle))
						return false;

					auto &info = m_MeshBuffers[Handle];

					glBindVertexArray(info.VertexArrayObject);

					return true;
				}

				void OpenGLDevice::Clear(void)
				{
					glClear(GetClearFlags(m_ClearFlags));
				}

				void OpenGLDevice::Draw(DrawModes Mode, uint32 Count)
				{
					glDrawElements(GetDrawMode(Mode), Count, GL_UNSIGNED_INT, 0);
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
