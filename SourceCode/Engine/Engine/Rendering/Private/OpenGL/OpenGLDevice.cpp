// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\OpenGL\OpenGLDevice.h>
#include <Rendering\Private\OpenGL\OpenGLRenderContext.h>
#include <Debugging\Debug.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Utility\Window.h>
#include <GL\glew.h>

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

				DynamicSizeAllocator allocator("OpenGL Device System Allocator", RootAllocator::GetInstance(), MegaByte);

				template<typename BaseType>
				BaseType* Allocate(uint32 Count)
				{
					return ReinterpretCast(BaseType*, AllocateMemory(&allocator, Count * sizeof(BaseType)));
				}

				template<typename BaseType>
				void Deallocate(BaseType* Ptr)
				{
					DeallocateMemory(&allocator, Ptr);
				}

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

				void GetOpenGLColor(const ColorUI8& InColor, Vector4F& OutColor)
				{
					OutColor.X = InColor.R / 255.F;
					OutColor.Y = InColor.G / 255.F;
					OutColor.Z = InColor.B / 255.F;
					OutColor.W = InColor.A / 255.F;
				}

				void DebugOutputProcedure(GLenum Source, GLenum Type, GLuint ID, GLenum Severity, GLsizei Length, const GLchar* Message, const void* Param)
				{
					//if (ID == 131169 || ID == 131185 || ID == 131218 || ID == 131204)
					//	return;

					OpenGLDevice* device = ConstCast(OpenGLDevice*, ReinterpretCast(const OpenGLDevice*, Param));
					IDevice::DebugProcedureType procedure = device->GetDebugCallback();

					if (procedure == nullptr)
						return;

					IDevice::DebugSources source;
					switch (Source)
					{
					case GL_DEBUG_SOURCE_API:             source = IDevice::DebugSources::API; break;
					case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source = IDevice::DebugSources::WindowSystem; break;
					case GL_DEBUG_SOURCE_SHADER_COMPILER: source = IDevice::DebugSources::ShaderCompiler; break;
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

					IDevice::DebugSeverities severityType;
					switch (Severity)
					{
					case GL_DEBUG_SEVERITY_HIGH:         severityType = IDevice::DebugSeverities::High; break;
					case GL_DEBUG_SEVERITY_MEDIUM:       severityType = IDevice::DebugSeverities::Medium; break;
					case GL_DEBUG_SEVERITY_LOW:          severityType = IDevice::DebugSeverities::Low; break;
					case GL_DEBUG_SEVERITY_NOTIFICATION: severityType = IDevice::DebugSeverities::Notification; break;
					}

					procedure(ID, source, Message, type, severityType);
				}

				OpenGLDevice::OpenGLDevice(void) :
					m_IsInitialized(false),
					m_BaseContext(nullptr),
					m_CurrentContext(nullptr),
					m_LastShader(0),
					m_LastMeshNumber(0),
					m_LastFrameBuffer(0),
					m_LastActiveTextureUnitIndex(0)
				{
				}

				OpenGLDevice::~OpenGLDevice(void)
				{
				}

				bool OpenGLDevice::Initialize(void)
				{
					Assert(!m_IsInitialized, "OpenGLDevice already initialized");
					Assert(m_CurrentContext != nullptr, "Context is null");

					m_IsInitialized = true;

#ifdef  DEBUG_MODE
					glEnable(GL_DEBUG_OUTPUT);
					glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
					glDebugMessageCallback(DebugOutputProcedure, this);

					glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
#endif

					ResetState();

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

				RenderContext* OpenGLDevice::CreateContext(PlatformWindow::WindowHandle Handle)
				{
					if (Handle == 0)
						return false;

					for each (auto & context in m_Contexts)
						if (context->GetWindowHandler() == Handle)
							return false;

					PlatformWindow::ContextHandle contextHandle = PlatformWindow::GetDeviceContext(Handle);

					if (contextHandle == 0)
						return nullptr;

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
						shareWGLContextHandle = m_BaseContext->GetWGLContextHandle();

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

					OpenGLRenderContext* context = Allocate<OpenGLRenderContext>(1);
					Construct(context, this, Handle, contextHandle, wglContextHandle);

					m_Contexts.Add(context);

					if (m_BaseContext == nullptr)
						m_BaseContext = context;

					return context;
				}

				bool OpenGLDevice::DestroyContext(RenderContext* Context)
				{
					if (Context == nullptr)
						return true;

					OpenGLRenderContext* context = ReinterpretCast(OpenGLRenderContext*, Context);

					PlatformWindow::DestroyWGLContext(context->GetWGLContextHandle());

					Deallocate(context);

					m_Contexts.Remove(context);

					return true;
				}

				bool OpenGLDevice::SetContext(RenderContext* Context)
				{
					if (m_CurrentContext == Context)
						return true;

					if (m_CurrentContext != nullptr)
						m_CurrentContext->Deactivate();

					if (Context == nullptr)
					{
						m_CurrentContext = nullptr;
						PlatformWindow::MakeCurrentWGLContext(0, 0);
						return true;
					}

					Assert(IsTypeOf(Context, OpenGLRenderContext), "Invalid context type cannot be set into OpenGLDevice");

					m_CurrentContext = ReinterpretCast(OpenGLRenderContext*, Context);

					PlatformWindow::MakeCurrentWGLContext(m_CurrentContext->GetContextHandle(), m_CurrentContext->GetWGLContextHandle());

					m_CurrentContext->Activate();

					//TODO: Impl. Multisample
					//https://www.khronos.org/opengl/wiki/Multisampling
					//https://learnopengl.com/Advanced-OpenGL/Anti-Aliasing
					//glEnable(GL_MULTISAMPLE);

					ResetState();

					return true;
				}

				bool OpenGLDevice::SetViewport(const Vector2I& Position, const Vector2I& Size)
				{
					//TODO: Fix Position
					glViewport(Position.X, Position.Y, Size.X, Size.Y);

					return true;
				}

				bool OpenGLDevice::SetClearColor(const ColorUI8& Color)
				{
					if (m_ClearColor == Color)
						return true;

					m_ClearColor = Color;

					Vector4F col;
					GetOpenGLColor(Color, col);

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

				bool OpenGLDevice::CreateBuffer(GPUBuffer::Handle& Handle)
				{
					glGenBuffers(1, &Handle);

					return true;
				}

				bool OpenGLDevice::DestroyBuffer(GPUBuffer::Handle Handle)
				{
					glDeleteBuffers(1, &Handle);

					return true;
				}

				bool OpenGLDevice::BindBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type)
				{
					glBindBuffer(GetBufferType(Type), Handle);

					return true;
				}

				bool OpenGLDevice::AttachBufferData(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, uint32 Size, const void* Data)
				{
					if (!BindBuffer(Handle, Type))
						return false;

					glBufferData(GetBufferType(Type), Size, Data, GetBufferUsage(Usage));

					BindBuffer(0, Type);

					return true;
				}

				bool OpenGLDevice::AttachBufferData(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Usages Usage, uint32 Size, Texture::Handle TextureHandle, Texture::Types TextureType, Texture::Formats TextureFormat, uint32 Level)
				{
					if (!BindBuffer(Handle, Type))
						return false;

					glBufferData(GetBufferType(Type), Size, nullptr, GetBufferUsage(Usage));

					glActiveTexture(GL_TEXTURE0);

					bool result = true;

					if (!BindTexture(TextureHandle, TextureType))
					{
						result = false;
						goto Finalize;
					}

					glGetTexImage(GetTextureType(TextureType), Level, GetTextureFormat(TextureFormat), GetTexturePixelType(TextureFormat), nullptr);

				Finalize:
					BindBuffer(0, Type);

					return result;
				}

				bool OpenGLDevice::ReadBufferData(GPUBuffer::Handle Handle, GPUBuffer::Types Type, Texture::Handle TextureHandle, Texture::Types TextureType, uint32 Width, uint32 Height, Texture::Formats TextureFormat)
				{
					if (!BindTexture(TextureHandle, TextureType))
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

				bool OpenGLDevice::UnlockBuffer(GPUBuffer::Types Type)
				{
					uint32 target = GetBufferType(Type);

					glUnmapBuffer(target);

					glBindBuffer(target, 0);

					return true;
				}

				bool OpenGLDevice::CreateShader(cstr VertexShader, cstr FragmentShader, Shader::Handle& Handle, cstr* ErrorMessage)
				{
					uint32 vertShaderID = glCreateShader(GL_VERTEX_SHADER);
					uint32 fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

					glShaderSource(vertShaderID, 1, &VertexShader, nullptr);
					glCompileShader(vertShaderID);

					glShaderSource(fragShaderID, 1, &FragmentShader, nullptr);
					glCompileShader(fragShaderID);

					int32 result;
					const int16 MessageSize = 1024;
					static char8 message[MessageSize];

					glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &result);
					if (result == GL_FALSE)
					{
						int32 len = MessageSize;
						glGetShaderInfoLog(vertShaderID, MessageSize, &len, message);

						*ErrorMessage = message;

						return true;
					}

					glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &result);
					if (result == GL_FALSE)
					{
						int32 len = MessageSize;
						glGetShaderInfoLog(fragShaderID, MessageSize, &len, message);

						*ErrorMessage = message;

						return true;
					}

					Handle = glCreateProgram();
					glAttachShader(Handle, vertShaderID);
					glAttachShader(Handle, fragShaderID);
					glLinkProgram(Handle);

					glGetProgramiv(Handle, GL_LINK_STATUS, &result);
					if (result == GL_FALSE)
						return false;

					glDetachShader(Handle, vertShaderID);
					glDetachShader(Handle, fragShaderID);

					glDeleteShader(vertShaderID);
					glDeleteShader(fragShaderID);

					return true;
				}

				bool OpenGLDevice::DestroyShader(Shader::Handle Handle)
				{
					glDeleteProgram(Handle);

					return true;
				}

				bool OpenGLDevice::BindShader(Shader::Handle Handle)
				{
					m_LastActiveTextureUnitIndex = 0;

					if (m_LastShader == Handle)
						return true;

					m_LastShader = Handle;

					glUseProgram(m_LastShader);

					return true;
				}

				bool OpenGLDevice::QueryShaderActiveConstants(Shader::Handle Handle, Shader::ConstantDataList& Constants)
				{
					int32 count = 0;

					glGetProgramiv(Handle, GL_ACTIVE_UNIFORMS, &count);

					if (count == 0)
						return false;

					const uint8 bufferSize = 32;
					char8 name[bufferSize];
					int32 nameLength;
					int32 constantSize;
					uint32 type;

					for (int8 i = 0; i < count; i++)
					{
						glGetActiveUniform(Handle, (GLuint)i, bufferSize, &nameLength, &constantSize, &type, name);

						name[nameLength] = CharacterUtility::Character<char8, '\0'>::Value;

						Shader::ConstantHandle handle;
						GetShaderConstantHandle(Handle, name, handle);

						ShaderDataType::Types dataType = ShaderDataType::Types::Unknown;
						AnyDataType value;

						switch (type)
						{
						case GL_FLOAT:
						{
							dataType = ShaderDataType::Types::Float;
							value = 0.0F;
						}
						break;

						case GL_FLOAT_VEC2:
						{
							dataType = ShaderDataType::Types::Float2;
							value = Vector2F();
						}
						break;

						case GL_FLOAT_VEC3:
						{
							dataType = ShaderDataType::Types::Float3;
							value = Vector3F();
						}
						break;

						case GL_FLOAT_VEC4:
						{
							dataType = ShaderDataType::Types::Float4;
							value = Vector4F();
						}
						break;

						case GL_FLOAT_MAT4:
						{
							dataType = ShaderDataType::Types::Matrix4;

							value = Matrix4F::Identity;
						}
						break;

						case GL_SAMPLER_2D:
						{
							dataType = ShaderDataType::Types::Texture2D;
							value = nullptr;
						}
						break;
						}

						Shader::ConstantData data;
						data.Handle = handle;
						data.Name = name;
						data.Type = dataType;;
						data.Value = value;

						Constants.Add(data);
					}

					return true;
				}

				bool OpenGLDevice::GetShaderConstantHandle(Shader::Handle Handle, const String& Name, Shader::ConstantHandle& ConstantHandle)
				{
					ConstantHandle = glGetUniformLocation(Handle, Name.GetValue());
					return true;
				}

				bool OpenGLDevice::SetShaderFloat32(Shader::ConstantHandle Handle, float32 Value)
				{
					glUniform1f(Handle, Value);

					return true;
				}

				bool OpenGLDevice::SetShaderColor(Shader::ConstantHandle Handle, const ColorUI8& Value)
				{
					Vector4F col;
					GetOpenGLColor(Value, col);

					glUniform4f(Handle, col.X, col.Y, col.Z, col.W);

					return true;
				}

				bool OpenGLDevice::SetShaderVector2(Shader::ConstantHandle Handle, const Vector2F& Value)
				{
					glUniform2f(Handle, Value.X, Value.Y);

					return true;
				}

				bool OpenGLDevice::SetShaderVector3(Shader::ConstantHandle Handle, const Vector3F& Value)
				{
					glUniform3f(Handle, Value.X, Value.Y, Value.Z);

					return true;
				}

				bool OpenGLDevice::SetShaderVector4(Shader::ConstantHandle Handle, const Vector4F& Value)
				{
					glUniform4f(Handle, Value.X, Value.Y, Value.Z, Value.W);

					return true;
				}

				bool OpenGLDevice::SetShaderMatrix4(Shader::ConstantHandle Handle, const Matrix4F& Value)
				{
					glUniformMatrix4fv(Handle, 1, false, Value.GetValue());

					return true;
				}

				bool OpenGLDevice::SetShaderTexture(Shader::ConstantHandle Handle, Texture::Types Type, Texture::Handle Value)
				{
					glActiveTexture(GL_TEXTURE0 + m_LastActiveTextureUnitIndex);

					BindTexture(Value, Type);

					glUniform1i(Handle, m_LastActiveTextureUnitIndex);

					++m_LastActiveTextureUnitIndex;

					return true;
				}

				bool OpenGLDevice::CreateTexture(const TextureInfo* Info, Texture::Handle& Handle)
				{
					glGenTextures(1, &Handle);

					BindTexture(Handle, Info->Type);

					if (Texture::GetChannelCount(Info->Format) == 3)
						glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					else
						glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

					glTexImage2D(GetTextureType(Info->Type), 0, GetTextureInternalFormat(Info->Format), Info->Dimension.X, Info->Dimension.Y, 0, GetTextureFormat(Info->Format), GetTexturePixelType(Info->Format), Info->Data);

					return true;
				}

				bool OpenGLDevice::DestroyTexture(Texture::Handle Handle)
				{
					glDeleteTextures(1, &Handle);

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
					glGenFramebuffers(1, &Handle);

					BindRenderTarget(Handle);

					m_RenderTargets[Handle] = {};

					auto& texturesList = m_RenderTargets[Handle];

					static uint32 drawBuffers[((int8)RenderTarget::AttachmentPoints::Color15 - (int8)RenderTarget::AttachmentPoints::Color0) + 1];

					int drawBufferIndex = 0;

					for each (const auto & textureInfo in Info->Textures)
					{
						TextureInfo info;
						info.Type = Texture::Types::TwoD;
						info.Dimension = textureInfo.Dimension;
						info.Format = textureInfo.Format;

						Texture::Handle texHandle;
						if (!CreateTexture(&info, texHandle))
							return false;

						uint32 point = GetAttachmentPoint(textureInfo.Point);

						glFramebufferTexture2D(GL_FRAMEBUFFER, point, GetTextureType(info.Type), texHandle, 0);

						texturesList.Texture.Add(texHandle);

						if (textureInfo.Point >= RenderTarget::AttachmentPoints::Color0)
							drawBuffers[drawBufferIndex++] = point;
					}

					Textures.AddRange(texturesList.Texture);

					glDrawBuffers(drawBufferIndex, drawBuffers);

					BindRenderTarget(0);

					return true;
				}

				bool OpenGLDevice::DestroyRenderTarget(RenderTarget::Handle Handle)
				{
					if (!m_RenderTargets.Contains(Handle))
						return false;

					auto& info = m_RenderTargets[Handle];

					for each (auto handle in info.Texture)
						DestroyTexture(handle);

					glDeleteFramebuffers(1, &Handle);

					m_RenderTargets.Remove(Handle);

					return true;
				}

				bool OpenGLDevice::BindRenderTarget(RenderTarget::Handle Handle)
				{
					glBindFramebuffer(GL_FRAMEBUFFER, Handle);

					return true;
				}

				bool OpenGLDevice::CreateMesh(const SubMeshInfo* Info, GPUBuffer::Usages Usage, SubMesh::Handle& Handle)
				{
					if (Info->Vertices.GetSize() == 0)
						return false;

					uint32 vbo;
					if (!CreateBuffer(vbo))
						return false;

					if (!AttachBufferData(vbo, GPUBuffer::Types::Array, Usage, SubMesh::GetVertexBufferSize(Info->Vertices.GetSize()), Info->Vertices.GetData()))
						return false;

					uint32 ebo = 0;
					if (Info->Indices.GetSize() != 0)
					{
						if (!CreateBuffer(ebo))
							return false;

						if (!AttachBufferData(ebo, GPUBuffer::Types::ElementArray, Usage, SubMesh::GetIndexBufferSize(Info->Indices.GetSize()), Info->Indices.GetData()))
							return false;
					}

					Handle = ++m_LastMeshNumber;
					m_MeshBuffers[Handle] = { vbo, ebo, Info->Layout };

					return true;
				}

				bool OpenGLDevice::GetMeshVertexBuffer(SubMesh::Handle Handle, GPUBuffer::Handle& BufferHandle)
				{
					if (m_CurrentContext == nullptr)
						return false;

					if (!m_MeshBuffers.Contains(Handle))
						return false;

					BufferHandle = m_MeshBuffers[Handle].VertexBufferObject;

					return true;
				}

				bool OpenGLDevice::GetMeshElementBuffer(SubMesh::Handle Handle, GPUBuffer::Handle& BufferHandle)
				{
					if (m_CurrentContext == nullptr)
						return false;

					if (!m_MeshBuffers.Contains(Handle))
						return false;

					BufferHandle = m_MeshBuffers[Handle].IndexBufferObject;

					return true;
				}

				bool OpenGLDevice::DestroyMesh(SubMesh::Handle Handle)
				{
					if (!m_MeshBuffers.Contains(Handle))
						return false;

					auto& info = m_MeshBuffers[Handle];

					DestroyBuffer(info.VertexBufferObject);

					if (info.IndexBufferObject != 0)
						DestroyBuffer(info.IndexBufferObject);

					for each (auto context in m_Contexts)
						context->DestroyVertexArray(Handle);

					m_MeshBuffers.Remove(Handle);

					return true;
				}

				bool OpenGLDevice::CreateVertexArray(const MeshBufferInfo& Info, NativeType::Handle& Handle)
				{
					glGenVertexArrays(1, &Handle);
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
					glDeleteVertexArrays(1, &Handle);

					return true;
				}

				bool OpenGLDevice::BindMesh(SubMesh::Handle Handle)
				{
					if (m_CurrentContext == nullptr)
						return false;

					if (!m_MeshBuffers.Contains(Handle))
						return false;

					return m_CurrentContext->BindVertextArray(Handle, m_MeshBuffers[Handle]);
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

					PlatformWindow::SwapBuffers(m_CurrentContext->GetContextHandle());

					return true;
				}

				bool OpenGLDevice::SetDebugCallback(DebugProcedureType Callback)
				{
					m_Callback = Callback;

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
			}
		}
	}
}