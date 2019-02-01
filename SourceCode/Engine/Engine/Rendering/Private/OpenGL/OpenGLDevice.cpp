// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\OpenGL\OpenGLDevice.h>
#include <Debugging\Debug.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Platform\PlatformMemory.h>

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

				uint32 GetClearingFlags(IDevice::ClearFlags Flags)
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

				uint32 GetFaceOrdering(IDevice::FaceOrders Order)
				{
					switch (Order)
					{
					case IDevice::FaceOrders::Clockwise:
						return GL_CW;
					case IDevice::FaceOrders::CounterClockwise:
						return GL_CCW;
					}

					return GL_CW;
				}

				uint32 GetCullingMode(IDevice::CullModes Modes)
				{
					if (BitwiseUtils::IsEnabled(Modes, IDevice::CullModes::Front) && BitwiseUtils::IsEnabled(Modes, IDevice::CullModes::Back))
						return GL_FRONT_AND_BACK;

					if (BitwiseUtils::IsEnabled(Modes, IDevice::CullModes::Front))
						return GL_FRONT;

					if (BitwiseUtils::IsEnabled(Modes, IDevice::CullModes::Back))
						return GL_BACK;

					return GL_NONE;
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

					return GL_NEVER;
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

					return GL_KEEP;
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

					return GL_ZERO;
				}

				uint32 GetTextureFormat(Texture::Formats Format)
				{
					switch (Format)
					{
					case Texture::Formats::R8:
						return GL_R8I;
					case Texture::Formats::R16:
						return GL_R16;
					case Texture::Formats::R32:
						return GL_R32I;
					case Texture::Formats::R8F:
						return GL_R8;
					case Texture::Formats::R16F:
						return GL_R16F;
					case Texture::Formats::R32F:
						return GL_R32F;
					case Texture::Formats::RG8:
						return GL_RG8I;
					case Texture::Formats::RG16:
						return GL_RG16;
					case Texture::Formats::RG32:
						return GL_RG32I;
					case Texture::Formats::RGB:
						return GL_RGB;
					case Texture::Formats::RGB8:
						return GL_RGB8;
					case Texture::Formats::RGB16:
						return GL_RGB16;
					case Texture::Formats::RGB32:
						return GL_RGB32F;
					case Texture::Formats::RGBA:
						return GL_RGBA;
					case Texture::Formats::RGBA8:
						return GL_RGBA8;
					case Texture::Formats::RGBA16:
						return GL_RGBA16;
					case Texture::Formats::RGBA32:
						return GL_RGBA32F;
					}

					return GL_RGBA;
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

					return GL_CLAMP;
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

					return GL_NEAREST;
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

					return GL_NEAREST;
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

					return GL_FILL;
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
					case RenderTarget::AttachmentPoints::Color16: return GL_COLOR_ATTACHMENT15;
					}

					return GL_COLOR_ATTACHMENT0;
				}

				OpenGLDevice::OpenGLDevice(void) :
					m_SampleCount(0),
					m_ForwardCompatible(false),
					m_LastProgram(0),
					m_LastMeshBuffer(0),
					m_LastFrameBuffer(0),
					m_LastActiveTextureUnitIndex(0)
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

				bool OpenGLDevice::SecondInitialize(void)
				{
					glewExperimental = true;
					if (glewInit() != GLEW_OK)
					{
						PlatformMemory::Copy("GLEW initialization failed", m_LastError, 26);

						return false;
					}

					m_State.DepthTestFunction = TestFunctions::Never;
					m_State.SetStencilTestFunction(TestFunctions::Never, 0, 0);
					State state;
					SetState(state);

					return true;
				}

				void OpenGLDevice::SetSampleCount(uint8 Count)
				{
					if (m_SampleCount == Count)
						return;

					m_SampleCount = Count;

					glfwWindowHint(GLFW_SAMPLES, m_SampleCount);
				}

				void OpenGLDevice::SetForwardCompatible(bool Value)
				{
					if (m_ForwardCompatible == Value)
						return;

					m_ForwardCompatible = Value;

					glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, (m_ForwardCompatible ? GL_TRUE : GL_FALSE));
				}

				void OpenGLDevice::SetClearColor(Color Color)
				{
					if (m_State.ClearColor == Color)
						return;

					m_State.ClearColor = Color;

					glClearColor(m_State.ClearColor.GetFloat32R(), m_State.ClearColor.GetFloat32G(), m_State.ClearColor.GetFloat32B(), m_State.ClearColor.GetFloat32A());
				}

				void OpenGLDevice::SetClearFlags(ClearFlags Flags)
				{
					m_State.ClearFlags = Flags;
				}

				void OpenGLDevice::SetFaceOrder(FaceOrders Order)
				{
					if (m_State.FaceOrder == Order)
						return;

					m_State.FaceOrder = Order;

					glFrontFace(GetFaceOrdering(m_State.FaceOrder));
				}

				void OpenGLDevice::SetCullMode(CullModes Mode)
				{
					if (m_State.CullMode == Mode)
						return;

					m_State.CullMode = Mode;

					if (m_State.CullMode == CullModes::None)
						glDisable(GL_CULL_FACE);
					else
					{
						glEnable(GL_CULL_FACE);
						glCullFace(GetCullingMode(m_State.CullMode));
					}
				}

				void OpenGLDevice::SetDepthTestFunction(TestFunctions Function)
				{
					if (m_State.DepthTestFunction == Function)
						return;

					m_State.DepthTestFunction = Function;

					if (m_State.DepthTestFunction == TestFunctions::Never)
						glDisable(GL_DEPTH_TEST);
					else
					{
						glEnable(GL_DEPTH_TEST);
						glDepthFunc(GetTestFunction(m_State.DepthTestFunction));
					}
				}

				void OpenGLDevice::SetStencilTestFunction(CullModes CullMode, TestFunctions Function, int32 Reference, uint32 Mask)
				{
					State::FaceState &state = m_State.GetFaceState(CullMode);

					if (state.StencilTestFunction == Function && state.StencilTestFunctionReference == Reference && state.StencilTestFunctionMask == Mask)
						return;

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
				}

				void OpenGLDevice::SetStencilMask(CullModes CullMode, uint32 Mask)
				{
					State::FaceState &state = m_State.GetFaceState(CullMode);

					if (state.StencilMask == Mask)
						return;

					state.StencilMask = Mask;

					glStencilMaskSeparate(GetCullingMode(CullMode), state.StencilMask);
				}

				void OpenGLDevice::SetStencilOperation(CullModes CullMode, StencilOperations StencilFailed, StencilOperations DepthFailed, StencilOperations DepthPassed)
				{
					State::FaceState &state = m_State.GetFaceState(CullMode);

					if (state.StencilOperationStencilFailed == StencilFailed && state.StencilOperationDepthFailed == DepthFailed && state.StencilOperationDepthPassed == DepthPassed)
						return;

					state.StencilOperationStencilFailed = StencilFailed;
					state.StencilOperationDepthFailed = DepthFailed;
					state.StencilOperationDepthPassed = DepthPassed;

					glStencilOpSeparate(GetCullingMode(CullMode), GetStencilingOperation(state.StencilOperationStencilFailed), GetStencilingOperation(state.StencilOperationDepthFailed), GetStencilingOperation(state.StencilOperationDepthPassed));
				}

				void OpenGLDevice::SetBlendFunction(BlendFunctions SourceFactor, BlendFunctions DestinationFactor)
				{
					if (m_State.BlendFunctionSourceFactor == SourceFactor && m_State.BlendFunctionDestinationFactor == DestinationFactor)
						return;

					m_State.BlendFunctionSourceFactor = SourceFactor;
					m_State.BlendFunctionDestinationFactor = DestinationFactor;

					if (m_State.BlendFunctionSourceFactor == BlendFunctions::One && m_State.BlendFunctionDestinationFactor == BlendFunctions::Zero)
						glDisable(GL_BLEND);
					else
					{
						glEnable(GL_BLEND);
						glBlendFunc(GetBlendingFunction(m_State.BlendFunctionSourceFactor), GetBlendingFunction(m_State.BlendFunctionDestinationFactor));
					}
				}

				void OpenGLDevice::SetPolygonMode(CullModes CullMode, PolygonModes PolygonMode)
				{
					State::FaceState &state = m_State.GetFaceState(CullMode);

					if (state.PolygonMode == PolygonMode)
						return;

					state.PolygonMode = PolygonMode;

					glPolygonMode(GetCullingMode(CullMode), GetPolygonRenderMode(state.PolygonMode));
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
					if (m_LastProgram == Handle)
						return true;
					m_LastProgram = Handle;

					glUseProgram(m_LastProgram);

					return true;
				}

				bool OpenGLDevice::QueryProgramActiveConstants(Program::Handle Handle, Program::ConstantDataList &Constants)
				{
					int32 count;

					glGetProgramiv(Handle, GL_ACTIVE_UNIFORMS, &count);

					const uint8 bufferSize = 32;
					char8 name[bufferSize];
					int32 nameLength;
					int32 constantSize;
					uint32 type;

					for (int8 i = 0; i < count; i++)
					{
						glGetActiveUniform(Handle, (GLuint)i, bufferSize, &nameLength, &constantSize, &type, name);

						name[nameLength] = CharacterUtility::Character<char8, '\0'>::Value;

						Program::ConstantHandle handle;
						GetProgramConstantHandle(Handle, name, handle);

						DataTypes dataType = DataTypes::Unknown;
						AnyDataType value;

						switch (type)
						{
						case GL_FLOAT:
						{
							dataType = DataTypes::Float;
							value = 0.0F;
						}
						break;

						case GL_FLOAT_VEC2:
						{
							dataType = DataTypes::Float2;
							value = Vector2F();
						}
						break;

						case GL_FLOAT_VEC3:
						{
							dataType = DataTypes::Float3;
							value = Vector2F();
						}
						break;

						case GL_FLOAT_MAT4:
						{
							dataType = DataTypes::Matrix4;

							Matrix4F mat;
							mat.MakeIdentity();
							value = mat;
						}
						break;

						case GL_SAMPLER_2D:
						{
							dataType = DataTypes::Texture2D;
							value = nullptr;
						}
						break;
						}

						Constants.Add({ handle, name, dataType, value });
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

				bool OpenGLDevice::SetProgramVector4(Program::ConstantHandle Handle, const Vector4F & Value)
				{
					glUniform4f(Handle, Value.X, Value.Y, Value.Z, Value.W);

					return true;
				}

				bool OpenGLDevice::SetProgramMatrix4(Program::ConstantHandle Handle, const Matrix4F &Value)
				{
					glUniformMatrix4fv(Handle, 1, false, Value.GetValue());

					return true;
				}

				bool OpenGLDevice::SetProgramTexture(Program::ConstantHandle Handle, Texture::Handle Value)
				{
					glActiveTexture(GL_TEXTURE0 + m_LastActiveTextureUnitIndex);

					BindTexture2D(Value);

					glUniform1i(Handle, m_LastActiveTextureUnitIndex);

					++m_LastActiveTextureUnitIndex;

					return true;
				}

				bool OpenGLDevice::CreateTexture2D(const byte *Data, uint32 Width, uint32 Height, Texture::Formats Format, Texture::Handle &Handle)
				{
					glGenTextures(1, &Handle);

					BindTexture2D(Handle);

					uint32 format = GetTextureFormat(Format);

					glTexImage2D(GL_TEXTURE_2D, 0, format, Width, Height, 0, format, GL_UNSIGNED_BYTE, Data);

					GenerateMipMap(Handle);

					return true;
				}

				bool OpenGLDevice::DestroyTexture(Texture::Handle Handle)
				{
					glDeleteTextures(1, &Handle);

					return true;
				}

				bool OpenGLDevice::BindTexture2D(Program::Handle Handle)
				{
					glBindTexture(GL_TEXTURE_2D, Handle);

					return true;
				}

				bool OpenGLDevice::SetTexture2DVerticalWrapping(Texture::Handle Handle, Texture::WrapModes Mode)
				{
					BindTexture2D(Handle);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetWrapMode(Mode));

					return true;
				}

				bool OpenGLDevice::SetTexture2DHorizontalWrapping(Texture::Handle Handle, Texture::WrapModes Mode)
				{
					BindTexture2D(Handle);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetWrapMode(Mode));

					return true;
				}

				bool OpenGLDevice::SetTexture2DMinifyFilter(Texture::Handle Handle, Texture::MinifyFilters Filter)
				{
					BindTexture2D(Handle);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetMinifyFilter(Filter));

					return true;
				}

				bool OpenGLDevice::SetTexture2DMagnifyFilter(Texture::Handle Handle, Texture::MagnfyFilters Filter)
				{
					BindTexture2D(Handle);

					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetMagnifyFilter(Filter));

					return true;
				}

				bool OpenGLDevice::GenerateMipMap(Texture::Handle Handle)
				{
					BindTexture2D(Handle);

					glGenerateMipmap(GL_TEXTURE_2D);

					return true;
				}

				bool OpenGLDevice::CreateRenderTarget(uint32 Width, uint32 Height, RenderTarget::Formats Format, RenderTarget::AttachmentPoints Point, RenderTarget::Handle & Handle)
				{
					RenderTarget::Handle renderTarget;
					glGenFramebuffers(1, &renderTarget);

					glBindFramebuffer(GL_FRAMEBUFFER, renderTarget);

					CreateTexture2D(nullptr, Width, Height, Format, Handle);
					SetTexture2DMinifyFilter(Handle, Texture::MinifyFilters::Nearest);
					SetTexture2DMagnifyFilter(Handle, Texture::MagnfyFilters::Nearest);
					SetTexture2DVerticalWrapping(Handle, Texture::WrapModes::ClampToEdge);
					SetTexture2DVerticalWrapping(Handle, Texture::WrapModes::ClampToEdge);

					glFramebufferTexture2D(GL_FRAMEBUFFER, GetAttachmentPoint(Point), GL_TEXTURE_2D, Handle, 0);

					GPUBuffer::Handle renderBuffer;
					//glGenRenderbuffers(1, &renderBuffer);

					//glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);

					//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Width, Height);
					//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GetAttachmentPoint(RenderTarget::AttachmentPoints::Depth), GL_RENDERBUFFER, renderBuffer);

					//GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
					//glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

					if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					{

						glBindFramebuffer(GL_FRAMEBUFFER, 0);
					}

					glBindFramebuffer(GL_FRAMEBUFFER, 0);

					m_FrameBuffers[Handle] = { renderTarget, renderBuffer };

					return true;
				}

				bool OpenGLDevice::DestroyRenderTarget(RenderTarget::Handle Handle)
				{
					if (!m_FrameBuffers.Contains(Handle))
						return false;

					auto &info = m_FrameBuffers[Handle];

					DestroyTexture(Handle);
					glDeleteFramebuffers(1, &info.RenderTarget);

					m_FrameBuffers.Remove(Handle);

					return true;
				}

				bool OpenGLDevice::BindRenderTarget(RenderTarget::Handle Handle)
				{
					if (m_LastFrameBuffer == Handle)
						return true;
					m_LastFrameBuffer = Handle;

					RenderTarget::Handle finalHandle = Handle;

					if (m_FrameBuffers.Contains(m_LastFrameBuffer))
						finalHandle = m_FrameBuffers[m_LastFrameBuffer].RenderTarget;

					glBindFramebuffer(GL_FRAMEBUFFER, finalHandle);

					glViewport(0, 0, 1024, 768);

					return true;
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
						uint16 index = SubMeshInfo::GetLayoutIndex(SubMeshInfo::VertexLayouts::Normal);

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

				bool OpenGLDevice::BindMesh(GPUBuffer::Handle Handle)
				{
					if (m_LastMeshBuffer == Handle)
						return true;
					m_LastMeshBuffer = Handle;

					if (!m_MeshBuffers.Contains(m_LastMeshBuffer))
						return false;

					auto &info = m_MeshBuffers[m_LastMeshBuffer];

					glBindVertexArray(info.VertexArrayObject);

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

					if (SecondInitialize())
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
					glClear(GetClearingFlags(m_State.ClearFlags));
				}

				void OpenGLDevice::Draw(DrawModes Mode, uint32 Count)
				{
					glDrawElements(GetDrawMode(Mode), Count, GL_UNSIGNED_INT, 0);
					m_LastActiveTextureUnitIndex = 0;
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
