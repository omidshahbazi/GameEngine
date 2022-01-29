// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <OpenGLRenderDevice\Private\OpenGLDevice.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <ProgramCompilerCommon\Common.h>
#include <RenderCommon\Helper.h>
#include <Containers\StringUtility.h>
#include <Debugging\CoreDebug.h>
#include <GLEW\include\GL\glew.h>

namespace Engine
{
	//https://www.khronos.org/opengl/wiki/Common_Mistakes

	using namespace Common;
	using namespace Containers;
	using namespace Platform;
	using namespace RenderCommon::Private;
	using namespace ProgramCompilerCommon;

	namespace OpenGLRenderDevice
	{
		namespace Private
		{
#define INITIALIZE_BUFFER_INFO(BufferInfoPtr, GLHandle, BufferSize, BufferType, IsIntermediateFlag) \
				(BufferInfoPtr)->Handle = GLHandle; \
				(BufferInfoPtr)->Size = BufferSize; \
				(BufferInfoPtr)->Type = BufferType; \
				(BufferInfoPtr)->IsIntermediate = IsIntermediateFlag;

			uint32 GetTextureInternalFormat(Formats Format)
			{
				switch (Format)
				{
				case Formats::R8:
					return GL_R8;
				case Formats::R16:
					return GL_R16;
				case Formats::R32:
					return GL_R32UI;
				case Formats::R16F:
					return GL_R16F;
				case Formats::R32F:
					return GL_R32F;
				case Formats::RG8:
					return GL_RG8;
				case Formats::RG16:
					return GL_RG16;
				case Formats::RG32:
					return GL_RG32UI;
				case Formats::RG16F:
					return GL_RG16F;
				case Formats::RG32F:
					return GL_RG32F;
				case Formats::RGB8:
					return GL_RGB8;
				case Formats::RGB16:
					return GL_RGB16UI;
				case Formats::RGB32:
					return GL_RGB32UI;
				case Formats::RGB16F:
					return GL_RGB16F;
				case Formats::RGB32F:
					return GL_RGB32F;
				case Formats::RGBA8:
					return GL_RGBA8;
				case Formats::RGBA16:
					return GL_RGBA16;
				case Formats::RGBA32:
					return GL_RGBA32UI;
				case Formats::RGBA16F:
					return GL_RGBA16F;
				case Formats::RGBA32F:
					return GL_RGBA32F;
				case Formats::Depth16:
					return GL_DEPTH_COMPONENT16;
				case Formats::Depth24:
					return GL_DEPTH_COMPONENT24;
				case Formats::Depth32:
					return GL_DEPTH_COMPONENT32;
				case Formats::Depth32F:
					return GL_DEPTH_COMPONENT32F;
				case Formats::DepthStencil24F:
					return GL_DEPTH24_STENCIL8;
				case Formats::DepthStencil32F:
					return GL_DEPTH24_STENCIL8;
				}

				return 0;
			}

			uint32 GetWrapMode(TextureWrapModes Mode)
			{
				switch (Mode)
				{
				case TextureWrapModes::Clamp:
					return GL_CLAMP;
				case TextureWrapModes::Repeat:
					return GL_REPEAT;
				case TextureWrapModes::ClampToEdge:
					return GL_CLAMP_TO_EDGE;
				case TextureWrapModes::MirroredRepeat:
					return GL_MIRRORED_REPEAT;
				}

				return 0;
			}

			uint32 GetMinifyFilter(TextureMinifyFilters Filter)
			{
				switch (Filter)
				{
				case TextureMinifyFilters::Nearest:
					return GL_NEAREST;
				case TextureMinifyFilters::Linear:
					return GL_LINEAR;
				case TextureMinifyFilters::NearestMipMapNearest:
					return GL_NEAREST_MIPMAP_NEAREST;
				case TextureMinifyFilters::LinearMipMapNearest:
					return GL_LINEAR_MIPMAP_NEAREST;
				case TextureMinifyFilters::NearestMipMapLinear:
					return GL_NEAREST_MIPMAP_LINEAR;
				case TextureMinifyFilters::LinearMipMapLinear:
					return GL_LINEAR_MIPMAP_LINEAR;
				}

				return 0;
			}

			uint32 GetMagnifyFilter(TextureMagnfyFilters Filter)
			{
				switch (Filter)
				{
				case TextureMagnfyFilters::Nearest:
					return GL_NEAREST;
				case TextureMagnfyFilters::Linear:
					return GL_LINEAR;
				}

				return 0;
			}

			uint32 GetAttachmentPoint(AttachmentPoints Point)
			{
				switch (Point)
				{
				case AttachmentPoints::Depth: return GL_DEPTH_ATTACHMENT;
				case AttachmentPoints::DepthStencil: return GL_DEPTH_STENCIL_ATTACHMENT;
				case AttachmentPoints::Color0: return GL_COLOR_ATTACHMENT0;
				case AttachmentPoints::Color1: return GL_COLOR_ATTACHMENT1;
				case AttachmentPoints::Color2: return GL_COLOR_ATTACHMENT2;
				case AttachmentPoints::Color3: return GL_COLOR_ATTACHMENT3;
				case AttachmentPoints::Color4: return GL_COLOR_ATTACHMENT4;
				case AttachmentPoints::Color5: return GL_COLOR_ATTACHMENT5;
				case AttachmentPoints::Color6: return GL_COLOR_ATTACHMENT6;
				case AttachmentPoints::Color7: return GL_COLOR_ATTACHMENT7;
				case AttachmentPoints::Color8: return GL_COLOR_ATTACHMENT8;
				}

				return 0;
			}

			bool CreateBufferInternal(GPUBufferTypes Type, uint32 Size, bool IsIntermediate, ResourceHandle& Handle)
			{
				if (Size == 0)
					return false;

				BufferInfo* info = RenderSystemAllocators::ResourceAllocator_Allocate<BufferInfo>();

				uint32 target = GetBufferType(Type);

				uint32 handle;
				glGenBuffers(1, &handle);

				glBindBuffer(target, handle);

				glBufferData(target, Size, nullptr, GL_STATIC_COPY);

				glBindBuffer(target, 0);

				INITIALIZE_BUFFER_INFO(info, handle, Size, Type, IsIntermediate);

				Handle = (ResourceHandle)info;

				return true;
			}

			void GLAPIENTRY DebugOutputProcedure(GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei Length, const GLchar* Message, const GLvoid* Param)
			{
				OpenGLDevice* device = ConstCast(OpenGLDevice*, ReinterpretCast(const OpenGLDevice*, Param));
				IDevice::DebugFunction procedure = device->GetDebugCallback();
				if (procedure == nullptr)
					return;

				IDevice::DebugSources source;
				switch (Source)
				{
				case GL_DEBUG_SOURCE_API:             source = IDevice::DebugSources::API; break;
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source = IDevice::DebugSources::WindowSystem; break;
				case GL_DEBUG_SOURCE_SHADER_COMPILER: source = IDevice::DebugSources::ProgramCompiler; break;
				case GL_DEBUG_SOURCE_THIRD_PARTY:     source = IDevice::DebugSources::ThirdParty; break;
				case GL_DEBUG_SOURCE_APPLICATION:     source = IDevice::DebugSources::Application; break;
				case GL_DEBUG_SOURCE_OTHER:           source = IDevice::DebugSources::Other; break;
				}

				IDevice::DebugTypes type;
				switch (Type)
				{
				case GL_DEBUG_TYPE_ERROR:               type = IDevice::DebugTypes::Error; break;
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type = IDevice::DebugTypes::DeprecatedBehavior; break;
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type = IDevice::DebugTypes::UndefinedBehavior; break;
				case GL_DEBUG_TYPE_PORTABILITY:         type = IDevice::DebugTypes::Portability; break;
				case GL_DEBUG_TYPE_PERFORMANCE:         type = IDevice::DebugTypes::Performance; break;
				case GL_DEBUG_TYPE_MARKER:              type = IDevice::DebugTypes::Marker; break;
				case GL_DEBUG_TYPE_PUSH_GROUP:          type = IDevice::DebugTypes::PushGroup; break;
				case GL_DEBUG_TYPE_POP_GROUP:           type = IDevice::DebugTypes::PopGroup; break;
				case GL_DEBUG_TYPE_OTHER:               type = IDevice::DebugTypes::Other; break;
				}

				IDevice::DebugSeverities severity;
				switch (Severity)
				{
				case GL_DEBUG_SEVERITY_HIGH:         severity = IDevice::DebugSeverities::High; break;
				case GL_DEBUG_SEVERITY_MEDIUM:       severity = IDevice::DebugSeverities::Medium; break;
				case GL_DEBUG_SEVERITY_LOW:          severity = IDevice::DebugSeverities::Low; break;
				case GL_DEBUG_SEVERITY_NOTIFICATION: severity = IDevice::DebugSeverities::Notification; break;
				}

				procedure(ID, source, Message, type, severity);
			}

			OpenGLDevice::OpenGLDevice(void) :
				m_Initialized(false),
				m_BaseContextHandle(0),
				m_BaseContextWindow(nullptr),
				m_BaseContext(nullptr),
				m_CurrentContextHandle(0),
				m_CurrentContext(nullptr)
			{
			}

			OpenGLDevice::~OpenGLDevice(void)
			{
				if (!m_Initialized)
					return;

				Uninitialize();
			}

			bool OpenGLDevice::Initialize(void)
			{
				CoreDebugAssert(Categories::RenderSystem, !m_Initialized, "OpenGLDevice already initialized");

				if (!InitializeBaseContext())
					return false;

#ifdef DEBUG_MODE
				glEnable(GL_DEBUG_OUTPUT);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

				glDebugMessageCallback(DebugOutputProcedure, this);

				glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
#endif

				m_CommandBufferPool.InitializeType(ICommandBuffer::Types::Copy);
				m_CommandBufferPool.InitializeType(ICommandBuffer::Types::Graphics);
				m_CommandBufferPool.InitializeType(ICommandBuffer::Types::Compute);

				m_Initialized = true;

				return true;
			}

			bool OpenGLDevice::Uninitialize(void)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Initialized, "OpenGLDevice already uninitialized");

				DestroyContext(m_BaseContextHandle);
				RenderSystemAllocators::RenderSystemAllocator_Deallocate(m_BaseContextWindow);

				m_Initialized = false;

				return true;
			}

			cstr OpenGLDevice::GetVersion(void)
			{
				return ReinterpretCast(cstr, glGetString(GL_VERSION));
			}

			cstr OpenGLDevice::GetVendorName(void)
			{
				return ReinterpretCast(cstr, glGetString(GL_VENDOR));
			}

			cstr OpenGLDevice::GetRendererName(void)
			{
				return ReinterpretCast(cstr, glGetString(GL_RENDERER));
			}

			cstr OpenGLDevice::GetShadingLanguageVersion(void)
			{
				return ReinterpretCast(cstr, glGetString(GL_SHADING_LANGUAGE_VERSION));
			}

			bool OpenGLDevice::CreateContext(PlatformWindow::WindowHandle WindowHandle, ResourceHandle& Handle)
			{
				if (WindowHandle == 0)
					return false;

				for (auto& item : m_Contexts)
					if (item.GetFirst() == (ResourceHandle)WindowHandle)
						return false;

				PlatformGL::ContextHandle contextHandle = PlatformGL::GetDeviceContext(WindowHandle);

				if (contextHandle == 0)
					return false;

				PlatformGL::PixelFormatInfo pixelFormat =
				{
					PlatformGL::PixelFormats::DrawToWindow | PlatformGL::PixelFormats::DrawToBitmap | PlatformGL::PixelFormats::SupportOpenGL | PlatformGL::PixelFormats::DoubleBuffer,
					PlatformGL::PixelTypes::RGBA,
					32,
					24,
					8,
					PlatformGL::LayerTypes::MainPlane
				};

				int32 pixelFormatIndex = PlatformGL::ChoosePixelFormat(contextHandle, &pixelFormat);
				PlatformGL::SetPixelFormat(contextHandle, pixelFormatIndex, &pixelFormat);

				PlatformGL::WGLContextHandle shareWGLContextHandle = 0;
				if (m_BaseContext != nullptr)
					shareWGLContextHandle = m_BaseContext->WGLContextHandle;

				PlatformGL::ContextHandle prevContextHandle;
				PlatformGL::WGLContextHandle prevWGLContextHandle;
				PlatformGL::GetCurrentWGLContext(prevContextHandle, prevWGLContextHandle);

				PlatformGL::WGLContextHandle wglContextHandle = PlatformGL::CreateWGLARBContext(contextHandle, shareWGLContextHandle,
#ifdef DEBUG_MODE
					true
#else
					false
#endif
				);

				PlatformGL::MakeCurrentWGLContext(prevContextHandle, prevWGLContextHandle);

				if (wglContextHandle == 0)
					return false;

				Handle = (ResourceHandle)WindowHandle;

				RenderContextInfo* info = RenderSystemAllocators::ResourceAllocator_Allocate<RenderContextInfo>();
				Construct(info);

				info->ContextHandle = contextHandle;
				info->WGLContextHandle = wglContextHandle;
				info->IsActive = false;

				m_Contexts[Handle] = info;

				if (m_BaseContext == nullptr)
					m_BaseContext = info;

				return true;
			}

			bool OpenGLDevice::DestroyContext(ResourceHandle Handle)
			{
				if (Handle == 0)
					return true;

				if (!m_Contexts.Contains(Handle))
					return false;

				RenderContextInfo* info = m_Contexts[Handle];

				if (m_CurrentContextHandle == Handle)
					SetContext(0);

				PlatformGL::DestroyWGLContext(info->WGLContextHandle);

				RenderSystemAllocators::ResourceAllocator_Deallocate(info);

				m_Contexts.Remove(Handle);

				return true;
			}

			bool OpenGLDevice::SetContext(ResourceHandle Handle)
			{
				if (m_CurrentContextHandle == Handle)
					return true;

				if (m_CurrentContext != nullptr)
					m_CurrentContext->IsActive = false;

				if (Handle == 0)
				{
					m_CurrentContextHandle = 0;
					m_CurrentContext = nullptr;
					PlatformGL::MakeCurrentWGLContext(0, 0);
					return true;
				}

				if (!m_Contexts.Contains(Handle))
					return false;

				RenderContextInfo* info = m_Contexts[Handle];

				m_CurrentContextHandle = Handle;
				m_CurrentContext = info;

				PlatformGL::MakeCurrentWGLContext(info->ContextHandle, info->WGLContextHandle);

				info->IsActive = true;

				//https://www.khronos.org/opengl/wiki/Multisampling
				//https://learnopengl.com/Advanced-OpenGL/Anti-Aliasing
				//glEnable(GL_MULTISAMPLE);

				return true;
			}

			bool OpenGLDevice::SwapBuffers(void)
			{
				if (m_CurrentContext == nullptr)
					return false;

				PlatformGL::SwapBuffers(m_CurrentContext->ContextHandle, false);

				return true;
			}

			bool OpenGLDevice::CreateBuffer(GPUBufferTypes Type, uint32 Size, ResourceHandle& Handle)
			{
				return CreateBufferInternal(Type, Size, true, Handle);
			}

			bool OpenGLDevice::DestroyBuffer(ResourceHandle Handle)
			{
				if (Handle == 0)
					return false;

				BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

				glDeleteBuffers(1, &info->Handle);

				RenderSystemAllocators::ResourceAllocator_Deallocate(info);

				return true;
			}

			bool OpenGLDevice::LockBuffer(ResourceHandle Handle, GPUBufferAccess Access, byte** Buffer)
			{
				if (Handle == 0)
					return false;

				if (Buffer == nullptr)
					return false;

				BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

				return LockBufferInternal(info, Access, Buffer);
			}

			bool OpenGLDevice::UnlockBuffer(ResourceHandle Handle)
			{
				if (Handle == 0)
					return false;

				BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

				return UnlockBufferInternal(info);
			}

			bool OpenGLDevice::CreateProgram(const CompiledShaders* Shaders, ResourceHandle& Handle, cstr* ErrorMessage)
			{
#define IMPLEMENT_CREATE_SHADER(Type, StageName) \
					uint32 StageName##ID = 0; \
					if (Shaders->StageName.Size != 0) \
					{ \
						StageName##ID = glCreateShader(Type); \
						glShaderBinary(1, &StageName##ID, GL_SHADER_BINARY_FORMAT_SPIR_V, Shaders->StageName.Buffer, Shaders->StageName.Size); \
						glSpecializeShader(StageName##ID, Constants::ENTRY_POINT_NAME, 0, nullptr, nullptr); \
						int32 result; \
						glGetShaderiv(StageName##ID, GL_COMPILE_STATUS, &result); \
						if (result == GL_FALSE) \
						{ \
							const int16 MessageSize = 1024; \
							static char8 message[MessageSize]; \
							int32 len = MessageSize; \
							glGetShaderInfoLog(StageName##ID, MessageSize, &len, message); \
							*ErrorMessage = message; \
							glDeleteShader(StageName##ID); \
							glDeleteProgram(handle); \
							return false; \
						} \
						glAttachShader(handle, StageName##ID); \
					}

				uint32 handle = glCreateProgram();

				IMPLEMENT_CREATE_SHADER(GL_VERTEX_SHADER, VertexShader);
				IMPLEMENT_CREATE_SHADER(GL_TESS_CONTROL_SHADER, TessellationShader);
				IMPLEMENT_CREATE_SHADER(GL_GEOMETRY_SHADER, GeometryShader);
				IMPLEMENT_CREATE_SHADER(GL_FRAGMENT_SHADER, FragmentShader);
				IMPLEMENT_CREATE_SHADER(GL_COMPUTE_SHADER, ComputeShader);

				glLinkProgram(handle);

				int32 result;
				glGetProgramiv(handle, GL_LINK_STATUS, &result);
				if (result == GL_FALSE)
				{
					const int16 MessageSize = 1024;
					static char8 message[MessageSize];

					int32 len = MessageSize;
					glGetProgramInfoLog(handle, MessageSize, &len, message);

					if (VertexShaderID != 0)
						glDeleteShader(VertexShaderID);

					if (TessellationShaderID != 0)
						glDeleteShader(TessellationShaderID);

					if (GeometryShaderID != 0)
						glDeleteShader(GeometryShaderID);

					if (FragmentShaderID != 0)
						glDeleteShader(FragmentShaderID);

					if (ComputeShaderID != 0)
						glDeleteShader(ComputeShaderID);

					*ErrorMessage = message;

					return false;
				}

				ProgramInfo* info = RenderSystemAllocators::ResourceAllocator_Allocate<ProgramInfo>();
				info->Handle = handle;

				if (VertexShaderID != 0)
					glDetachShader(handle, VertexShaderID);

				if (TessellationShaderID != 0)
					glDetachShader(handle, TessellationShaderID);

				if (GeometryShaderID != 0)
					glDetachShader(handle, GeometryShaderID);

				if (FragmentShaderID != 0)
					glDetachShader(handle, FragmentShaderID);

				if (ComputeShaderID != 0)
					glDetachShader(handle, ComputeShaderID);

				Handle = (ResourceHandle)info;

				return true;

#undef IMPLEMENT_CREATE_SHADER
			}

			bool OpenGLDevice::DestroyProgram(ResourceHandle Handle)
			{
				if (Handle == 0)
					return false;

				ProgramInfo* info = ReinterpretCast(ProgramInfo*, Handle);

				glDeleteProgram(info->Handle);

				RenderSystemAllocators::ResourceAllocator_Deallocate(info);

				return true;
			}

			bool OpenGLDevice::CreateTexture(const TextureInfo* Info, ResourceHandle& Handle)
			{
				uint32 handle;
				glGenTextures(1, &handle);

				TextureBufferInfo* info = RenderSystemAllocators::ResourceAllocator_Allocate<TextureBufferInfo>();
				Handle = (ResourceHandle)info;

				INITIALIZE_BUFFER_INFO(info, handle, Helper::GetTextureBufferSize(Info->Format, Info->Dimension), GPUBufferTypes::Pixel, false);
				info->TextureType = Info->Type;
				info->Width = Info->Dimension.X;
				info->Height = Info->Dimension.Y;
				info->Format = Info->Format;

				uint32 type = GetTextureType(Info->Type);

				glBindTexture(type, handle);

				if (Helper::GetTextureChannelCount(Info->Format) == 4)
					glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
				else
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

				glTexImage2D(type, 0, GetTextureInternalFormat(Info->Format), Info->Dimension.X, Info->Dimension.Y, 0, GetTextureFormat(Info->Format), GetTexturePixelType(Info->Format), Info->Data);

				SetTextureMinifyFilter(Handle, TextureMinifyFilters::Linear);
				SetTextureMagnifyFilter(Handle, TextureMagnfyFilters::Linear);

				return true;
			}

			bool OpenGLDevice::DestroyTexture(ResourceHandle Handle)
			{
				if (Handle == 0)
					return false;

				TextureBufferInfo* info = ReinterpretCast(TextureBufferInfo*, Handle);

				glDeleteTextures(1, &info->Handle);

				RenderSystemAllocators::ResourceAllocator_Deallocate(info);

				return true;
			}

			bool OpenGLDevice::SetTextureVerticalWrapping(ResourceHandle Handle, TextureWrapModes Mode)
			{
				if (Handle == 0)
					return false;

				TextureBufferInfo* info = ReinterpretCast(TextureBufferInfo*, Handle);

				uint32 type = GetTextureType(info->TextureType);

				glBindTexture(type, info->Handle);

				glTexParameteri(type, GL_TEXTURE_WRAP_T, GetWrapMode(Mode));

				return true;
			}

			bool OpenGLDevice::SetTextureHorizontalWrapping(ResourceHandle Handle, TextureWrapModes Mode)
			{
				if (Handle == 0)
					return false;

				TextureBufferInfo* info = ReinterpretCast(TextureBufferInfo*, Handle);

				uint32 type = GetTextureType(info->TextureType);

				glBindTexture(type, info->Handle);

				glTexParameteri(type, GL_TEXTURE_WRAP_S, GetWrapMode(Mode));

				return true;
			}

			bool OpenGLDevice::SetTextureMinifyFilter(ResourceHandle Handle, TextureMinifyFilters Filter)
			{
				if (Handle == 0)
					return false;

				TextureBufferInfo* info = ReinterpretCast(TextureBufferInfo*, Handle);

				uint32 type = GetTextureType(info->TextureType);

				glBindTexture(type, info->Handle);

				glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GetMinifyFilter(Filter));

				return true;
			}

			bool OpenGLDevice::SetTextureMagnifyFilter(ResourceHandle Handle, TextureMagnfyFilters Filter)
			{
				if (Handle == 0)
					return false;

				TextureBufferInfo* info = ReinterpretCast(TextureBufferInfo*, Handle);

				uint32 type = GetTextureType(info->TextureType);

				glBindTexture(type, info->Handle);

				glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GetMagnifyFilter(Filter));

				return true;
			}

			bool OpenGLDevice::CreateRenderTarget(const RenderTargetInfo* Info, ResourceHandle& Handle, TextureList& Textures)
			{
				if (Info->Textures.GetSize() == 0)
					return false;

				RenderTargetBufferInfo* renderTargetInfo = RenderSystemAllocators::ResourceAllocator_Allocate<RenderTargetBufferInfo>();
				PlatformMemory::Set(renderTargetInfo, 0, 1);

				uint32 handle;
				glGenFramebuffers(1, &handle);
				renderTargetInfo->Handle = handle;

				Handle = (ResourceHandle)renderTargetInfo;

				glBindFramebuffer(GL_FRAMEBUFFER, handle);

				static uint32 drawBuffers[((int8)AttachmentPoints::Color8 - (int8)AttachmentPoints::Color0) + 1];

				int drawBufferIndex = 0;

				for (const auto& textureInfo : Info->Textures)
				{
					TextureInfo info;
					info.Type = TextureTypes::TwoD;
					info.Dimension = textureInfo.Dimension;
					info.Format = textureInfo.Format;
					info.Data = nullptr;

					ResourceHandle texHandle;
					if (!CreateTexture(&info, texHandle))
						return false;

					uint32 point = GetAttachmentPoint(textureInfo.Point);

					TextureBufferInfo* texInfo = ReinterpretCast(TextureBufferInfo*, texHandle);
					glFramebufferTexture2D(GL_FRAMEBUFFER, point, GetTextureType(info.Type), texInfo->Handle, 0);

					renderTargetInfo->Textures.Add(texHandle);

					if (textureInfo.Point >= AttachmentPoints::Color0)
						drawBuffers[drawBufferIndex++] = point;
				}

				Textures.AddRange(renderTargetInfo->Textures);

				glDrawBuffers(drawBufferIndex, drawBuffers);

				glBindFramebuffer(GL_FRAMEBUFFER, 0);

				return true;
			}

			bool OpenGLDevice::DestroyRenderTarget(ResourceHandle Handle)
			{
				if (Handle == 0)
					return false;

				RenderTargetBufferInfo* renderTargetInfo = ReinterpretCast(RenderTargetBufferInfo*, Handle);

				for (auto handle : renderTargetInfo->Textures)
					DestroyTexture(handle);

				uint32 handle = Handle;
				glDeleteFramebuffers(1, &handle);

				RenderSystemAllocators::ResourceAllocator_Deallocate(renderTargetInfo);

				return true;
			}

			bool OpenGLDevice::CreateMesh(const SubMeshInfo* Info, ResourceHandle& Handle)
			{
#define IMPLEMENT_UPLOAD_BUFFER(BufferHandle, Type, Size, Data) \
					uint32 target = GetBufferType(Type); \
					glBindBuffer(target, ReinterpretCast(BufferInfo*, BufferHandle)->Handle); \
					glBufferData(target, Size, Data, GL_STATIC_DRAW); \
					glBindBuffer(target, 0);

				if (Info->Vertices.GetSize() == 0)
					return false;

				uint32 size = Helper::GetMeshVertexBufferSize(Info->Vertices.GetSize());

				ResourceHandle vbo;
				if (!CreateBufferInternal(GPUBufferTypes::Vertex, size, false, vbo))
					return false;

				IMPLEMENT_UPLOAD_BUFFER(vbo, GPUBufferTypes::Vertex, size, Info->Vertices.GetData());

				ResourceHandle ebo = 0;
				if (Info->Indices.GetSize() != 0)
				{
					size = Helper::GetMeshIndexBufferSize(Info->Indices.GetSize());

					if (!CreateBufferInternal(GPUBufferTypes::Index, size, false, ebo))
						return false;

					IMPLEMENT_UPLOAD_BUFFER(ebo, GPUBufferTypes::Index, size, Info->Indices.GetData());
				}

				MeshBufferInfo* meshBufferInfo = RenderSystemAllocators::ResourceAllocator_Allocate<MeshBufferInfo>();
				PlatformMemory::Set(meshBufferInfo, 0, 1);

				meshBufferInfo = ReinterpretCast(MeshBufferInfo*, vbo);
				if (ebo != 0)
					meshBufferInfo->IndexBufferObject = ReinterpretCast(BufferInfo*, ebo);
				meshBufferInfo->Layout = Info->Layout;

				Handle = (ResourceHandle)meshBufferInfo;

				return true;

#undef IMPLEMENT_UPLOAD_BUFFER
			}

			bool OpenGLDevice::DestroyMesh(ResourceHandle Handle)
			{
				if (Handle == 0)
					return false;

				MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Handle);

				if (meshBufferInfo->IndexBufferObject != nullptr)
					DestroyBuffer((ResourceHandle)meshBufferInfo->IndexBufferObject);

				DestroyBuffer((ResourceHandle)meshBufferInfo);

				RenderContextInfo* currentInfo = m_CurrentContext;

				for (auto& item : m_Contexts)
				{
					auto* context = item.GetSecond();

					if (!context->IsActive)
						SetContext(item.GetFirst());

					if (!context->VertexArrays.Contains(meshBufferInfo))
						continue;

					uint32 vao = context->VertexArrays[meshBufferInfo];
					glDeleteVertexArrays(1, &vao);

					context->VertexArrays.Remove(meshBufferInfo);
				}

				if (m_CurrentContext != currentInfo)
					SetContext(m_CurrentContextHandle);

				RenderSystemAllocators::ResourceAllocator_Deallocate(meshBufferInfo);

				return true;
			}

			bool OpenGLDevice::CreateCommandBuffer(ICommandBuffer::Types Type, ICommandBuffer*& Buffer)
			{
				Buffer = m_CommandBufferPool.Get(this, Type);
				Buffer->SetName(L"Command Buffer");

				return true;
			}

			bool OpenGLDevice::DestroyCommandBuffer(ICommandBuffer** Buffers, uint16 Count)
			{
				for (uint16 i = 0; i < Count; ++i)
					m_CommandBufferPool.Back(Buffers[i]);

				return true;
			}

			bool OpenGLDevice::SubmitCommandBuffer(ICommandBuffer* const* Buffers, uint16 Count)
			{
				OpenGLCommandBuffer** buffers = ReinterpretCast(OpenGLCommandBuffer**, ConstCast(ICommandBuffer**, Buffers));

				for (uint16 i = 0; i < Count; ++i)
					if (!buffers[i]->Execute())
						return false;

				return true;
			}

			bool OpenGLDevice::SetResourceName(ResourceHandle Handle, ResourceTypes Type, cwstr Name)
			{
				char8 name[128];
				CharacterUtility::ChangeType(Name, name);

				if (Type == ResourceTypes::Buffer)
				{
					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

					glObjectLabel(GL_BUFFER, info->Handle, -1, name);
				}
				else if (Type == ResourceTypes::Program)
				{
					ProgramInfo* info = ReinterpretCast(ProgramInfo*, Handle);

					glObjectLabel(GL_PROGRAM, info->Handle, -1, name);
				}
				else if (Type == ResourceTypes::Mesh)
				{
					MeshBufferInfo* info = ReinterpretCast(MeshBufferInfo*, Handle);

					String tempName(name);

					glObjectLabel(GL_BUFFER, info->Handle, -1, (tempName + "_Vertex").GetValue());

					if (info->IndexBufferObject != nullptr)
						glObjectLabel(GL_BUFFER, info->IndexBufferObject->Handle, -1, (tempName + "_Index").GetValue());
				}
				else if (Type == ResourceTypes::Texture)
				{
					TextureBufferInfo* info = ReinterpretCast(TextureBufferInfo*, Handle);

					glObjectLabel(GL_TEXTURE, info->Handle, -1, name);
				}
				else if (Type == ResourceTypes::RenderTarget)
				{
					RenderTargetBufferInfo* info = ReinterpretCast(RenderTargetBufferInfo*, Handle);

					String tempName(name);

					glObjectLabel(GL_FRAMEBUFFER, info->Handle, -1, (tempName + "_Frames").GetValue());

					uint8 index = 0;
					for (auto texture : info->Textures)
					{
						TextureBufferInfo* texInfo = ReinterpretCast(TextureBufferInfo*, texture);

						glObjectLabel(GL_TEXTURE, texInfo->Handle, -1, (tempName + "_Texture_" + StringUtility::ToString<char8>(index++)).GetValue());
					}
				}

				return true;
			}

			bool OpenGLDevice::InitializeBaseContext(void)
			{
				m_BaseContextWindow = RenderSystemAllocators::RenderSystemAllocator_Allocate<Window>();
				Construct(m_BaseContextWindow, "DummyContextWindow");
				m_BaseContextWindow->Initialize();
				m_BaseContextWindow->SetIsVisible(false);

				if (!CreateContext(m_BaseContextWindow->GetHandle(), m_BaseContextHandle))
					return false;

				return SetContext(m_BaseContextHandle);
			}
		}
	}
}