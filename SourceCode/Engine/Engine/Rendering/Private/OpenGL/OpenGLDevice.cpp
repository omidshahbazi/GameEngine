// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\OpenGL\OpenGLDevice.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Rendering\Private\Helper.h>
#include <Rendering\Private\ProgramCompiler\Compiler.h>
#include <Rendering\Private\ProgramCompiler\GLSLANGCompiler.h>
#include <Containers\StringUtility.h>
#include <Debugging\Debug.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <WindowUtility\Window.h>
#include <GL\glew.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Platform;
	using namespace MemoryManagement::Allocator;
	using namespace WindowUtility;

	namespace Rendering
	{
		namespace Private
		{
			using namespace ProgramCompiler;

			namespace OpenGL
			{
#define SET_IF_ENABLED(CheckFlagVariable, CheckFlag, FlagVariable, Flag) \
		if ((CheckFlagVariable & CheckFlag) == CheckFlag) \
			FlagVariable |= Flag;

#if DEBUG_MODE
#define CHECK_FAILED() \
	{ \
		GLenum __error = glGetError(); \
		Assert(__error == 0, "OpenGL call failed"); \
	}
#else
#define CHECK_FAILED()
#endif

#define INITIALIZE_BUFFER_INFO(BufferInfoPtr, GLHandle) \
				(BufferInfoPtr)->Handle = GLHandle;

				const uint16 LAST_ERROR_SIZE = 512;

				uint32 GetClearingFlags(IDevice::ClearFlags Flags)
				{
					uint32 flags = 0;

					SET_IF_ENABLED(Flags, IDevice::ClearFlags::ColorBuffer, flags, GL_COLOR_BUFFER_BIT);
					SET_IF_ENABLED(Flags, IDevice::ClearFlags::DepthBuffer, flags, GL_DEPTH_BUFFER_BIT);
					SET_IF_ENABLED(Flags, IDevice::ClearFlags::StencilBuffer, flags, GL_STENCIL_BUFFER_BIT);

					return flags;
				}

				uint32 GetBufferType(GPUBuffer::Types Type)
				{
					switch (Type)
					{
					case GPUBuffer::Types::Constant:
						return GL_UNIFORM_BUFFER;
					case GPUBuffer::Types::Vertex:
						return GL_ARRAY_BUFFER;
					case GPUBuffer::Types::Index:
						return GL_ELEMENT_ARRAY_BUFFER;
					case GPUBuffer::Types::Pixel:
						return GL_PIXEL_UNPACK_BUFFER;
					}

					return 0;
				}

				uint32 GetBufferAccess(GPUBuffer::Access Type)
				{
					switch (Type)
					{
					case GPUBuffer::Access::ReadOnly:
						return GL_READ_ONLY;
					case GPUBuffer::Access::WriteOnly:
						return GL_WRITE_ONLY;
					case GPUBuffer::Access::ReadAndWrite:
						return GL_READ_WRITE;
					}

					return 0;
				}

				uint32 GetPolygonType(SubMesh::PolygonTypes PolygonType)
				{
					switch (PolygonType)
					{
					case SubMesh::PolygonTypes::Lines:
						return GL_LINES;
					case SubMesh::PolygonTypes::LineLoop:
						return GL_LINE_LOOP;
					case SubMesh::PolygonTypes::LineStrip:
						return GL_LINE_STRIP;

					case SubMesh::PolygonTypes::Triangles:
						return GL_TRIANGLES;
					case SubMesh::PolygonTypes::TriangleStrip:
						return GL_TRIANGLE_STRIP;
					case SubMesh::PolygonTypes::TriangleFan:
						return GL_TRIANGLE_FAN;

					case SubMesh::PolygonTypes::Quads:
						return GL_QUADS;
					case SubMesh::PolygonTypes::QuadStrip:
						return GL_QUAD_STRIP;

					case SubMesh::PolygonTypes::Polygon:
						return GL_POLYGON;
					}

					return 0;
				}

				uint32 GetFaceOrdering(IDevice::FaceOrders Order)
				{
					switch (Order)
					{
					case IDevice::FaceOrders::Clockwise:
						return GL_CW;
					case IDevice::FaceOrders::CounterClockwise:
						return GL_CCW;
					}

					return 0;
				}

				uint32 GetCullingMode(IDevice::CullModes Modes)
				{
					if (BitwiseUtils::IsEnabled(Modes, IDevice::CullModes::Front) && BitwiseUtils::IsEnabled(Modes, IDevice::CullModes::Back))
						return GL_FRONT_AND_BACK;

					if (BitwiseUtils::IsEnabled(Modes, IDevice::CullModes::Front))
						return GL_FRONT;

					if (BitwiseUtils::IsEnabled(Modes, IDevice::CullModes::Back))
						return GL_BACK;

					return 0;
				}

				uint32 GetTestFunction(IDevice::TestFunctions Function)
				{
					switch (Function)
					{
					case IDevice::TestFunctions::Never:
						return GL_NEVER;
					case IDevice::TestFunctions::Less:
						return GL_LESS;
					case IDevice::TestFunctions::LessEqual:
						return GL_LEQUAL;
					case IDevice::TestFunctions::Equal:
						return GL_EQUAL;
					case IDevice::TestFunctions::NotEqual:
						return GL_NOTEQUAL;
					case IDevice::TestFunctions::GreaterEqual:
						return GL_GEQUAL;
					case IDevice::TestFunctions::Greater:
						return GL_GREATER;
					case IDevice::TestFunctions::Always:
						return GL_ALWAYS;
					}

					return 0;
				}

				uint32 GetStencilingOperation(IDevice::StencilOperations Operation)
				{
					switch (Operation)
					{
					case IDevice::StencilOperations::Keep:
						return GL_KEEP;
					case IDevice::StencilOperations::Zero:
						return GL_ZERO;
					case IDevice::StencilOperations::Replace:
						return GL_REPLACE;
					case IDevice::StencilOperations::Increament:
						return GL_INCR;
					case IDevice::StencilOperations::IncreamentWrap:
						return GL_INCR_WRAP;
					case IDevice::StencilOperations::Decreament:
						return GL_DECR;
					case IDevice::StencilOperations::DecreamentWrap:
						return GL_DECR_WRAP;
					case IDevice::StencilOperations::Invert:
						return GL_INVERT;
					}

					return 0;
				}

				uint32 GetBlendingEquation(IDevice::BlendEquations Equation)
				{
					switch (Equation)
					{
					case IDevice::BlendEquations::Add:
						return GL_FUNC_ADD;
					case IDevice::BlendEquations::Subtract:
						return GL_FUNC_SUBTRACT;
					case IDevice::BlendEquations::ReverseSubtract:
						return GL_FUNC_REVERSE_SUBTRACT;
					case IDevice::BlendEquations::Min:
						return GL_MIN;
					case IDevice::BlendEquations::Max:
						return GL_MAX;
					}

					return 0;
				}

				uint32 GetBlendingFunction(IDevice::BlendFunctions Function)
				{
					switch (Function)
					{
					case IDevice::BlendFunctions::Zero:
						return GL_ZERO;
					case IDevice::BlendFunctions::One:
						return GL_ONE;
					case IDevice::BlendFunctions::SourceColor:
						return GL_SRC_COLOR;
					case IDevice::BlendFunctions::OneMinusSourceColor:
						return GL_ONE_MINUS_SRC_COLOR;
					case IDevice::BlendFunctions::DestinationColor:
						return GL_DST_COLOR;
					case IDevice::BlendFunctions::OneMinusDestinationColor:
						return GL_ONE_MINUS_DST_COLOR;
					case IDevice::BlendFunctions::SourceAlpha:
						return GL_SRC_ALPHA;
					case IDevice::BlendFunctions::OneMinusSourceAlpha:
						return GL_ONE_MINUS_SRC_ALPHA;
					case IDevice::BlendFunctions::DestinationAlpha:
						return GL_DST_ALPHA;
					case IDevice::BlendFunctions::OneMinusDestinationAlpha:
						return GL_ONE_MINUS_DST_ALPHA;
					case IDevice::BlendFunctions::ConstantColor:
						return GL_CONSTANT_COLOR;
					case IDevice::BlendFunctions::OneMinusConstantColor:
						return GL_ONE_MINUS_CONSTANT_COLOR;
					case IDevice::BlendFunctions::ConstantAlpha:
						return GL_CONSTANT_ALPHA;
					case IDevice::BlendFunctions::OneMinusConstantAlpha:
						return GL_ONE_MINUS_CONSTANT_ALPHA;
					}

					return 0;
				}

				uint32 GetTextureType(Texture::Types Type)
				{
					switch (Type)
					{
					case Texture::Types::TwoD:
						return GL_TEXTURE_2D;
					}

					return 0;
				}

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

				uint32 GetTextureFormat(Formats Format)
				{
					switch (Format)
					{
					case Formats::R8:
						return GL_RED;
					case Formats::R16:
						return GL_RED;
					case Formats::R32:
						return GL_RED;
					case Formats::R16F:
						return GL_RED;
					case Formats::R32F:
						return GL_RED;
					case Formats::RG8:
						return GL_RG;
					case Formats::RG16:
						return GL_RG;
					case Formats::RG32:
						return GL_RG;
					case Formats::RG16F:
						return GL_RG;
					case Formats::RG32F:
						return GL_RG;
					case Formats::RGB8:
						return GL_RGB;
					case Formats::RGB16:
						return GL_RGB;
					case Formats::RGB32:
						return GL_RGB;
					case Formats::RGB16F:
						return GL_RGB;
					case Formats::RGB32F:
						return GL_RGB;
					case Formats::RGBA8:
						return GL_RGBA;
					case Formats::RGBA16:
						return GL_RGBA;
					case Formats::RGBA32:
						return GL_RGBA;
					case Formats::RGBA16F:
						return GL_RGBA;
					case Formats::RGBA32F:
						return GL_RGBA;
					case Formats::Depth16:
						return GL_DEPTH_COMPONENT;
					case Formats::Depth24:
						return GL_DEPTH_COMPONENT;
					case Formats::Depth32:
						return GL_DEPTH_COMPONENT;
					case Formats::Depth32F:
						return GL_DEPTH_COMPONENT;
					case Formats::DepthStencil24F:
						return GL_DEPTH_STENCIL;
					case Formats::DepthStencil32F:
						return GL_DEPTH_STENCIL;
					}

					return 0;
				}

				uint32 GetTexturePixelType(Formats Format)
				{
					switch (Format)
					{
					case Formats::R8:
						return GL_UNSIGNED_BYTE;
					case Formats::R16:
						return GL_UNSIGNED_SHORT;
					case Formats::R32:
						return GL_UNSIGNED_INT;
					case Formats::R16F:
						return GL_HALF_FLOAT;
					case Formats::R32F:
						return GL_FLOAT;
					case Formats::RG8:
						return GL_UNSIGNED_BYTE;
					case Formats::RG16:
						return GL_UNSIGNED_SHORT;
					case Formats::RG32:
						return GL_UNSIGNED_INT;
					case Formats::RG16F:
						return GL_HALF_FLOAT;
					case Formats::RG32F:
						return GL_FLOAT;
					case Formats::RGB8:
						return GL_UNSIGNED_BYTE;
					case Formats::RGB16:
						return GL_UNSIGNED_INT;
					case Formats::RGB32:
						return GL_UNSIGNED_INT;
					case Formats::RGB16F:
						return GL_HALF_FLOAT;
					case Formats::RGB32F:
						return GL_FLOAT;
					case Formats::RGBA8:
						return GL_UNSIGNED_BYTE;
					case Formats::RGBA16:
						return GL_UNSIGNED_SHORT;
					case Formats::RGBA32:
						return GL_UNSIGNED_INT;
					case Formats::RGBA16F:
						return GL_HALF_FLOAT;
					case Formats::RGBA32F:
						return GL_FLOAT;
					case Formats::Depth16:
						return GL_UNSIGNED_SHORT;
					case Formats::Depth24:
						return GL_UNSIGNED_INT;
					case Formats::Depth32:
						return GL_UNSIGNED_INT;
					case Formats::Depth32F:
						return GL_FLOAT;
					case Formats::DepthStencil24F:
						return GL_UNSIGNED_INT_24_8;
					case Formats::DepthStencil32F:
						return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
					}

					return 0;
				}

				uint32 GetWrapMode(Texture::WrapModes Mode)
				{
					switch (Mode)
					{
					case Texture::WrapModes::Clamp:
						return GL_CLAMP;
					case Texture::WrapModes::Repeat:
						return GL_REPEAT;
					case Texture::WrapModes::ClampToEdge:
						return GL_CLAMP_TO_EDGE;
					case Texture::WrapModes::MirroredRepeat:
						return GL_MIRRORED_REPEAT;
					}

					return 0;
				}

				uint32 GetMinifyFilter(Texture::MinifyFilters Filter)
				{
					switch (Filter)
					{
					case Texture::MinifyFilters::Nearest:
						return GL_NEAREST;
					case Texture::MinifyFilters::Linear:
						return GL_LINEAR;
					case Texture::MinifyFilters::NearestMipMapNearest:
						return GL_NEAREST_MIPMAP_NEAREST;
					case Texture::MinifyFilters::LinearMipMapNearest:
						return GL_LINEAR_MIPMAP_NEAREST;
					case Texture::MinifyFilters::NearestMipMapLinear:
						return GL_NEAREST_MIPMAP_LINEAR;
					case Texture::MinifyFilters::LinearMipMapLinear:
						return GL_LINEAR_MIPMAP_LINEAR;
					}

					return 0;
				}

				uint32 GetMagnifyFilter(Texture::MagnfyFilters Filter)
				{
					switch (Filter)
					{
					case Texture::MagnfyFilters::Nearest:
						return GL_NEAREST;
					case Texture::MagnfyFilters::Linear:
						return GL_LINEAR;
					}

					return 0;
				}

				uint32 GetPolygonRenderMode(IDevice::PolygonModes Mode)
				{
					switch (Mode)
					{
					case IDevice::PolygonModes::Point:
						return GL_POINT;
					case IDevice::PolygonModes::Line:
						return GL_LINE;
					case IDevice::PolygonModes::Fill:
						return GL_FILL;
					}

					return 0;
				}

				uint32 GetAttachmentPoint(RenderTarget::AttachmentPoints Point)
				{
					switch (Point)
					{
					case RenderTarget::AttachmentPoints::Depth: return GL_DEPTH_ATTACHMENT;
					case RenderTarget::AttachmentPoints::DepthStencil: return GL_DEPTH_STENCIL_ATTACHMENT;
					case RenderTarget::AttachmentPoints::Color0: return GL_COLOR_ATTACHMENT0;
					case RenderTarget::AttachmentPoints::Color1: return GL_COLOR_ATTACHMENT1;
					case RenderTarget::AttachmentPoints::Color2: return GL_COLOR_ATTACHMENT2;
					case RenderTarget::AttachmentPoints::Color3: return GL_COLOR_ATTACHMENT3;
					case RenderTarget::AttachmentPoints::Color4: return GL_COLOR_ATTACHMENT4;
					case RenderTarget::AttachmentPoints::Color5: return GL_COLOR_ATTACHMENT5;
					case RenderTarget::AttachmentPoints::Color6: return GL_COLOR_ATTACHMENT6;
					case RenderTarget::AttachmentPoints::Color7: return GL_COLOR_ATTACHMENT7;
					case RenderTarget::AttachmentPoints::Color8: return GL_COLOR_ATTACHMENT8;
					}

					return 0;
				}

				void GLAPIENTRY DebugOutputProcedure(GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei Length, const GLchar* Message, const GLvoid* Param)
				{
					//if (ID == 131169 || ID == 131185 || ID == 131218 || ID == 131204)
					//	return;

					OpenGLDevice* device = ConstCast(OpenGLDevice*, ReinterpretCast(const OpenGLDevice*, Param));
					IDevice::DebugFunction procedure = device->GetDebugCallback();
#ifndef DEBUG_MODE
					if (procedure == nullptr)
						return true;
#endif

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

#ifdef DEBUG_MODE
					if (severity == IDevice::DebugSeverities::High)
						Debug::LogError(Message);
					else if (severity == IDevice::DebugSeverities::Medium)
						Debug::LogWarning(Message);
					else
						Debug::LogInfo(Message);

					if (procedure != nullptr)
#endif
						procedure(ID, source, Message, type, severity);
				}

				OpenGLDevice::OpenGLDevice(void) :
					m_Initialized(false),
					m_BaseContextHandle(0),
					m_BaseContextWindow(nullptr),
					m_BaseContext(nullptr),
					m_CurrentContextHandle(0),
					m_CurrentContext(nullptr),
					m_LastProgram(0),
					m_LastFrameBuffer(0)
				{
				}

				OpenGLDevice::~OpenGLDevice(void)
				{
					DestroyContext(m_BaseContextHandle);
					RenderingAllocators::RenderingSystemAllocator_Deallocate(ReinterpretCast(Window*, m_BaseContextWindow));
				}

				bool OpenGLDevice::Initialize(void)
				{
					Assert(!m_Initialized, "OpenGLDevice already initialized");

					if (!InitializeBaseContext())
						return false;

#ifdef DEBUG_MODE
					glEnable(GL_DEBUG_OUTPUT);
					glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

					glDebugMessageCallback(DebugOutputProcedure, this);

					glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
#endif

					ResetState();

					m_Initialized = true;

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

				bool OpenGLDevice::CreateContext(PlatformWindow::WindowHandle WindowHandle, RenderContext::Handle& Handle)
				{
					if (WindowHandle == 0)
						return false;

					for (auto& item : m_Contexts)
						if (item.GetFirst() == (RenderContext::Handle)WindowHandle)
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

					Handle = (RenderContext::Handle)WindowHandle;

					RenderContextInfo* info = RenderingAllocators::ResourceAllocator_Allocate<RenderContextInfo>();
					Construct(info);

					info->ContextHandle = contextHandle;
					info->WGLContextHandle = wglContextHandle;
					info->LastMeshHandle = 0;
					info->IsActive = false;

					m_Contexts[Handle] = info;

					if (m_BaseContext == nullptr)
						m_BaseContext = info;

					return true;
				}

				bool OpenGLDevice::DestroyContext(RenderContext::Handle Handle)
				{
					if (Handle == 0)
						return true;

					if (!m_Contexts.Contains(Handle))
						return false;

					RenderContextInfo* info = m_Contexts[Handle];

					if (m_CurrentContextHandle == Handle)
						SetContext(0);

					PlatformGL::DestroyWGLContext(info->WGLContextHandle);

					RenderingAllocators::ResourceAllocator_Deallocate(info);

					m_Contexts.Remove(Handle);

					return true;
				}

				bool OpenGLDevice::SetContext(RenderContext::Handle Handle)
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

					info->LastMeshHandle = 0;
					info->IsActive = true;

					//https://www.khronos.org/opengl/wiki/Multisampling
					//https://learnopengl.com/Advanced-OpenGL/Anti-Aliasing
					//glEnable(GL_MULTISAMPLE);

					ResetState();

					return true;
				}

				bool OpenGLDevice::SetContextSize(const Vector2I& Size)
				{
					return true;
				}

				bool OpenGLDevice::SetViewport(const Vector2I& Position, const Vector2I& Size)
				{
					glViewport(Position.X, Position.Y, Size.X, Size.Y);

					return true;
				}

				bool OpenGLDevice::SetResourceName(NativeType::Handle Handle, ResourceTypes Type, cwstr Name)
				{
					char8 name[128];
					CharacterUtility::ChangeType(Name, name);

					if (Type == ResourceTypes::Buffer)
					{
						BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

						glObjectLabel(GL_BUFFER, info->Handle, -1, name);
					}
					if (Type == ResourceTypes::Program)
					{
						ProgramInfo* info = ReinterpretCast(ProgramInfo*, Handle);

						glObjectLabel(GL_PROGRAM, info->Handle, -1, name);
					}
					else if (Type == ResourceTypes::Mesh)
					{
						MeshBufferInfo* info = ReinterpretCast(MeshBufferInfo*, Handle);

						String tempName(name);

						glObjectLabel(GL_BUFFER, info->VertexBufferObject->Handle, -1, (tempName + "_VertexBuffer").GetValue());

						if (info->IndexBufferObject != nullptr)
							glObjectLabel(GL_BUFFER, info->IndexBufferObject->Handle, -1, (tempName + "_IndexBuffer").GetValue());
					}
					else if (Type == ResourceTypes::Texture)
					{
						BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

						glObjectLabel(GL_TEXTURE, info->Handle, -1, name);
					}
					else if (Type == ResourceTypes::RenderTarget)
					{
						RenderTargetInfos* info = ReinterpretCast(RenderTargetInfos*, Handle);

						String tempName(name);

						glObjectLabel(GL_FRAMEBUFFER, info->Handle, -1, (tempName + "_FrameBuffer").GetValue());

						uint8 index = 0;
						for (auto texture : info->Textures)
						{
							BufferInfo* texInfo = ReinterpretCast(BufferInfo*, texture);

							glObjectLabel(GL_TEXTURE, texInfo->Handle, -1, (tempName + "_TextureBuffer_" + StringUtility::ToString<char8>(index++)).GetValue());
						}
					}

					return true;
				}

				void OpenGLDevice::SetState(const State& State)
				{
#define SET_STENCIL_STATE(CullMode) \
					{ \
						State::FaceState& state = m_State.GetFaceState(CullMode); \
						const State::FaceState& otherState = State.GetFaceState(CullMode); \
						if (otherState.StencilTestFunction != state.StencilTestFunction || otherState.StencilTestFunctionReference != state.StencilTestFunctionReference || otherState.StencilTestFunctionMask != state.StencilTestFunctionMask) \
						{ \
							if (otherState.StencilTestFunction == TestFunctions::Never) \
								glDisable(GL_STENCIL_TEST); \
							else \
							{ \
								glEnable(GL_STENCIL_TEST); \
								glStencilFuncSeparate(GetCullingMode(CullMode), GetTestFunction(otherState.StencilTestFunction), otherState.StencilTestFunctionReference, otherState.StencilTestFunctionMask); \
							} \
						}\
						if (otherState.StencilMask != state.StencilMask) \
							glStencilMaskSeparate(GetCullingMode(CullMode), otherState.StencilMask);\
						if (otherState.StencilOperationStencilFailed != state.StencilOperationStencilFailed || otherState.StencilOperationDepthFailed != state.StencilOperationDepthFailed || otherState.StencilOperationDepthPassed != state.StencilOperationDepthPassed) \
							glStencilOpSeparate(GetCullingMode(CullMode), GetStencilingOperation(otherState.StencilOperationStencilFailed), GetStencilingOperation(otherState.StencilOperationDepthFailed), GetStencilingOperation(otherState.StencilOperationDepthPassed)); \
						if (otherState.PolygonMode != state.PolygonMode) \
							glPolygonMode(GetCullingMode(CullMode), GetPolygonRenderMode(otherState.PolygonMode)); \
					}

					if (State.FaceOrder != m_State.FaceOrder)
						glFrontFace(GetFaceOrdering(State.FaceOrder));

					if (State.CullMode != m_State.CullMode)
					{
						if (State.CullMode == CullModes::None)
							glDisable(GL_CULL_FACE);
						else
						{
							glEnable(GL_CULL_FACE);
							glCullFace(GetCullingMode(State.CullMode));
						}
					}

					if (State.DepthTestFunction != m_State.DepthTestFunction)
					{
						if (State.DepthTestFunction == TestFunctions::Never)
							glDisable(GL_DEPTH_TEST);
						else
						{
							glEnable(GL_DEPTH_TEST);
							glDepthFunc(GetTestFunction(State.DepthTestFunction));
						}
					}

					SET_STENCIL_STATE(CullModes::None);
					SET_STENCIL_STATE(CullModes::Back);
					SET_STENCIL_STATE(CullModes::Front);
					SET_STENCIL_STATE(CullModes::Both);

					if (State.BlendEquation != m_State.BlendEquation)
						glBlendEquation(GetBlendingEquation(m_State.BlendEquation));

					if (State.BlendFunctionSourceFactor != m_State.BlendFunctionSourceFactor || State.BlendFunctionDestinationFactor != m_State.BlendFunctionDestinationFactor)
					{
						if (State.BlendFunctionSourceFactor == BlendFunctions::One && State.BlendFunctionDestinationFactor == BlendFunctions::Zero)
							glDisable(GL_BLEND);
						else
						{
							glEnable(GL_BLEND);
							glBlendFunc(GetBlendingFunction(State.BlendFunctionSourceFactor), GetBlendingFunction(State.BlendFunctionDestinationFactor));
						}
					}

					PlatformMemory::Copy(&State, &m_State, 1);
				}

				bool OpenGLDevice::CreateBuffer(GPUBuffer::Handle& Handle)
				{
					BufferInfo* info = RenderingAllocators::ResourceAllocator_Allocate<BufferInfo>();

					uint32 handle;
					glGenBuffers(1, &handle);

					INITIALIZE_BUFFER_INFO(info, handle);

					Handle = (GPUBuffer::Handle)info;

					return true;
				}

				bool OpenGLDevice::DestroyBuffer(GPUBuffer::Handle Handle)
				{
					if (Handle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

					glDeleteBuffers(1, &info->Handle);

					RenderingAllocators::ResourceAllocator_Deallocate(info);

					return true;
				}

				bool OpenGLDevice::InitializeConstantBuffer(GPUBuffer::Handle Handle, const byte* Data, uint32 Size)
				{
					if (Handle == 0)
						return false;

					if (Size == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

					uint32 target = GetBufferType(GPUBuffer::Types::Constant);

					glBindBuffer(target, info->Handle);

					glBufferData(target, Size, Data, GL_STATIC_COPY);

					glBindBuffer(target, 0);

					return true;
				}

				bool OpenGLDevice::CopyFromVertexToBuffer(GPUBuffer::Handle Handle, SubMesh::Handle FromMeshHandle, uint32 Size)
				{
					if (Handle == 0)
						return false;

					if (FromMeshHandle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);
					MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, FromMeshHandle);

					GPUBuffer::Types type = GPUBuffer::Types::Vertex;
					uint32 target = GetBufferType(type);

					byte* buffer = nullptr;
					if (!LockBuffer(meshBufferInfo->VertexBufferObject, type, GPUBuffer::Access::ReadOnly, &buffer))
						return false;

					UnlockBuffer(meshBufferInfo->VertexBufferObject, type);

					glBindBuffer(target, info->Handle);

					glBufferData(target, Size, buffer, GL_STATIC_COPY);

					glBindBuffer(target, 0);

					return true;
				}

				bool OpenGLDevice::CopyFromBufferToVertex(GPUBuffer::Handle Handle, SubMesh::Handle ToMeshHandle, uint32 Size)
				{
					if (Handle == 0)
						return false;

					if (ToMeshHandle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);
					MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, ToMeshHandle);

					GPUBuffer::Types type = GPUBuffer::Types::Vertex;
					uint32 target = GetBufferType(type);

					byte* buffer = nullptr;
					if (!LockBuffer(info, type, GPUBuffer::Access::ReadOnly, &buffer))
						return false;

					UnlockBuffer(info, type);

					glBindBuffer(target, meshBufferInfo->VertexBufferObject->Handle);

					glBufferData(target, Size, buffer, GL_STATIC_COPY);

					glBindBuffer(target, 0);

					return true;
				}

				bool OpenGLDevice::CopyFromIndexToBuffer(GPUBuffer::Handle Handle, SubMesh::Handle FromMeshHandle, uint32 Size)
				{
					if (Handle == 0)
						return false;

					if (FromMeshHandle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);
					MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, FromMeshHandle);

					GPUBuffer::Types type = GPUBuffer::Types::Vertex;
					uint32 target = GetBufferType(type);

					byte* buffer = nullptr;
					if (!LockBuffer(meshBufferInfo->IndexBufferObject, type, GPUBuffer::Access::ReadOnly, &buffer))
						return false;

					UnlockBuffer(meshBufferInfo->IndexBufferObject, type);

					glBindBuffer(target, info->Handle);

					glBufferData(target, Size, buffer, GL_STATIC_COPY);

					glBindBuffer(target, 0);

					return true;
				}

				bool OpenGLDevice::CopyFromBufferToIndex(GPUBuffer::Handle Handle, SubMesh::Handle ToMeshHandle, uint32 Size)
				{
					if (Handle == 0)
						return false;

					if (ToMeshHandle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);
					MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, ToMeshHandle);

					GPUBuffer::Types type = GPUBuffer::Types::Index;
					uint32 target = GetBufferType(type);

					byte* buffer = nullptr;
					if (!LockBuffer(info, type, GPUBuffer::Access::ReadOnly, &buffer))
						return false;

					UnlockBuffer(info, type);

					glBindBuffer(target, meshBufferInfo->IndexBufferObject->Handle);

					glBufferData(target, Size, buffer, GL_STATIC_COPY);

					glBindBuffer(target, 0);

					return true;
				}

				bool OpenGLDevice::CopyFromTextureToBuffer(GPUBuffer::Handle Handle, Texture::Handle FromTextureHandle, uint32 Size, Texture::Types TextureType, Formats TextureFormat, uint32 Level)
				{
					if (Handle == 0)
						return false;

					if (FromTextureHandle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);
					BufferInfo* texInfo = ReinterpretCast(BufferInfo*, FromTextureHandle);

					glBindBuffer(GL_PIXEL_PACK_BUFFER, info->Handle);

					glBufferData(GL_PIXEL_PACK_BUFFER, Size, nullptr, GL_STATIC_COPY);

					glActiveTexture(GL_TEXTURE0);

					bool result = true;

					glBindTexture(GetTextureType(TextureType), texInfo->Handle);

					glGetTexImage(GetTextureType(TextureType), Level, GetTextureFormat(TextureFormat), GetTexturePixelType(TextureFormat), nullptr);

					glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

					return result;
				}

				bool OpenGLDevice::CopyFromBufferToTexture(GPUBuffer::Handle Handle, Texture::Handle ToTextureHandle, Texture::Types TextureType, uint32 Width, uint32 Height, Formats TextureFormat)
				{
					if (Handle == 0)
						return false;

					if (ToTextureHandle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

					uint32 target = GetBufferType(GPUBuffer::Types::Pixel);

					BufferInfo* texInfo = ReinterpretCast(BufferInfo*, ToTextureHandle);
					glBindTexture(GetTextureType(TextureType), texInfo->Handle);

					glBindBuffer(target, info->Handle);

					glTexSubImage2D(GetTextureType(TextureType), 0, 0, 0, Width, Height, GetTextureFormat(TextureFormat), GetTexturePixelType(TextureFormat), 0);

					glBindBuffer(target, 0);

					glBindTexture(GetTextureType(TextureType), 0);

					return true;
				}

				bool OpenGLDevice::LockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer)
				{
					if (Handle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

					return LockBuffer(info, Type, Access, Buffer);
				}

				bool OpenGLDevice::UnlockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type)
				{
					if (Handle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

					return UnlockBuffer(info, Type);
				}

				bool OpenGLDevice::CompileProgramAPI(const Shaders* Shaders, CompiledShaders* CompiledShaders, cstr* ErrorMessage)
				{
#define IMPLEMENT_COMPILE(StageType, StageName) \
					if (Shaders->StageName == nullptr) \
					{ \
						CompiledShaders->StageName.Buffer = nullptr; \
						CompiledShaders->StageName.Size = 0; \
					} \
					else if (!compiler->Compile(EShClientOpenGL, StageType, Shaders->StageName, Compiler::ENTRY_POINT_NAME, CompiledShaders->StageName.Buffer, CompiledShaders->StageName.Size, message)) \
					{ \
						CompiledShaders->StageName.Size = 0; \
						*ErrorMessage = message; \
						return false; \
					}

					const int16 MessageSize = 1024;
					static char8 message[MessageSize];

					GLSLANGCompiler* compiler = GLSLANGCompiler::GetInstance();

					IMPLEMENT_COMPILE(EShLangVertex, VertexShader);
					IMPLEMENT_COMPILE(EShLangTessControl, TessellationShader);
					IMPLEMENT_COMPILE(EShLangGeometry, GeometryShader);
					IMPLEMENT_COMPILE(EShLangFragment, FragmentShader);
					IMPLEMENT_COMPILE(EShLangCompute, ComputeShader);

					return true;

#undef IMPLEMENT_COMPILE
				}

				bool OpenGLDevice::CompileProgram(const Shaders* Shaders, CompiledShaders* CompiledShaders, cstr* ErrorMessage)
				{
					return CompileProgramAPI(Shaders, CompiledShaders, ErrorMessage);
				}

				bool OpenGLDevice::CreateProgram(const CompiledShaders* Shaders, Program::Handle& Handle, cstr* ErrorMessage)
				{
#define IMPLEMENT_CREATE_SHADER(Type, StageName) \
					uint32 StageName##ID = 0; \
					if (Shaders->StageName.Size != 0) \
					{ \
						StageName##ID = glCreateShader(Type); \
						glShaderBinary(1, &StageName##ID, GL_SHADER_BINARY_FORMAT_SPIR_V, Shaders->StageName.Buffer, Shaders->StageName.Size); \
						glSpecializeShader(StageName##ID, Compiler::ENTRY_POINT_NAME, 0, nullptr, nullptr); \
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

					ProgramInfo* info = RenderingAllocators::ResourceAllocator_Allocate<ProgramInfo>();
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

					Handle = (Program::Handle)info;

					return true;

#undef IMPLEMENT_CREATE_SHADER
				}

				bool OpenGLDevice::DestroyProgram(Program::Handle Handle)
				{
					if (Handle == 0)
						return false;

					ProgramInfo* info = ReinterpretCast(ProgramInfo*, Handle);

					glDeleteProgram(info->Handle);

					RenderingAllocators::ResourceAllocator_Deallocate(info);

					return true;
				}

				bool OpenGLDevice::BindProgram(Program::Handle Handle)
				{
					if (m_LastProgram == Handle)
						return true;

					m_LastProgram = Handle;

					if (Handle != 0)
						Handle = ReinterpretCast(ProgramInfo*, m_LastProgram)->Handle;

					glUseProgram(Handle);

					return true;
				}

				bool OpenGLDevice::SetProgramConstantBuffer(Program::ConstantHandle Handle, ConstantBuffer::Handle Value)
				{
					if (Value != 0)
						Value = ReinterpretCast(BufferInfo*, Value)->Handle;

					glBindBufferBase(GetBufferType(GPUBuffer::Types::Constant), Handle, Value);

					return true;
				}

				bool OpenGLDevice::SetProgramTexture(Program::ConstantHandle Handle, Texture::Types Type, Texture::Handle Value)
				{
					if (Value != 0)
						Value = ReinterpretCast(BufferInfo*, Value)->Handle;

					glActiveTexture(GL_TEXTURE0 + Handle);

					glBindTexture(GetTextureType(Type), Value);

					glUniform1i(Handle, Handle);

					return true;
				}

				bool OpenGLDevice::CreateTexture(const TextureInfo* Info, Texture::Handle& Handle)
				{
					uint32 handle;
					glGenTextures(1, &handle);

					BufferInfo* info = RenderingAllocators::ResourceAllocator_Allocate<BufferInfo>();

					INITIALIZE_BUFFER_INFO(info, handle);

					glBindTexture(GetTextureType(Info->Type), handle);

					if (Texture::GetChannelCount(Info->Format) == 4)
						glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
					else
						glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

					glTexImage2D(GetTextureType(Info->Type), 0, GetTextureInternalFormat(Info->Format), Info->Dimension.X, Info->Dimension.Y, 0, GetTextureFormat(Info->Format), GetTexturePixelType(Info->Format), Info->Data);

					Handle = (Texture::Handle)info;

					return true;
				}

				bool OpenGLDevice::DestroyTexture(Texture::Handle Handle)
				{
					if (Handle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

					glDeleteTextures(1, &info->Handle);

					RenderingAllocators::ResourceAllocator_Deallocate(info);

					return true;
				}

				bool OpenGLDevice::SetTextureVerticalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode)
				{
					if (Handle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

					glBindTexture(GetTextureType(Type), info->Handle);

					glTexParameteri(GetTextureType(Type), GL_TEXTURE_WRAP_T, GetWrapMode(Mode));

					return true;
				}

				bool OpenGLDevice::SetTextureHorizontalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode)
				{
					if (Handle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

					glBindTexture(GetTextureType(Type), info->Handle);

					glTexParameteri(GetTextureType(Type), GL_TEXTURE_WRAP_S, GetWrapMode(Mode));

					return true;
				}

				bool OpenGLDevice::SetTextureMinifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MinifyFilters Filter)
				{
					if (Handle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

					glBindTexture(GetTextureType(Type), info->Handle);

					glTexParameteri(GetTextureType(Type), GL_TEXTURE_MIN_FILTER, GetMinifyFilter(Filter));

					return true;
				}

				bool OpenGLDevice::SetTextureMagnifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MagnfyFilters Filter)
				{
					if (Handle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

					glBindTexture(GetTextureType(Type), info->Handle);

					glTexParameteri(GetTextureType(Type), GL_TEXTURE_MAG_FILTER, GetMagnifyFilter(Filter));

					return true;
				}

				bool OpenGLDevice::GenerateTextureMipMap(Texture::Handle Handle, Texture::Types Type)
				{
					if (Handle == 0)
						return false;

					BufferInfo* info = ReinterpretCast(BufferInfo*, Handle);

					glBindTexture(GetTextureType(Type), info->Handle);

					glGenerateMipmap(GetTextureType(Type));

					return true;
				}

				bool OpenGLDevice::CreateRenderTarget(const RenderTargetInfo* Info, RenderTarget::Handle& Handle, TextureList& Textures)
				{
					if (Info->Textures.GetSize() == 0)
						return false;

					RenderTargetInfos* renderTargetInfos = RenderingAllocators::ResourceAllocator_Allocate<RenderTargetInfos>();
					PlatformMemory::Set(renderTargetInfos, 0, 1);

					uint32 handle;
					glGenFramebuffers(1, &handle);
					renderTargetInfos->Handle = handle;

					Handle = (RenderTarget::Handle)renderTargetInfos;

					BindRenderTarget(Handle);

					static uint32 drawBuffers[((int8)RenderTarget::AttachmentPoints::Color8 - (int8)RenderTarget::AttachmentPoints::Color0) + 1];

					int drawBufferIndex = 0;

					for (const auto& textureInfo : Info->Textures)
					{
						TextureInfo info;
						info.Type = Texture::Types::TwoD;
						info.Dimension = textureInfo.Dimension;
						info.Format = textureInfo.Format;
						info.Data = nullptr;

						Texture::Handle texHandle;
						if (!CreateTexture(&info, texHandle))
							return false;

						uint32 point = GetAttachmentPoint(textureInfo.Point);

						BufferInfo* texInfo = ReinterpretCast(BufferInfo*, texHandle);
						glFramebufferTexture2D(GL_FRAMEBUFFER, point, GetTextureType(info.Type), texInfo->Handle, 0);

						renderTargetInfos->Textures.Add(texHandle);

						if (textureInfo.Point >= RenderTarget::AttachmentPoints::Color0)
							drawBuffers[drawBufferIndex++] = point;
					}

					Textures.AddRange(renderTargetInfos->Textures);

					glDrawBuffers(drawBufferIndex, drawBuffers);

					BindRenderTarget(0);

					return true;
				}

				bool OpenGLDevice::DestroyRenderTarget(RenderTarget::Handle Handle)
				{
					if (Handle == 0)
						return false;

					RenderTargetInfos* renderTargetInfos = ReinterpretCast(RenderTargetInfos*, Handle);

					for (auto handle : renderTargetInfos->Textures)
						DestroyTexture(handle);

					uint32 handle = Handle;
					glDeleteFramebuffers(1, &handle);

					RenderingAllocators::ResourceAllocator_Deallocate(renderTargetInfos);

					return true;
				}

				bool OpenGLDevice::BindRenderTarget(RenderTarget::Handle Handle)
				{
					if (Handle != 0)
						Handle = ReinterpretCast(RenderTargetInfos*, Handle)->Handle;

					glBindFramebuffer(GL_FRAMEBUFFER, Handle);

					return true;
				}

				bool OpenGLDevice::CreateMesh(const SubMeshInfo* Info, SubMesh::Handle& Handle)
				{
#define IMPLEMENT_UPLOAD_BUFFER(BufferHandle, Type, Size, Data) \
					uint32 target = GetBufferType(Type); \
					glBindBuffer(target, ReinterpretCast(BufferInfo*, BufferHandle)->Handle); \
					glBufferData(target, Size, Data, GL_STATIC_DRAW); \
					glBindBuffer(target, 0);

					if (Info->Vertices.GetSize() == 0)
						return false;

					GPUBuffer::Handle vbo;
					if (!CreateBuffer(vbo))
						return false;

					IMPLEMENT_UPLOAD_BUFFER(vbo, GPUBuffer::Types::Vertex, SubMesh::GetVertexBufferSize(Info->Vertices.GetSize()), Info->Vertices.GetData());

					GPUBuffer::Handle ebo = 0;
					if (Info->Indices.GetSize() != 0)
					{
						if (!CreateBuffer(ebo))
							return false;

						IMPLEMENT_UPLOAD_BUFFER(ebo, GPUBuffer::Types::Index, SubMesh::GetIndexBufferSize(Info->Indices.GetSize()), Info->Indices.GetData());
					}

					MeshBufferInfo* meshBufferInfo = RenderingAllocators::ResourceAllocator_Allocate<MeshBufferInfo>();
					PlatformMemory::Set(meshBufferInfo, 0, 1);

					meshBufferInfo->VertexBufferObject = ReinterpretCast(BufferInfo*, vbo);
					if (ebo != 0)
						meshBufferInfo->IndexBufferObject = ReinterpretCast(BufferInfo*, ebo);
					meshBufferInfo->Layout = Info->Layout;

					Handle = (SubMesh::Handle)meshBufferInfo;

					return true;

#undef IMPLEMENT_UPLOAD_BUFFER
				}

				bool OpenGLDevice::DestroyMesh(SubMesh::Handle Handle)
				{
					if (Handle == 0)
						return false;

					MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Handle);

					DestroyBuffer((GPUBuffer::Handle)meshBufferInfo->VertexBufferObject);

					if (meshBufferInfo->IndexBufferObject != nullptr)
						DestroyBuffer((GPUBuffer::Handle)meshBufferInfo->IndexBufferObject);

					RenderContextInfo* currentInfo = m_CurrentContext;

					for (auto& item : m_Contexts)
					{
						auto* context = item.GetSecond();

						if (!context->IsActive)
							SetContext(item.GetFirst());

						if (!context->VertexArrays.Contains(Handle))
							continue;

						DestroyVertexArray(context->VertexArrays[Handle]);

						context->VertexArrays.Remove(Handle);
					}

					if (m_CurrentContext != currentInfo)
						SetContext(m_CurrentContextHandle);

					RenderingAllocators::ResourceAllocator_Deallocate(meshBufferInfo);

					return true;
				}

				bool OpenGLDevice::BindMesh(SubMesh::Handle Handle)
				{
					if (m_CurrentContext == nullptr)
						return false;

					if (Handle == 0)
						return false;

					MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Handle);

					Assert(m_CurrentContext->IsActive, "Context is not active");

					if (m_CurrentContext->LastMeshHandle == Handle)
						return true;

					m_CurrentContext->LastMeshHandle = Handle;

					GPUBuffer::Handle vao = 0;
					if (m_CurrentContext->VertexArrays.Contains(Handle))
						vao = m_CurrentContext->VertexArrays[Handle];
					else
					{
						if (!CreateVertexArray(*meshBufferInfo, vao))
							return false;

						m_CurrentContext->VertexArrays[Handle] = vao;
					}

					glBindVertexArray(vao);

					return true;
				}

				bool OpenGLDevice::Clear(ClearFlags Flags, const ColorUI8& Color)
				{
					if (m_ClearColor != Color)
					{
						m_ClearColor = Color;

						Vector4F col;
						Helper::GetNormalizedColor(Color, col);

						glClearColor(col.X, col.Y, col.Z, col.W);
					}

					glClear(GetClearingFlags(Flags));

					return true;
				}

				bool OpenGLDevice::DrawIndexed(SubMesh::PolygonTypes PolygonType, uint32 IndexCount)
				{
					glDrawElements(GetPolygonType(PolygonType), IndexCount, GL_UNSIGNED_INT, nullptr);

					return true;
				}

				bool OpenGLDevice::DrawArray(SubMesh::PolygonTypes PolygonType, uint32 VertexCount)
				{
					glDrawArrays(GetPolygonType(PolygonType), 0, VertexCount);

					return true;
				}

				bool OpenGLDevice::SwapBuffers(void)
				{
					if (m_CurrentContext == nullptr)
						return false;

					PlatformGL::SwapBuffers(m_CurrentContext->ContextHandle, false);

					return true;
				}

				bool OpenGLDevice::BeginEvent(cwstr Label)
				{
					return SetMarker(Label);
				}

				bool OpenGLDevice::SetMarker(cwstr Label)
				{
					uint8 len = CharacterUtility::GetLength(Label);
					static char8 label[512];
					CharacterUtility::ChangeType(Label, label, len);

					glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0, GL_DEBUG_SEVERITY_NOTIFICATION, len, label);

					return true;
				}

				bool OpenGLDevice::SetDebugCallback(DebugFunction Callback)
				{
					m_DebugCallback = Callback;

					return true;
				}

				bool OpenGLDevice::InitializeBaseContext(void)
				{
					Window* window = RenderingAllocators::RenderingSystemAllocator_Allocate<Window>();
					m_BaseContextWindow = ReinterpretCast(byte*, window);
					Construct(window, "DummyContextWindow");
					window->Initialize();
					window->SetIsVisible(false);

					if (!CreateContext(window->GetHandle(), m_BaseContextHandle))
						return false;

					if (!SetContext(m_BaseContextHandle))
						return false;

					return SetViewport(Vector2I::Zero, window->GetClientSize());
				}

				bool OpenGLDevice::CreateVertexArray(const MeshBufferInfo& Info, NativeType::Handle& Handle)
				{
					uint32 handle;
					glGenVertexArrays(1, &handle);
					Handle = handle;

					glBindVertexArray(Handle);

					glBindBuffer(GL_ARRAY_BUFFER, Info.VertexBufferObject->Handle);

					uint32 vertexSize = sizeof(Vertex);

					if (BitwiseUtils::IsEnabled(Info.Layout, SubMesh::VertexLayouts::Position))
					{
						uint16 index = SubMeshInfo::GetLayoutIndex(SubMesh::VertexLayouts::Position);

						glVertexAttribPointer(index, 3, GL_FLOAT, false, vertexSize, (void*)OffsetOf(&Vertex::Position));
						glEnableVertexAttribArray(index++);
					}
					if (BitwiseUtils::IsEnabled(Info.Layout, SubMesh::VertexLayouts::Normal))
					{
						uint16 index = SubMeshInfo::GetLayoutIndex(SubMesh::VertexLayouts::Normal);

						glVertexAttribPointer(index, 3, GL_FLOAT, false, vertexSize, (void*)OffsetOf(&Vertex::Normal));
						glEnableVertexAttribArray(index++);
					}
					if (BitwiseUtils::IsEnabled(Info.Layout, SubMesh::VertexLayouts::TexCoord))
					{
						uint16 index = SubMeshInfo::GetLayoutIndex(SubMesh::VertexLayouts::TexCoord);

						glVertexAttribPointer(index, 2, GL_FLOAT, false, vertexSize, (void*)OffsetOf(&Vertex::UV));
						glEnableVertexAttribArray(index);
					}

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Info.IndexBufferObject->Handle);

					return true;
				}

				bool OpenGLDevice::DestroyVertexArray(NativeType::Handle Handle)
				{
					uint32 handle;
					glDeleteVertexArrays(1, &handle);
					Handle = handle;

					return true;
				}

				bool OpenGLDevice::LockBuffer(BufferInfo* Info, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer)
				{
					uint32 target = GetBufferType(Type);

					glBindBuffer(target, Info->Handle);

					void* buffer = glMapBuffer(target, GetBufferAccess(Access));

					if (buffer == nullptr)
						return false;

					*Buffer = ReinterpretCast(byte*, buffer);

					return true;
				}

				bool OpenGLDevice::UnlockBuffer(BufferInfo* Info, GPUBuffer::Types Type)
				{
					uint32 target = GetBufferType(Type);

					glBindBuffer(target, Info->Handle);

					glUnmapBuffer(target);

					glBindBuffer(target, 0);

					return true;
				}
			}
		}
	}
}