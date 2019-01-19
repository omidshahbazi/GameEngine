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

					return GL_FRONT_AND_BACK;
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

				uint32 GetTextureFormat(IDevice::TextureFormats Format)
				{
					switch (Format)
					{
					case IDevice::TextureFormats::R8:
						return GL_R8I;
					case IDevice::TextureFormats::R16:
						return GL_R16;
					case IDevice::TextureFormats::R32:
						return GL_R32I;
					case IDevice::TextureFormats::R8F:
						return GL_R8;
					case IDevice::TextureFormats::R16F:
						return GL_R16F;
					case IDevice::TextureFormats::R32F:
						return GL_R32F;
					case IDevice::TextureFormats::RG8:
						return GL_RG8I;
					case IDevice::TextureFormats::RG16:
						return GL_RG16;
					case IDevice::TextureFormats::RG32:
						return GL_RG32I;
					case IDevice::TextureFormats::RGB8:
						return GL_RGB8;
					case IDevice::TextureFormats::RGB16:
						return GL_RGB16;
					case IDevice::TextureFormats::RGB32:
						return GL_RGB32F;
					case IDevice::TextureFormats::RGBA8:
						return GL_RGBA8;
					case IDevice::TextureFormats::RGBA16:
						return GL_RGBA16;
					case IDevice::TextureFormats::RGBA32:
						return GL_RGBA32F;
					}

					return GL_RGBA;
				}

				uint32 GetWrapMode(IDevice::TextureWrapModes Mode)
				{
					switch (Mode)
					{
					case IDevice::TextureWrapModes::Clamp:
						return GL_CLAMP;
					case IDevice::TextureWrapModes::Repeat:
						return GL_REPEAT;
					case IDevice::TextureWrapModes::ClampToEdge:
						return GL_CLAMP_TO_EDGE;
					case IDevice::TextureWrapModes::MirroredRepeat:
						return GL_MIRRORED_REPEAT;
					}

					return GL_CLAMP;
				}

				uint32 GetMinifyFilter(IDevice::MinifyFilters Filter)
				{
					switch (Filter)
					{
					case IDevice::MinifyFilters::Nearest:
						return GL_NEAREST;
					case IDevice::MinifyFilters::Linear:
						return GL_LINEAR;
					case IDevice::MinifyFilters::NearestMipMapNearest:
						return GL_NEAREST_MIPMAP_NEAREST;
					case IDevice::MinifyFilters::LinearMipMapNearest:
						return GL_LINEAR_MIPMAP_NEAREST;
					case IDevice::MinifyFilters::NearestMipMapLinear:
						return GL_NEAREST_MIPMAP_LINEAR;
					case IDevice::MinifyFilters::LinearMipMapLinear:
						return GL_LINEAR_MIPMAP_LINEAR;
					}

					return GL_NEAREST;
				}

				uint32 GetMagnifyFilter(IDevice::MagnfyFilters Filter)
				{
					switch (Filter)
					{
					case IDevice::MagnfyFilters::Nearest:
						return GL_NEAREST;
					case IDevice::MagnfyFilters::Linear:
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

				OpenGLDevice::OpenGLDevice(void) :
					m_SampleCount(0),
					m_ForwardCompatible(0),
					m_ClearColor(Color()),
					m_ClearFlags(ClearFlags::ColorBuffer),
					m_FaceOrder(FaceOrders::CounterClockwise),
					m_CullMode(CullModes::Back),
					m_DepthTestFunction(TestFunctions::Less),
					m_StencilTestFunction(TestFunctions::Always),
					m_StencilTestFunctionReference(0),
					m_StencilTestFunctionMask(0xFF),
					m_StencilMask(0xFF),
					m_StencilOperationStencilFailed(StencilOperations::Keep),
					m_StencilOperationDepthFailed(StencilOperations::Keep),
					m_StencilOperationDepthPassed(StencilOperations::Keep),
					m_BlendFunctionSourceFactor(BlendFunctions::One),
					m_BlendFunctionDestinationFactor(BlendFunctions::Zero),
					m_PolygonMode(PolygonModes::Fill),
					m_PolygonModeCullMode(CullModes::Both),
					m_LastProgram(0),
					m_LastBuffer(0),
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
					if (m_ClearColor == Color)
						return;

					m_ClearColor = Color;

					glClearColor(m_ClearColor.GetFloat32R(), m_ClearColor.GetFloat32G(), m_ClearColor.GetFloat32B(), m_ClearColor.GetFloat32A());
				}

				void OpenGLDevice::SetClearFlags(ClearFlags Flags)
				{
					m_ClearFlags = Flags;
				}

				void OpenGLDevice::SetFaceOrder(FaceOrders Order)
				{
					if (m_FaceOrder == Order)
						return;

					m_FaceOrder = Order;

					glFrontFace(GetFaceOrdering(m_FaceOrder));
				}

				void OpenGLDevice::SetCullMode(CullModes Mode)
				{
					if (m_CullMode == Mode)
						return;

					m_CullMode = Mode;

					if (m_CullMode == CullModes::None)
						glDisable(GL_CULL_FACE);
					else
					{
						glEnable(GL_CULL_FACE);
						glCullFace(GetCullingMode(m_CullMode));
					}
				}

				void OpenGLDevice::SetDepthTestFunction(TestFunctions Function)
				{
					if (m_DepthTestFunction == Function)
						return;

					m_DepthTestFunction = Function;

					if (m_DepthTestFunction == TestFunctions::Never)
						glDisable(GL_DEPTH_TEST);
					else
					{
						glEnable(GL_DEPTH_TEST);
						glDepthFunc(GetTestFunction(m_DepthTestFunction));
					}
				}

				void OpenGLDevice::SetStencilTestFunction(TestFunctions Function, int32 Reference, uint32 Mask)
				{
					if (m_StencilTestFunction == Function && m_StencilTestFunctionReference == Reference && m_StencilTestFunctionMask == Mask)
						return;

					m_StencilTestFunction = Function;
					m_StencilTestFunctionReference = Reference;
					m_StencilTestFunctionMask = Mask;

					if (m_StencilTestFunction == TestFunctions::Never)
						glDisable(GL_STENCIL_TEST);
					else
					{
						glEnable(GL_STENCIL_TEST);
						glStencilFunc(GetTestFunction(m_StencilTestFunction), m_StencilTestFunctionReference, m_StencilTestFunctionMask);
					}
				}

				void OpenGLDevice::SetStencilMask(uint32 Mask)
				{
					if (m_StencilMask == Mask)
						return;

					m_StencilMask = Mask;

					glStencilMask(m_StencilMask);
				}

				void OpenGLDevice::SetStencilOperation(StencilOperations StencilFailed, StencilOperations DepthFailed, StencilOperations DepthPassed)
				{
					if (m_StencilOperationStencilFailed == StencilFailed && m_StencilOperationDepthFailed == DepthFailed && m_StencilOperationDepthPassed == DepthPassed)
						return;

					m_StencilOperationStencilFailed = StencilFailed;
					m_StencilOperationDepthFailed = DepthFailed;
					m_StencilOperationDepthPassed = DepthPassed;

					glStencilOp(GetStencilingOperation(m_StencilOperationStencilFailed), GetStencilingOperation(m_StencilOperationDepthFailed), GetStencilingOperation(m_StencilOperationDepthPassed));
				}

				void OpenGLDevice::SetBlendFunction(BlendFunctions SourceFactor, BlendFunctions DestinationFactor)
				{
					if (m_BlendFunctionSourceFactor == SourceFactor && m_BlendFunctionDestinationFactor == DestinationFactor)
						return;

					m_BlendFunctionSourceFactor = SourceFactor;
					m_BlendFunctionSourceFactor = DestinationFactor;

					if (m_BlendFunctionSourceFactor == BlendFunctions::One && m_BlendFunctionDestinationFactor == BlendFunctions::Zero)
						glDisable(GL_BLEND);
					else
					{
						glEnable(GL_BLEND);
						glBlendFunc(GetBlendingFunction(m_BlendFunctionSourceFactor), GetBlendingFunction(m_BlendFunctionDestinationFactor));
					}
				}

				void OpenGLDevice::SetPolygonMode(CullModes CullMode, PolygonModes PolygonMode)
				{
					if (m_PolygonModeCullMode == CullMode && m_PolygonMode == PolygonMode)
						return;

					m_PolygonModeCullMode = CullMode;
					m_PolygonMode = PolygonMode;

					glPolygonMode(GetCullingMode(m_PolygonModeCullMode), GetPolygonRenderMode(m_PolygonMode));
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

				bool OpenGLDevice::CreateTexture2D(const byte *Data, uint32 Width, uint32 Height, uint8 ComponentCount, IDevice::TextureFormats Format, Texture::Handle &Handle)
				{
					glGenTextures(1, &Handle);

					BindTexture2D(Handle);

					glTexImage2D(GL_TEXTURE_2D, 0, (ComponentCount == 3 ? GL_RGB : GL_RGBA), Width, Height, 0, GetTextureFormat(Format), GL_UNSIGNED_BYTE, Data);

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

				bool OpenGLDevice::SetTexture2DVerticalWrapping(Texture::Handle Handle, TextureWrapModes Mode)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GetWrapMode(Mode));

					return true;
				}

				bool OpenGLDevice::SetTexture2DHorizontalWrapping(Texture::Handle Handle, TextureWrapModes Mode)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GetWrapMode(Mode));

					return true;
				}

				bool OpenGLDevice::SetTexture2DMinifyFilter(Texture::Handle Handle, MinifyFilters Filter)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GetMinifyFilter(Filter));

					return true;
				}

				bool OpenGLDevice::SetTexture2DMagnifyFilter(Texture::Handle Handle, MagnfyFilters Filter)
				{
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GetMagnifyFilter(Filter));

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
					if (m_LastBuffer == Handle)
						return true;
					m_LastBuffer = Handle;

					if (!m_MeshBuffers.Contains(m_LastBuffer))
						return false;

					auto &info = m_MeshBuffers[m_LastBuffer];

					glBindVertexArray(info.VertexArrayObject);

					return true;
				}

				void OpenGLDevice::Clear(void)
				{
					glClear(GetClearingFlags(m_ClearFlags));
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
