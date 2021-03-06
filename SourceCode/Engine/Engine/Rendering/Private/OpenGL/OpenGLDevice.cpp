// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\OpenGL\OpenGLDevice.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Rendering\Private\Helper.h>
#include <Rendering\Private\ProgramCompiler\GLSLANGCompiler.h>
#include <Containers\StringUtility.h>
#include <Debugging\Debug.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Utility\Window.h>
#include <GL\glew.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Platform;
	using namespace MemoryManagement::Allocator;

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

				const uint16 LAST_ERROR_SIZE = 512;

				uint32 GetClearingFlags(IDevice::ClearFlags Flags)
				{
					uint32 flags = 0;

					SET_IF_ENABLED(Flags, IDevice::ClearFlags::ColorBuffer, flags, GL_COLOR_BUFFER_BIT);
					SET_IF_ENABLED(Flags, IDevice::ClearFlags::DepthBuffer, flags, GL_DEPTH_BUFFER_BIT);
					SET_IF_ENABLED(Flags, IDevice::ClearFlags::AccumulationBuffer, flags, GL_ACCUM_BUFFER_BIT);
					SET_IF_ENABLED(Flags, IDevice::ClearFlags::StencilBuffer, flags, GL_STENCIL_BUFFER_BIT);

					return flags;
				}

				uint32 GetBufferType(GPUBuffer::Types Type)
				{
					switch (Type)
					{
					case GPUBuffer::Types::Array:
						return GL_ARRAY_BUFFER;
					case GPUBuffer::Types::ElementArray:
						return GL_ELEMENT_ARRAY_BUFFER;
					case GPUBuffer::Types::PixelPack:
						return GL_PIXEL_PACK_BUFFER;
					case GPUBuffer::Types::PixelUnpack:
						return GL_PIXEL_UNPACK_BUFFER;
					}

					return 0;
				}

				uint32 GetBufferUsage(GPUBuffer::Usages Type)
				{
					switch (Type)
					{
					case GPUBuffer::Usages::StreamDraw:
						return GL_STREAM_DRAW;
					case GPUBuffer::Usages::StreamRead:
						return GL_STREAM_READ;
					case GPUBuffer::Usages::StreamCopy:
						return GL_STREAM_COPY;
					case GPUBuffer::Usages::StaticDraw:
						return GL_STATIC_DRAW;
					case GPUBuffer::Usages::StaticRead:
						return GL_STATIC_READ;
					case GPUBuffer::Usages::StaticCopy:
						return GL_STATIC_COPY;
					case GPUBuffer::Usages::DynamicDraw:
						return GL_DYNAMIC_DRAW;
					case GPUBuffer::Usages::DynamicRead:
						return GL_DYNAMIC_READ;
					case GPUBuffer::Usages::DynamicCopy:
						return GL_DYNAMIC_COPY;
					}

					return 0;
				}

				uint32 GetBufferAccess(GPUBuffer::Access Type)
				{
					switch (Type)
					{
					case  GPUBuffer::Access::ReadOnly:
						return GL_READ_ONLY;
					case  GPUBuffer::Access::WriteOnly:
						return GL_WRITE_ONLY;
					case  GPUBuffer::Access::ReadAndWrite:
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

				uint32 GetTextureInternalFormat(Texture::Formats Format)
				{
					switch (Format)
					{
					case Texture::Formats::R8:
						return GL_R8;
					case Texture::Formats::R16:
						return GL_R16;
					case Texture::Formats::R32:
						return GL_R32UI;
					case Texture::Formats::R16F:
						return GL_R16F;
					case Texture::Formats::R32F:
						return GL_R32F;
					case Texture::Formats::RG8:
						return GL_RG8;
					case Texture::Formats::RG16:
						return GL_RG16;
					case Texture::Formats::RG32:
						return GL_RG32UI;
					case Texture::Formats::RG16F:
						return GL_RG16F;
					case Texture::Formats::RG32F:
						return GL_RG32F;
					case Texture::Formats::RGB8:
						return GL_RGB8;
					case Texture::Formats::RGB16:
						return GL_RGB16UI;
					case Texture::Formats::RGB32:
						return GL_RGB32UI;
					case Texture::Formats::RGB16F:
						return GL_RGB16F;
					case Texture::Formats::RGB32F:
						return GL_RGB32F;
					case Texture::Formats::RGBA8:
						return GL_RGBA8;
					case Texture::Formats::RGBA16:
						return GL_RGBA16;
					case Texture::Formats::RGBA32:
						return GL_RGBA32UI;
					case Texture::Formats::RGBA16F:
						return GL_RGBA16F;
					case Texture::Formats::RGBA32F:
						return GL_RGBA32F;
					case Texture::Formats::Depth16:
						return GL_DEPTH_COMPONENT16;
					case Texture::Formats::Depth24:
						return GL_DEPTH_COMPONENT24;
					case Texture::Formats::Depth32:
						return GL_DEPTH_COMPONENT32;
					case Texture::Formats::Depth32F:
						return GL_DEPTH_COMPONENT32F;
					case Texture::Formats::Stencil24F:
						return GL_DEPTH24_STENCIL8;
					case Texture::Formats::Stencil32F:
						return GL_DEPTH32F_STENCIL8;
					}

					return 0;
				}

				uint32 GetTextureFormat(Texture::Formats Format)
				{
					switch (Format)
					{
					case Texture::Formats::R8:
						return GL_RED;
					case Texture::Formats::R16:
						return GL_RED;
					case Texture::Formats::R32:
						return GL_RED;
					case Texture::Formats::R16F:
						return GL_RED;
					case Texture::Formats::R32F:
						return GL_RED;
					case Texture::Formats::RG8:
						return GL_RG;
					case Texture::Formats::RG16:
						return GL_RG;
					case Texture::Formats::RG32:
						return GL_RG;
					case Texture::Formats::RG16F:
						return GL_RG;
					case Texture::Formats::RG32F:
						return GL_RG;
					case Texture::Formats::RGB8:
						return GL_RGB;
					case Texture::Formats::RGB16:
						return GL_RGB;
					case Texture::Formats::RGB32:
						return GL_RGB;
					case Texture::Formats::RGB16F:
						return GL_RGB;
					case Texture::Formats::RGB32F:
						return GL_RGB;
					case Texture::Formats::RGBA8:
						return GL_RGBA;
					case Texture::Formats::RGBA16:
						return GL_RGBA;
					case Texture::Formats::RGBA32:
						return GL_RGBA;
					case Texture::Formats::RGBA16F:
						return GL_RGBA;
					case Texture::Formats::RGBA32F:
						return GL_RGBA;
					case Texture::Formats::Depth16:
						return GL_DEPTH_COMPONENT;
					case Texture::Formats::Depth24:
						return GL_DEPTH_COMPONENT;
					case Texture::Formats::Depth32:
						return GL_DEPTH_COMPONENT;
					case Texture::Formats::Depth32F:
						return GL_DEPTH_COMPONENT;
					case Texture::Formats::Stencil24F:
						return GL_DEPTH_STENCIL;
					case Texture::Formats::Stencil32F:
						return GL_DEPTH_STENCIL;
					}

					return 0;
				}

				uint32 GetTexturePixelType(Texture::Formats Format)
				{
					switch (Format)
					{
					case Texture::Formats::R8:
						return GL_UNSIGNED_BYTE;
					case Texture::Formats::R16:
						return GL_UNSIGNED_SHORT;
					case Texture::Formats::R32:
						return GL_UNSIGNED_INT;
					case Texture::Formats::R16F:
						return GL_HALF_FLOAT;
					case Texture::Formats::R32F:
						return GL_FLOAT;
					case Texture::Formats::RG8:
						return GL_UNSIGNED_BYTE;
					case Texture::Formats::RG16:
						return GL_UNSIGNED_SHORT;
					case Texture::Formats::RG32:
						return GL_UNSIGNED_INT;
					case Texture::Formats::RG16F:
						return GL_HALF_FLOAT;
					case Texture::Formats::RG32F:
						return GL_FLOAT;
					case Texture::Formats::RGB8:
						return GL_UNSIGNED_BYTE;
					case Texture::Formats::RGB16:
						return GL_UNSIGNED_INT;
					case Texture::Formats::RGB32:
						return GL_UNSIGNED_INT;
					case Texture::Formats::RGB16F:
						return GL_HALF_FLOAT;
					case Texture::Formats::RGB32F:
						return GL_FLOAT;
					case Texture::Formats::RGBA8:
						return GL_UNSIGNED_BYTE;
					case Texture::Formats::RGBA16:
						return GL_UNSIGNED_SHORT;
					case Texture::Formats::RGBA32:
						return GL_UNSIGNED_INT;
					case Texture::Formats::RGBA16F:
						return GL_HALF_FLOAT;
					case Texture::Formats::RGBA32F:
						return GL_FLOAT;
					case Texture::Formats::Depth16:
						return GL_UNSIGNED_SHORT;
					case Texture::Formats::Depth24:
						return GL_UNSIGNED_INT;
					case Texture::Formats::Depth32:
						return GL_UNSIGNED_INT;
					case Texture::Formats::Depth32F:
						return GL_FLOAT;
					case Texture::Formats::Stencil24F:
						return GL_UNSIGNED_INT_24_8;
					case Texture::Formats::Stencil32F:
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
					case RenderTarget::AttachmentPoints::Stencil: return GL_STENCIL_ATTACHMENT;
					case RenderTarget::AttachmentPoints::Color0: return GL_COLOR_ATTACHMENT0;
					case RenderTarget::AttachmentPoints::Color1: return GL_COLOR_ATTACHMENT1;
					case RenderTarget::AttachmentPoints::Color2: return GL_COLOR_ATTACHMENT2;
					case RenderTarget::AttachmentPoints::Color3: return GL_COLOR_ATTACHMENT3;
					case RenderTarget::AttachmentPoints::Color4: return GL_COLOR_ATTACHMENT4;
					case RenderTarget::AttachmentPoints::Color5: return GL_COLOR_ATTACHMENT5;
					case RenderTarget::AttachmentPoints::Color6: return GL_COLOR_ATTACHMENT6;
					case RenderTarget::AttachmentPoints::Color7: return GL_COLOR_ATTACHMENT7;
					case RenderTarget::AttachmentPoints::Color8: return GL_COLOR_ATTACHMENT8;
					case RenderTarget::AttachmentPoints::Color9: return GL_COLOR_ATTACHMENT9;
					case RenderTarget::AttachmentPoints::Color10: return GL_COLOR_ATTACHMENT10;
					case RenderTarget::AttachmentPoints::Color11: return GL_COLOR_ATTACHMENT11;
					case RenderTarget::AttachmentPoints::Color12: return GL_COLOR_ATTACHMENT12;
					case RenderTarget::AttachmentPoints::Color13: return GL_COLOR_ATTACHMENT13;
					case RenderTarget::AttachmentPoints::Color14: return GL_COLOR_ATTACHMENT14;
					case RenderTarget::AttachmentPoints::Color15: return GL_COLOR_ATTACHMENT15;
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
					m_BaseContext(nullptr),
					m_CurrentContextHandle(0),
					m_CurrentContext(nullptr),
					m_LastProgram(0),
					m_LastFrameBuffer(0),
					m_LastActiveTextureUnitIndex(0)
				{
				}

				OpenGLDevice::~OpenGLDevice(void)
				{
				}

				bool OpenGLDevice::Initialize(void)
				{
					Assert(!m_Initialized, "OpenGLDevice already initialized");
					Assert(m_CurrentContextHandle != 0, "Context is null");

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

					PlatformWindow::ContextHandle contextHandle = PlatformWindow::GetDeviceContext(WindowHandle);

					if (contextHandle == 0)
						return false;

					PlatformWindow::PixelFormatInfo pixelFormat =
					{
						PlatformWindow::PixelFormats::DrawToWindow | PlatformWindow::PixelFormats::DrawToBitmap | PlatformWindow::PixelFormats::SupportOpenGL | PlatformWindow::PixelFormats::DoubleBuffer,
						PlatformWindow::PixelTypes::RGBA,
						32,
						24,
						8,
						PlatformWindow::LayerTypes::MainPlane
					};

					int32 pixelFormatIndex = PlatformWindow::ChoosePixelFormat(contextHandle, &pixelFormat);
					PlatformWindow::SetPixelFormat(contextHandle, pixelFormatIndex, &pixelFormat);

					PlatformWindow::WGLContextHandle shareWGLContextHandle = 0;
					if (m_BaseContext != nullptr)
						shareWGLContextHandle = m_BaseContext->WGLContextHandle;

					PlatformWindow::ContextHandle prevContextHandle;
					PlatformWindow::WGLContextHandle prevWGLContextHandle;
					PlatformWindow::GetCurrentWGLContext(prevContextHandle, prevWGLContextHandle);

					PlatformWindow::WGLContextHandle wglContextHandle = PlatformWindow::CreateWGLARBContext(contextHandle, shareWGLContextHandle,
#ifdef DEBUG_MODE
						true
#else
						false
#endif
					);

					PlatformWindow::MakeCurrentWGLContext(prevContextHandle, prevWGLContextHandle);

					if (wglContextHandle == 0)
						return false;

					Handle = (RenderContext::Handle)WindowHandle;

					RenderContextInfo* info = RenderingAllocators::RenderingSystemAllocator_Allocate<RenderContextInfo>();
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

					PlatformWindow::DestroyWGLContext(info->WGLContextHandle);

					RenderingAllocators::RenderingSystemAllocator_Deallocate(info);

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
						PlatformWindow::MakeCurrentWGLContext(0, 0);
						return true;
					}

					if (!m_Contexts.Contains(Handle))
						return false;

					RenderContextInfo* info = m_Contexts[Handle];

					m_CurrentContextHandle = Handle;
					m_CurrentContext = info;

					PlatformWindow::MakeCurrentWGLContext(info->ContextHandle, info->WGLContextHandle);

					info->LastMeshHandle = 0;
					info->IsActive = true;

					//HITODO: Impl. Multisample
					//https://www.khronos.org/opengl/wiki/Multisampling
					//https://learnopengl.com/Advanced-OpenGL/Anti-Aliasing
					//glEnable(GL_MULTISAMPLE);

					ResetState();

					return true;
				}

				bool OpenGLDevice::SetViewport(const Vector2I& Position, const Vector2I& Size)
				{
					glViewport(Position.X, Position.Y, Size.X, Size.Y);

					return true;
				}

				bool OpenGLDevice::SetClearColor(const ColorUI8& Color)
				{
					if (m_ClearColor == Color)
						return true;

					m_ClearColor = Color;

					Vector4F col;
					Helper::GetNormalizedColor(Color, col);

					glClearColor(col.X, col.Y, col.Z, col.W);

					return true;
				}

				bool OpenGLDevice::SetFaceOrder(FaceOrders Order)
				{
					if (m_State.FaceOrder == Order)
						return true;

					return SetFaceOrderInternal(Order);
				}

				bool OpenGLDevice::SetCullMode(CullModes Mode)
				{
					if (m_State.CullMode == Mode)
						return true;

					return SetCullModeInternal(Mode);
				}

				bool OpenGLDevice::SetDepthTestFunction(TestFunctions Function)
				{
					if (m_State.DepthTestFunction == Function)
						return true;

					return SetDepthTestFunctionInternal(Function);
				}

				bool OpenGLDevice::SetStencilTestFunction(CullModes CullMode, TestFunctions Function, int32 Reference, uint32 Mask)
				{
					State::FaceState& state = m_State.GetFaceState(CullMode);

					if (state.StencilTestFunction == Function && state.StencilTestFunctionReference == Reference && state.StencilTestFunctionMask == Mask)
						return true;

					return SetStencilTestFunctionInternal(CullMode, Function, Reference, Mask);
				}

				bool OpenGLDevice::SetStencilMask(CullModes CullMode, uint32 Mask)
				{
					State::FaceState& state = m_State.GetFaceState(CullMode);

					if (state.StencilMask == Mask)
						return true;

					return SetStencilMaskInternal(CullMode, Mask);
				}

				bool OpenGLDevice::SetStencilOperation(CullModes CullMode, StencilOperations StencilFailed, StencilOperations DepthFailed, StencilOperations DepthPassed)
				{
					State::FaceState& state = m_State.GetFaceState(CullMode);

					if (state.StencilOperationStencilFailed == StencilFailed && state.StencilOperationDepthFailed == DepthFailed && state.StencilOperationDepthPassed == DepthPassed)
						return true;

					return SetStencilOperationInternal(CullMode, StencilFailed, DepthFailed, DepthPassed);
				}

				bool OpenGLDevice::SetBlendEquation(BlendEquations Equation)
				{
					if (m_State.BlendEquation == Equation)
						return true;

					return SetBlendEquationInternal(Equation);
				}

				bool OpenGLDevice::SetBlendFunction(BlendFunctions SourceFactor, BlendFunctions DestinationFactor)
				{
					if (m_State.BlendFunctionSourceFactor == SourceFactor && m_State.BlendFunctionDestinationFactor == DestinationFactor)
						return true;

					return SetBlendFunctionInternal(SourceFactor, DestinationFactor);
				}

				bool OpenGLDevice::SetPolygonMode(CullModes CullMode, PolygonModes PolygonMode)
				{
					State::FaceState& state = m_State.GetFaceState(CullMode);

					if (state.PolygonMode == PolygonMode)
						return true;

					return SetPolygonModeInternal(CullMode, PolygonMode);
				}

				bool OpenGLDevice::SetResourceName(NativeType::Handle Handle, ResourceTypes Type, cwstr Name)
				{
					char8 name[128];
					CharacterUtility::ChangeType(Name, name);

					if (Type == ResourceTypes::Mesh)
					{
						MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Handle);

						String tempName(name);

						glObjectLabel(GL_BUFFER, meshBufferInfo->VertexBufferObject, -1, (tempName + "_VertexBuffer").GetValue());

						if (meshBufferInfo->IndexBufferObject != 0)
							glObjectLabel(GL_BUFFER, meshBufferInfo->IndexBufferObject, -1, (tempName + "_IndexBuffer").GetValue());
					}
					else if (Type == ResourceTypes::RenderTarget)
					{
						RenderTargetInfos* renderTargetInfos = ReinterpretCast(RenderTargetInfos*, Handle);

						String tempName(name);

						glObjectLabel(GL_FRAMEBUFFER, renderTargetInfos->Handle, -1, (tempName + "_FrameBuffer").GetValue());

						uint8 index = 0;
						for (auto texture : renderTargetInfos->Textures)
							glObjectLabel(GL_TEXTURE, renderTargetInfos->Handle, -1, (tempName + "_TextureBuffer_" + StringUtility::ToString<char8>(index++)).GetValue());
					}
					else
					{
						uint32 type = 0;
						switch (Type)
						{
						case IDevice::ResourceTypes::Buffer:
							type = GL_BUFFER;
							break;

						case IDevice::ResourceTypes::Program:
							type = GL_PROGRAM;
							break;

						case IDevice::ResourceTypes::Texture:
							type = GL_TEXTURE;
							break;
						}

						glObjectLabel(type, Handle, -1, name);
					}

					return true;
				}

				bool OpenGLDevice::CreateBuffer(GPUBuffer::Handle& Handle)
				{
					GLuint handle;
					glGenBuffers(1, &handle);

					Handle = handle;

					return true;
				}

				bool OpenGLDevice::DestroyBuffer(GPUBuffer::Handle Handle)
				{
					GLuint handle = Handle;
					glDeleteBuffers(1, &handle);

					return true;
				}

				bool OpenGLDevice::BindBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type)
				{
					glBindBuffer(GetBufferType(Type), Handle);

					return true;
				}

				bool OpenGLDevice::CopyFromVertexToBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, SubMesh::Handle FromMeshHandle, uint32 Size)
				{
					if (FromMeshHandle == 0)
						return false;

					MeshBufferInfo* meshBufferInfoo = ReinterpretCast(MeshBufferInfo*, FromMeshHandle);

					byte* buffer = nullptr;
					if (!LockBuffer(meshBufferInfoo->VertexBufferObject, Type, GPUBuffer::Access::ReadOnly, &buffer))
						return false;

					UnlockBuffer(meshBufferInfoo->VertexBufferObject, Type);

					if (!BindBuffer(Handle, Type))
						return false;

					glBufferData(GetBufferType(Type), Size, buffer, GetBufferUsage(Usage));

					BindBuffer(0, Type);

					return true;
				}

				bool OpenGLDevice::CopyFromIndexoBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, SubMesh::Handle FromMeshHandle, uint32 Size)
				{
					if (FromMeshHandle == 0)
						return false;

					MeshBufferInfo* meshBufferInfoo = ReinterpretCast(MeshBufferInfo*, FromMeshHandle);

					byte* buffer = nullptr;
					if (!LockBuffer(meshBufferInfoo->IndexBufferObject, Type, GPUBuffer::Access::ReadOnly, &buffer))
						return false;

					UnlockBuffer(meshBufferInfoo->IndexBufferObject, Type);

					if (!BindBuffer(Handle, Type))
						return false;

					glBufferData(GetBufferType(Type), Size, buffer, GetBufferUsage(Usage));

					BindBuffer(0, Type);

					return true;
				}

				bool OpenGLDevice::CopyFromTextureToBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, Texture::Handle FromTextureHandle, uint32 Size, Texture::Types TextureType, Texture::Formats TextureFormat, uint32 Level)
				{
					if (!BindBuffer(Handle, Type))
						return false;

					glBufferData(GetBufferType(Type), Size, nullptr, GetBufferUsage(Usage));

					glActiveTexture(GL_TEXTURE0);

					bool result = true;

					if (!BindTexture(FromTextureHandle, TextureType))
					{
						result = false;
						goto Finalize;
					}

					glGetTexImage(GetTextureType(TextureType), Level, GetTextureFormat(TextureFormat), GetTexturePixelType(TextureFormat), nullptr);

				Finalize:
					BindBuffer(0, Type);

					return result;
				}

				bool OpenGLDevice::CopyFromBufferToVertex(GPUBuffer::Handle Handle, GPUBuffer::Types Type, Texture::Handle ToMeshHandle, uint32 Size)
				{
					if (ToMeshHandle == 0)
						return false;

					MeshBufferInfo* meshBufferInfoo = ReinterpretCast(MeshBufferInfo*, ToMeshHandle);

					byte* buffer = nullptr;
					if (!LockBuffer(Handle, Type, GPUBuffer::Access::ReadOnly, &buffer))
						return false;

					UnlockBuffer(Handle, Type);

					if (!BindBuffer(meshBufferInfoo->VertexBufferObject, Type))
						return false;

					glBufferData(GetBufferType(Type), Size, buffer, GetBufferUsage(GPUBuffer::Usages::StaticCopy));

					BindBuffer(0, Type);

					return true;
				}

				bool OpenGLDevice::CopyFromBufferToIndex(GPUBuffer::Handle Handle, GPUBuffer::Types Type, Texture::Handle ToMeshHandle, uint32 Size)
				{
					if (ToMeshHandle == 0)
						return false;

					MeshBufferInfo* meshBufferInfoo = ReinterpretCast(MeshBufferInfo*, ToMeshHandle);

					byte* buffer = nullptr;
					if (!LockBuffer(Handle, Type, GPUBuffer::Access::ReadOnly, &buffer))
						return false;

					UnlockBuffer(Handle, Type);

					if (!BindBuffer(meshBufferInfoo->IndexBufferObject, Type))
						return false;

					glBufferData(GetBufferType(Type), Size, buffer, GetBufferUsage(GPUBuffer::Usages::StaticCopy));

					BindBuffer(0, Type);

					return true;
				}

				bool OpenGLDevice::CopyFromBufferToTexture(GPUBuffer::Handle Handle, GPUBuffer::Types Type, Texture::Handle ToTextureHandle, Texture::Types TextureType, uint32 Width, uint32 Height, Texture::Formats TextureFormat)
				{
					if (!BindTexture(ToTextureHandle, TextureType))
						return false;

					if (!BindBuffer(Handle, Type))
						return false;

					glTexSubImage2D(GetTextureType(TextureType), 0, 0, 0, Width, Height, GetTextureFormat(TextureFormat), GetTexturePixelType(TextureFormat), 0);

					if (!BindBuffer(0, Type))
						return false;

					if (!BindTexture(0, TextureType))
						return false;

					return true;
				}

				bool OpenGLDevice::LockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer)
				{
					if (!BindBuffer(Handle, Type))
						return false;

					void* buffer = glMapBuffer(GetBufferType(Type), GetBufferAccess(Access));

					if (buffer == nullptr)
						return false;

					*Buffer = ReinterpretCast(byte*, buffer);

					return true;
				}

				bool OpenGLDevice::UnlockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type)
				{
					uint32 target = GetBufferType(Type);

					glUnmapBuffer(target);

					glBindBuffer(target, 0);

					return true;
				}

				bool OpenGLDevice::CompileProgramAPI(const Shaders* Shaders, CompiledShaders* CompiledShaders, cstr* ErrorMessage)
				{
#define IMPLEMENT_COMPILE(StageType, StageName) \
					if (Shaders->StageName == nullptr) \
					{ \
						CompiledShaders->StageName.Buffer = nullptr; \
						CompiledShaders->StageName.Size = 0; \
					} \
					else if (!GLSLANGCompiler::Compile(GLSLANG_CLIENT_OPENGL, StageType, Shaders->StageName, CompiledShaders->StageName.Buffer, CompiledShaders->StageName.Size, message)) \
					{ \
						*ErrorMessage = message; \
						return false; \
					}

					const int16 MessageSize = 1024;
					static char8 message[MessageSize];

					IMPLEMENT_COMPILE(GLSLANG_STAGE_VERTEX, VertexShader);
					IMPLEMENT_COMPILE(GLSLANG_STAGE_GEOMETRY, GeometryShader);
					IMPLEMENT_COMPILE(GLSLANG_STAGE_TESSCONTROL, DomainShader);
					IMPLEMENT_COMPILE(GLSLANG_STAGE_FRAGMENT, FragmentShader);
					IMPLEMENT_COMPILE(GLSLANG_STAGE_COMPUTE, ComputeShader);

					return true;
				}

				bool OpenGLDevice::CompileProgram(const Shaders* Shaders, CompiledShaders* CompiledShaders, cstr* ErrorMessage)
				{
					return CompileProgramAPI(Shaders, CompiledShaders, ErrorMessage);
				}

				bool OpenGLDevice::CreateProgram(const CompiledShaders* Shaders, Program::Handle& Handle, cstr* ErrorMessage)
				{
					//glShaderBinary()???????????
					//	glProgramBinary       ???????????


//#define COMPILE_SHADER(Type, Source, Handle) \
//					Handle = glCreateShader(Type); \
//					glShaderSource(Handle, 1, &Source, nullptr); \
//					glCompileShader(Handle); \
//					glGetShaderiv(Handle, GL_COMPILE_STATUS, &result); \
//					if (result == GL_FALSE) \
//					{ \
//						const int16 MessageSize = 1024; \
//						static char8 message[MessageSize]; \
//						int32 len = MessageSize; \
//						glGetShaderInfoLog(Handle, MessageSize, &len, message); \
//						*ErrorMessage = message; \
//						glDeleteShader(Handle); \
//						return false; \
//					}
//
//					int32 result;
//
//					uint32 vertShaderID = 0;
//					COMPILE_SHADER(GL_VERTEX_SHADER, Shaders->VertexShader, vertShaderID);
//
//					uint32 fragShaderID = 0;
//					COMPILE_SHADER(GL_FRAGMENT_SHADER, Shaders->FragmentShader, vertShaderID);
//
//					Handle = glCreateProgram();
//
//					glAttachShader(Handle, vertShaderID);
//					glAttachShader(Handle, fragShaderID);
//
//					glLinkProgram(Handle);
//
//					glGetProgramiv(Handle, GL_LINK_STATUS, &result);
//					if (result == GL_FALSE)
//					{
//						const int16 MessageSize = 1024;
//						static char8 message[MessageSize];
//
//						int32 len = MessageSize;
//						glGetProgramInfoLog(Handle, MessageSize, &len, message);
//
//						*ErrorMessage = message;
//
//						return false;
//					}
//
//					glDetachShader(Handle, vertShaderID);
//					glDeleteShader(vertShaderID);
//
//					glDetachShader(Handle, fragShaderID);
//					glDeleteShader(fragShaderID);
//
//					return true;
//
//#undef COMPILE_SHADER

					return true;
				}

				bool OpenGLDevice::CreateProgram(const Shaders* Shaders, Program::Handle& Handle, cstr* ErrorMessage)
				{
#define COMPILE_SHADER(Type, Source, Handle) \
					Handle = glCreateShader(Type); \
					glShaderSource(Handle, 1, &Source, nullptr); \
					glCompileShader(Handle); \
					glGetShaderiv(Handle, GL_COMPILE_STATUS, &result); \
					if (result == GL_FALSE) \
					{ \
						const int16 MessageSize = 1024; \
						static char8 message[MessageSize]; \
						int32 len = MessageSize; \
						glGetShaderInfoLog(Handle, MessageSize, &len, message); \
						*ErrorMessage = message; \
						glDeleteShader(Handle); \
						return false; \
					}

					int32 result;

					uint32 vertShaderID = 0;
					COMPILE_SHADER(GL_VERTEX_SHADER, Shaders->VertexShader, vertShaderID);

					uint32 fragShaderID = 0;
					COMPILE_SHADER(GL_FRAGMENT_SHADER, Shaders->FragmentShader, vertShaderID);

					Handle = glCreateProgram();

					glAttachShader(Handle, vertShaderID);
					glAttachShader(Handle, fragShaderID);

					glLinkProgram(Handle);

					glGetProgramiv(Handle, GL_LINK_STATUS, &result);
					if (result == GL_FALSE)
					{
						const int16 MessageSize = 1024;
						static char8 message[MessageSize];

						int32 len = MessageSize;
						glGetProgramInfoLog(Handle, MessageSize, &len, message);

						*ErrorMessage = message;

						return false;
					}

					glDetachShader(Handle, vertShaderID);
					glDeleteShader(vertShaderID);

					glDetachShader(Handle, fragShaderID);
					glDeleteShader(fragShaderID);

					return true;

#undef COMPILE_SHADER
				}

				bool OpenGLDevice::DestroyProgram(Program::Handle Handle)
				{
					glDeleteProgram(Handle);

					return true;
				}

				bool OpenGLDevice::BindProgram(Program::Handle Handle)
				{
					m_LastActiveTextureUnitIndex = 0;

					if (m_LastProgram == Handle)
						return true;

					m_LastProgram = Handle;

					glUseProgram(m_LastProgram);

					return true;
				}

				bool OpenGLDevice::QueryProgramActiveConstants(Program::Handle Handle, Program::ConstantDataList& Constants)
				{
					int32 count = 0;
					//GL_ACTIVE_UNIFORM_BLOCKS ????????????????????????????????????????????????????
					glGetProgramiv(Handle, GL_ACTIVE_UNIFORMS, &count);

					if (count == 0)
						return false;

					Constants.Extend(count);

					const uint8 bufferSize = 32;
					char8 name[bufferSize];
					int32 nameLength;
					int32 constantSize;
					uint32 type;

					for (int8 i = 0; i < count; i++)
					{
						glGetActiveUniform(Handle, (GLuint)i, bufferSize, &nameLength, &constantSize, &type, name);

						name[nameLength] = CharacterUtility::Character<char8, '\0'>::Value;

						Program::ConstantHandle handle = glGetUniformLocation(Handle, name);

						ProgramDataTypes dataType = ProgramDataTypes::Unknown;
						AnyDataType value;

						switch (type)
						{
						case GL_FLOAT:
						{
							dataType = ProgramDataTypes::Float;
							value = 0.0F;
						}
						break;

						case GL_FLOAT_VEC2:
						{
							dataType = ProgramDataTypes::Float2;
							value = Vector2F();
						}
						break;

						case GL_FLOAT_VEC3:
						{
							dataType = ProgramDataTypes::Float3;
							value = Vector3F();
						}
						break;

						case GL_FLOAT_VEC4:
						{
							dataType = ProgramDataTypes::Float4;
							value = Vector4F();
						}
						break;

						case GL_FLOAT_MAT4:
						{
							dataType = ProgramDataTypes::Matrix4;
							value = Matrix4F::Identity;
						}
						break;

						case GL_SAMPLER_2D:
						{
							dataType = ProgramDataTypes::Texture2D;
							value = nullptr;
						}
						break;
						}

						Constants[i] = Program::ConstantData(handle, name, dataType, value);
					}

					return true;
				}

				bool OpenGLDevice::SetProgramFloat32(Program::ConstantHandle Handle, float32 Value)
				{
					glUniform1f(Handle, Value);

					return true;
				}

				bool OpenGLDevice::SetProgramColor(Program::ConstantHandle Handle, const ColorUI8& Value)
				{
					Vector4F col;
					Helper::GetNormalizedColor(Value, col);

					glUniform4f(Handle, col.X, col.Y, col.Z, col.W);

					return true;
				}

				bool OpenGLDevice::SetProgramVector2(Program::ConstantHandle Handle, const Vector2F& Value)
				{
					glUniform2f(Handle, Value.X, Value.Y);

					return true;
				}

				bool OpenGLDevice::SetProgramVector3(Program::ConstantHandle Handle, const Vector3F& Value)
				{
					glUniform3f(Handle, Value.X, Value.Y, Value.Z);

					return true;
				}

				bool OpenGLDevice::SetProgramVector4(Program::ConstantHandle Handle, const Vector4F& Value)
				{
					glUniform4f(Handle, Value.X, Value.Y, Value.Z, Value.W);

					return true;
				}

				bool OpenGLDevice::SetProgramMatrix4(Program::ConstantHandle Handle, const Matrix4F& Value)
				{
					glUniformMatrix4fv(Handle, 1, false, Value.GetValue());

					return true;
				}

				bool OpenGLDevice::SetProgramTexture(Program::ConstantHandle Handle, Texture::Types Type, Texture::Handle Value)
				{
					glActiveTexture(GL_TEXTURE0 + m_LastActiveTextureUnitIndex);

					BindTexture(Value, Type);

					glUniform1i(Handle, m_LastActiveTextureUnitIndex);

					++m_LastActiveTextureUnitIndex;

					return true;
				}

				bool OpenGLDevice::CreateTexture(const TextureInfo* Info, Texture::Handle& Handle)
				{
					GLuint handle;
					glGenTextures(1, &handle);
					Handle = handle;

					BindTexture(Handle, Info->Type);

					if (Texture::GetChannelCount(Info->Format) == 4)
						glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
					else
						glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

					glTexImage2D(GetTextureType(Info->Type), 0, GetTextureInternalFormat(Info->Format), Info->Dimension.X, Info->Dimension.Y, 0, GetTextureFormat(Info->Format), GetTexturePixelType(Info->Format), Info->Data);

					return true;
				}

				bool OpenGLDevice::DestroyTexture(Texture::Handle Handle)
				{
					GLuint handle = Handle;
					glDeleteTextures(1, &handle);

					return true;
				}

				bool OpenGLDevice::BindTexture(Texture::Handle Handle, Texture::Types Type)
				{
					glBindTexture(GetTextureType(Type), Handle);

					return true;
				}

				bool OpenGLDevice::SetTextureVerticalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode)
				{
					BindTexture(Handle, Type);

					glTexParameteri(GetTextureType(Type), GL_TEXTURE_WRAP_T, GetWrapMode(Mode));

					return true;
				}

				bool OpenGLDevice::SetTextureHorizontalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode)
				{
					BindTexture(Handle, Type);

					glTexParameteri(GetTextureType(Type), GL_TEXTURE_WRAP_S, GetWrapMode(Mode));

					return true;
				}

				bool OpenGLDevice::SetTextureMinifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MinifyFilters Filter)
				{
					BindTexture(Handle, Type);

					glTexParameteri(GetTextureType(Type), GL_TEXTURE_MIN_FILTER, GetMinifyFilter(Filter));

					return true;
				}

				bool OpenGLDevice::SetTextureMagnifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MagnfyFilters Filter)
				{
					BindTexture(Handle, Type);

					glTexParameteri(GetTextureType(Type), GL_TEXTURE_MAG_FILTER, GetMagnifyFilter(Filter));

					return true;
				}

				bool OpenGLDevice::GenerateTextureMipMap(Texture::Handle Handle, Texture::Types Type)
				{
					BindTexture(Handle, Type);

					glGenerateMipmap(GetTextureType(Type));

					return true;
				}

				bool OpenGLDevice::CreateRenderTarget(const RenderTargetInfo* Info, RenderTarget::Handle& Handle, TextureList& Textures)
				{
					if (Info->Textures.GetSize() == 0)
						return false;

					RenderTargetInfos* renderTargetInfos = RenderingAllocators::RenderingSystemAllocator_Allocate<RenderTargetInfos>();
					PlatformMemory::Set(renderTargetInfos, 0, 1);

					GLuint handle;
					glGenFramebuffers(1, &handle);
					renderTargetInfos->Handle = handle;

					Handle = (RenderTarget::Handle)renderTargetInfos;

					BindRenderTarget(Handle);

					static uint32 drawBuffers[((int8)RenderTarget::AttachmentPoints::Color15 - (int8)RenderTarget::AttachmentPoints::Color0) + 1];

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

						glFramebufferTexture2D(GL_FRAMEBUFFER, point, GetTextureType(info.Type), texHandle, 0);

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

					GLuint handle = Handle;
					glDeleteFramebuffers(1, &handle);

					RenderingAllocators::RenderingSystemAllocator_Deallocate(renderTargetInfos);

					return true;
				}

				bool OpenGLDevice::BindRenderTarget(RenderTarget::Handle Handle)
				{
					if (Handle == 0)
					{
						glBindFramebuffer(GL_FRAMEBUFFER, 0);

						return false;
					}

					RenderTargetInfos* renderTargetInfos = ReinterpretCast(RenderTargetInfos*, Handle);

					glBindFramebuffer(GL_FRAMEBUFFER, renderTargetInfos->Handle);

					return true;
				}

				bool OpenGLDevice::CreateMesh(const SubMeshInfo* Info, GPUBuffer::Usages Usage, SubMesh::Handle& Handle)
				{
					if (Info->Vertices.GetSize() == 0)
						return false;

					GPUBuffer::Handle vbo;
					if (!CreateBuffer(vbo))
						return false;

					if (!UploadMeshBuffer(vbo, GPUBuffer::Types::Array, Usage, SubMesh::GetVertexBufferSize(Info->Vertices.GetSize()), Info->Vertices.GetData()))
						return false;

					GPUBuffer::Handle ebo = 0;
					if (Info->Indices.GetSize() != 0)
					{
						if (!CreateBuffer(ebo))
							return false;

						if (!UploadMeshBuffer(ebo, GPUBuffer::Types::ElementArray, Usage, SubMesh::GetIndexBufferSize(Info->Indices.GetSize()), Info->Indices.GetData()))
							return false;
					}

					MeshBufferInfo* meshBufferInfo = RenderingAllocators::RenderingSystemAllocator_Allocate<MeshBufferInfo>();
					PlatformMemory::Set(meshBufferInfo, 0, 1);

					meshBufferInfo->VertexBufferObject = vbo;
					meshBufferInfo->IndexBufferObject = ebo;
					meshBufferInfo->Layout = Info->Layout;

					Handle = (SubMesh::Handle)meshBufferInfo;

					return true;
				}

				bool OpenGLDevice::DestroyMesh(SubMesh::Handle Handle)
				{
					if (Handle == 0)
						return false;

					MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Handle);

					DestroyBuffer(meshBufferInfo->VertexBufferObject);

					if (meshBufferInfo->IndexBufferObject != 0)
						DestroyBuffer(meshBufferInfo->IndexBufferObject);

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

					RenderingAllocators::RenderingSystemAllocator_Deallocate(meshBufferInfo);

					return true;
				}

				bool OpenGLDevice::CreateVertexArray(const MeshBufferInfo& Info, NativeType::Handle& Handle)
				{
					GLuint handle;
					glGenVertexArrays(1, &handle);
					Handle = handle;

					glBindVertexArray(Handle);

					glBindBuffer(GL_ARRAY_BUFFER, Info.VertexBufferObject);

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
					if (BitwiseUtils::IsEnabled(Info.Layout, SubMesh::VertexLayouts::UV))
					{
						uint16 index = SubMeshInfo::GetLayoutIndex(SubMesh::VertexLayouts::UV);

						glVertexAttribPointer(index, 2, GL_FLOAT, false, vertexSize, (void*)OffsetOf(&Vertex::UV));
						glEnableVertexAttribArray(index);
					}

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Info.IndexBufferObject);

					return true;
				}

				bool OpenGLDevice::DestroyVertexArray(NativeType::Handle Handle)
				{
					GLuint handle;
					glDeleteVertexArrays(1, &handle);
					Handle = handle;

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

				bool OpenGLDevice::Clear(ClearFlags Flags)
				{
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

					PlatformWindow::SwapBuffers(m_CurrentContext->ContextHandle);

					return true;
				}

				bool OpenGLDevice::SetDebugCallback(DebugFunction Callback)
				{
					m_DebugCallback = Callback;

					return true;
				}

				bool OpenGLDevice::SetFaceOrderInternal(FaceOrders Order)
				{
					m_State.FaceOrder = Order;

					glFrontFace(GetFaceOrdering(m_State.FaceOrder));

					return true;
				}

				bool OpenGLDevice::SetCullModeInternal(CullModes Mode)
				{
					m_State.CullMode = Mode;

					if (m_State.CullMode == CullModes::None)
						glDisable(GL_CULL_FACE);
					else
					{
						glEnable(GL_CULL_FACE);
						glCullFace(GetCullingMode(m_State.CullMode));
					}

					return true;
				}

				bool OpenGLDevice::SetDepthTestFunctionInternal(TestFunctions Function)
				{
					m_State.DepthTestFunction = Function;

					if (m_State.DepthTestFunction == TestFunctions::Never)
						glDisable(GL_DEPTH_TEST);
					else
					{
						glEnable(GL_DEPTH_TEST);
						glDepthFunc(GetTestFunction(m_State.DepthTestFunction));
					}

					return true;
				}

				bool OpenGLDevice::SetStencilTestFunctionInternal(CullModes CullMode, TestFunctions Function, int32 Reference, uint32 Mask)
				{
					State::FaceState& state = m_State.GetFaceState(CullMode);

					state.StencilTestFunction = Function;
					state.StencilTestFunctionReference = Reference;
					state.StencilTestFunctionMask = Mask;

					if (state.StencilTestFunction == TestFunctions::Never)
						glDisable(GL_STENCIL_TEST);
					else
					{
						glEnable(GL_STENCIL_TEST);
						glStencilFuncSeparate(GetCullingMode(CullMode), GetTestFunction(state.StencilTestFunction), state.StencilTestFunctionReference, state.StencilTestFunctionMask);
					}

					return true;
				}

				bool OpenGLDevice::SetStencilMaskInternal(CullModes CullMode, uint32 Mask)
				{
					State::FaceState& state = m_State.GetFaceState(CullMode);

					state.StencilMask = Mask;

					glStencilMaskSeparate(GetCullingMode(CullMode), state.StencilMask);

					return true;
				}

				bool OpenGLDevice::SetStencilOperationInternal(CullModes CullMode, StencilOperations StencilFailed, StencilOperations DepthFailed, StencilOperations DepthPassed)
				{
					State::FaceState& state = m_State.GetFaceState(CullMode);

					state.StencilOperationStencilFailed = StencilFailed;
					state.StencilOperationDepthFailed = DepthFailed;
					state.StencilOperationDepthPassed = DepthPassed;

					glStencilOpSeparate(GetCullingMode(CullMode), GetStencilingOperation(state.StencilOperationStencilFailed), GetStencilingOperation(state.StencilOperationDepthFailed), GetStencilingOperation(state.StencilOperationDepthPassed));

					return true;
				}

				bool OpenGLDevice::SetBlendEquationInternal(BlendEquations Equation)
				{
					m_State.BlendEquation = Equation;

					glBlendEquation(GetBlendingEquation(m_State.BlendEquation));

					return true;
				}

				bool OpenGLDevice::SetBlendFunctionInternal(BlendFunctions SourceFactor, BlendFunctions DestinationFactor)
				{
					m_State.BlendFunctionSourceFactor = SourceFactor;
					m_State.BlendFunctionDestinationFactor = DestinationFactor;

					if (m_State.BlendFunctionSourceFactor == BlendFunctions::One && m_State.BlendFunctionDestinationFactor == BlendFunctions::Zero)
						glDisable(GL_BLEND);
					else
					{
						glEnable(GL_BLEND);
						glBlendFunc(GetBlendingFunction(m_State.BlendFunctionSourceFactor), GetBlendingFunction(m_State.BlendFunctionDestinationFactor));
					}

					return true;
				}

				bool OpenGLDevice::SetPolygonModeInternal(CullModes CullMode, PolygonModes PolygonMode)
				{
					State::FaceState& state = m_State.GetFaceState(CullMode);

					state.PolygonMode = PolygonMode;

					glPolygonMode(GetCullingMode(CullMode), GetPolygonRenderMode(state.PolygonMode));

					return true;
				}

				bool OpenGLDevice::UploadMeshBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, uint32 Size, const void* Data)
				{
					if (!BindBuffer(Handle, Type))
						return false;

					glBufferData(GetBufferType(Type), Size, Data, GetBufferUsage(Usage));

					BindBuffer(0, Type);

					return true;
				}
			}
		}
	}
}