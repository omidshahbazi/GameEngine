// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\OpenGL\OpenGLDevice.h>
#include <Debugging\Debug.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

#include <GL\glew.h>
#include <glfw\glfw3.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>

namespace Engine
{
	using namespace MemoryManagement::Allocator;

	namespace Rendering
	{
		namespace Private
		{
			namespace OpenGL
			{
				DynamicSizeAllocator allocator("OpenGL Device System Allocator", RootAllocator::GetInstance(), 1024 * 1024);

				template<typename BaseType>
				BaseType *Allocate(uint32 Count)
				{
					return reinterpret_cast<BaseType*>(AllocateMemory(&allocator, Count * sizeof(BaseType)));
				}

				template<typename BaseType>
				void Deallocate(BaseType *Ptr)
				{
					allocator.Deallocate(reinterpret_cast<byte*>(Ptr));
				}

				bool OpenGLDevice::Initialize(void)
				{
					if (glfwInit() == GLFW_FALSE)
					{
						Assert(false, "GLFW initialization failed");
						return false;
					}

					glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLFW_VERSION_MINOR);
					glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLFW_VERSION_MAJOR);

					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
					glfwWindowHint(GLFW_SAMPLES, 4);
					glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

					return true;
				}

				bool OpenGLDevice::CreateTexture2D(const byte * Data, uint32 Width, uint32 Height, TextureHandle &Handle)
				{
					glGenTextures(1, &Handle);
					glBindTexture(GL_TEXTURE_2D, Handle);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_BGR, GL_UNSIGNED_BYTE, Data);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

					return true;
				}

				bool OpenGLDevice::CreateProgram(cstr VertexShader, cstr FragmentShader, ProgramHandle &Handle)
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
						str info = Allocate<char8>(infoLogLength + 1);

						glGetShaderInfoLog(vertShaderID, infoLogLength, nullptr, info);

						Assert(false, info);

						Deallocate(info);

						return false;
					}

					glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &result);
					glGetShaderiv(fragShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
					if (infoLogLength != 0)
					{
						str info = Allocate<char8>(infoLogLength + 1);

						glGetShaderInfoLog(fragShaderID, infoLogLength, nullptr, info);

						Assert(false, info);

						Deallocate(info);

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
						str info = Allocate<char8>(infoLogLength + 1);

						glGetProgramInfoLog(Handle, infoLogLength, nullptr, info);

						Assert(false, info);

						Deallocate(info);

						return false;
					}

					glDetachShader(Handle, vertShaderID);
					glDetachShader(Handle, fragShaderID);

					glDeleteShader(vertShaderID);
					glDeleteShader(fragShaderID);

					return true;
				}
			}
		}
	}
}
