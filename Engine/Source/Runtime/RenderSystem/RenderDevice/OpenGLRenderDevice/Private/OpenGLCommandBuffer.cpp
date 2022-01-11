// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <OpenGLRenderDevice\Private\OpenGLCommandBuffer.h>
#include <OpenGLRenderDevice\Private\OpenGLDevice.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <RenderCommon\Helper.h>
#include <Common\TypeTraits.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace RenderCommon;
	using namespace RenderCommon::Private;

	namespace OpenGLRenderDevice
	{
		namespace Private
		{
			uint32 GetCullingMode(CullModes Modes)
			{
				if (BitwiseUtils::IsEnabled(Modes, CullModes::Front) && BitwiseUtils::IsEnabled(Modes, CullModes::Back))
					return GL_FRONT_AND_BACK;

				if (BitwiseUtils::IsEnabled(Modes, CullModes::Front))
					return GL_FRONT;

				if (BitwiseUtils::IsEnabled(Modes, CullModes::Back))
					return GL_BACK;

				return 0;
			}

			uint32 GetTestFunction(TestFunctions Function)
			{
				switch (Function)
				{
				case TestFunctions::Never:
					return GL_NEVER;
				case TestFunctions::Less:
					return GL_LESS;
				case TestFunctions::LessEqual:
					return GL_LEQUAL;
				case TestFunctions::Equal:
					return GL_EQUAL;
				case TestFunctions::NotEqual:
					return GL_NOTEQUAL;
				case TestFunctions::GreaterEqual:
					return GL_GEQUAL;
				case TestFunctions::Greater:
					return GL_GREATER;
				case TestFunctions::Always:
					return GL_ALWAYS;
				}

				return 0;
			}

			uint32 GetStencilingOperation(StencilOperations Operation)
			{
				switch (Operation)
				{
				case StencilOperations::Keep:
					return GL_KEEP;
				case StencilOperations::Zero:
					return GL_ZERO;
				case StencilOperations::Replace:
					return GL_REPLACE;
				case StencilOperations::Increament:
					return GL_INCR;
				case StencilOperations::IncreamentWrap:
					return GL_INCR_WRAP;
				case StencilOperations::Decreament:
					return GL_DECR;
				case StencilOperations::DecreamentWrap:
					return GL_DECR_WRAP;
				case StencilOperations::Invert:
					return GL_INVERT;
				}

				return 0;
			}

			uint32 GetPolygonRenderMode(PolygonModes Mode)
			{
				switch (Mode)
				{
				case PolygonModes::Point:
					return GL_POINT;
				case PolygonModes::Line:
					return GL_LINE;
				case PolygonModes::Fill:
					return GL_FILL;
				}

				return 0;
			}

			uint32 GetFaceOrdering(FaceOrders Order)
			{
				switch (Order)
				{
				case FaceOrders::Clockwise:
					return GL_CW;
				case FaceOrders::CounterClockwise:
					return GL_CCW;
				}

				return 0;
			}

			uint32 GetBlendingEquation(BlendEquations Equation)
			{
				switch (Equation)
				{
				case BlendEquations::Add:
					return GL_FUNC_ADD;
				case BlendEquations::Subtract:
					return GL_FUNC_SUBTRACT;
				case BlendEquations::ReverseSubtract:
					return GL_FUNC_REVERSE_SUBTRACT;
				case BlendEquations::Min:
					return GL_MIN;
				case BlendEquations::Max:
					return GL_MAX;
				}

				return 0;
			}

			uint32 GetBlendingFunction(BlendFunctions Function)
			{
				switch (Function)
				{
				case BlendFunctions::Zero:
					return GL_ZERO;
				case BlendFunctions::One:
					return GL_ONE;
				case BlendFunctions::SourceColor:
					return GL_SRC_COLOR;
				case BlendFunctions::OneMinusSourceColor:
					return GL_ONE_MINUS_SRC_COLOR;
				case BlendFunctions::DestinationColor:
					return GL_DST_COLOR;
				case BlendFunctions::OneMinusDestinationColor:
					return GL_ONE_MINUS_DST_COLOR;
				case BlendFunctions::SourceAlpha:
					return GL_SRC_ALPHA;
				case BlendFunctions::OneMinusSourceAlpha:
					return GL_ONE_MINUS_SRC_ALPHA;
				case BlendFunctions::DestinationAlpha:
					return GL_DST_ALPHA;
				case BlendFunctions::OneMinusDestinationAlpha:
					return GL_ONE_MINUS_DST_ALPHA;
				case BlendFunctions::ConstantColor:
					return GL_CONSTANT_COLOR;
				case BlendFunctions::OneMinusConstantColor:
					return GL_ONE_MINUS_CONSTANT_COLOR;
				case BlendFunctions::ConstantAlpha:
					return GL_CONSTANT_ALPHA;
				case BlendFunctions::OneMinusConstantAlpha:
					return GL_ONE_MINUS_CONSTANT_ALPHA;
				}

				return 0;
			}

			uint32 GetClearingFlags(ClearFlags Flags)
			{
#define SET_IF_ENABLED(CheckFlagVariable, CheckFlag, FlagVariable, Flag) \
							if ((CheckFlagVariable & CheckFlag) == CheckFlag) \
								FlagVariable |= Flag;

				uint32 flags = 0;

				SET_IF_ENABLED(Flags, ClearFlags::ColorBuffer, flags, GL_COLOR_BUFFER_BIT);
				SET_IF_ENABLED(Flags, ClearFlags::DepthBuffer, flags, GL_DEPTH_BUFFER_BIT);
				SET_IF_ENABLED(Flags, ClearFlags::StencilBuffer, flags, GL_STENCIL_BUFFER_BIT);

				return flags;

#undef SET_IF_ENABLED
			}

			uint32 GetPolygonType(PolygonTypes PolygonType)
			{
				switch (PolygonType)
				{
				case PolygonTypes::Lines:
					return GL_LINES;
				case PolygonTypes::LineLoop:
					return GL_LINE_LOOP;
				case PolygonTypes::LineStrip:
					return GL_LINE_STRIP;

				case PolygonTypes::Triangles:
					return GL_TRIANGLES;
				case PolygonTypes::TriangleStrip:
					return GL_TRIANGLE_STRIP;
				case PolygonTypes::TriangleFan:
					return GL_TRIANGLE_FAN;

				case PolygonTypes::Quads:
					return GL_QUADS;
				case PolygonTypes::QuadStrip:
					return GL_QUAD_STRIP;

				case PolygonTypes::Polygon:
					return GL_POLYGON;
				}

				return 0;
			}

			bool CreateVertexArray(const MeshBufferInfo& Info, ResourceHandle& Handle)
			{
				uint32 handle;
				glGenVertexArrays(1, &handle);
				Handle = handle;

				glBindVertexArray(Handle);

				glBindBuffer(GL_ARRAY_BUFFER, Info.VertexBufferObject->Handle);

				uint32 vertexSize = sizeof(Vertex);

				if (BitwiseUtils::IsEnabled(Info.Layout, VertexLayouts::Position))
				{
					uint16 index = SubMeshInfo::GetLayoutIndex(VertexLayouts::Position);

					glVertexAttribPointer(index, 3, GL_FLOAT, false, vertexSize, (void*)OffsetOf(&Vertex::Position));
					glEnableVertexAttribArray(index++);
				}

				if (BitwiseUtils::IsEnabled(Info.Layout, VertexLayouts::Normal))
				{
					uint16 index = SubMeshInfo::GetLayoutIndex(VertexLayouts::Normal);

					glVertexAttribPointer(index, 3, GL_FLOAT, false, vertexSize, (void*)OffsetOf(&Vertex::Normal));
					glEnableVertexAttribArray(index++);
				}

				if (BitwiseUtils::IsEnabled(Info.Layout, VertexLayouts::TexCoord))
				{
					uint16 index = SubMeshInfo::GetLayoutIndex(VertexLayouts::TexCoord);

					glVertexAttribPointer(index, 2, GL_FLOAT, false, vertexSize, (void*)OffsetOf(&Vertex::UV));
					glEnableVertexAttribArray(index);
				}

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Info.IndexBufferObject->Handle);

				return true;
			}

			OpenGLCommandBuffer::OpenGLCommandBuffer(OpenGLDevice* Device) :
				m_Device(Device),
				m_Type(Types::Graphics),
				m_Buffer(RenderSystemAllocators::CommandBufferAllocator)
			{
			}

			void OpenGLCommandBuffer::CopyBuffer(GPUBufferTypes Type, ResourceHandle SourceHandle, bool SourceIsABuffer, ResourceHandle DestinationHandle, bool DestinationIsABuffer)
			{
				CoreDebugAssert(Categories::RenderSystem, Type != GPUBufferTypes::Constant, "Type couldn't be Constant");
				CoreDebugAssert(Categories::RenderSystem, SourceHandle != 0, "SourceHandle is invalid");
				CoreDebugAssert(Categories::RenderSystem, DestinationHandle != 0, "DestinationHandle is invalid");

				m_Buffer.Append(CommandTypes::CopyBuffer);

				CopyBufferCommandData data = {};
				data.Type = Type;
				data.Source = SourceHandle;
				data.SourceIsABuffer = SourceIsABuffer;
				data.Destination = DestinationHandle;
				data.DestinationIsABuffer = DestinationIsABuffer;

				m_Buffer.Append(data);
			}

			void OpenGLCommandBuffer::SetProgram(ResourceHandle Handle)
			{
				CoreDebugAssert(Categories::RenderSystem, Handle != 0, "Program is invalid");

				m_Buffer.Append(CommandTypes::SetProgram);

				SetProgramCommandData data = {};
				data.Program = ReinterpretCast(ProgramInfo*, Handle);

				m_Buffer.Append(data);
			}

			void OpenGLCommandBuffer::SetProgramConstantBuffer(ProgramConstantHandle Handle, ResourceHandle Value)
			{
				CoreDebugAssert(Categories::RenderSystem, Value != 0, "Value is invalid");

				m_Buffer.Append(CommandTypes::SetProgramConstantBuffer);

				SetProgramConstantBufferCommandData data = {};
				data.Handle = Handle;
				data.Value = ReinterpretCast(BufferInfo*, Value);

				m_Buffer.Append(data);
			}

			void OpenGLCommandBuffer::SetProgramTexture(ProgramConstantHandle Handle, ResourceHandle Value)
			{
				CoreDebugAssert(Categories::RenderSystem, Value != 0, "Value is invalid");

				m_Buffer.Append(CommandTypes::SetProgramTexture);

				SetProgramTextureCommandData data = {};
				data.Handle = Handle;
				data.Value = ReinterpretCast(TextureBufferInfo*, Value);

				m_Buffer.Append(data);
			}

			void OpenGLCommandBuffer::SetState(const RenderState& State)
			{
				m_Buffer.Append(CommandTypes::SetState);

				SetStateCommandData data = {};
				data.State = State;

				m_Buffer.Append(data);
			}

			void OpenGLCommandBuffer::SetRenderTarget(ResourceHandle Handle)
			{
				m_Buffer.Append(CommandTypes::SetRenderTarget);

				SetRenderTargetCommandData data = {};
				data.RenderTarget = ReinterpretCast(RenderTargetBufferInfo*, Handle);

				m_Buffer.Append(data);
			}

			void OpenGLCommandBuffer::SetViewport(const Vector2I& Position, const Vector2I& Size)
			{
				CoreDebugAssert(Categories::RenderSystem, Position >= Vector2I::Zero, "Position is invalid");
				CoreDebugAssert(Categories::RenderSystem, Size >= Vector2I::Zero, "Size is invalid");

				m_Buffer.Append(CommandTypes::SetViewport);

				SetViewportCommandData data = {};
				data.Position = Position;
				data.Size = Size;

				m_Buffer.Append(data);
			}

			void OpenGLCommandBuffer::Clear(ClearFlags Flags, const ColorUI8& Color)
			{
				m_Buffer.Append(CommandTypes::Clear);

				ClearCommandData data = {};
				data.Flags = Flags;
				data.Color = Color;

				m_Buffer.Append(data);
			}

			void OpenGLCommandBuffer::SetMesh(ResourceHandle Handle)
			{
				CoreDebugAssert(Categories::RenderSystem, Handle != 0, "Handle is invalid");

				m_Buffer.Append(CommandTypes::SetMesh);

				SetMeshCommandData data = {};
				data.Mesh = ReinterpretCast(MeshBufferInfo*, Handle);

				m_Buffer.Append(data);
			}

			void OpenGLCommandBuffer::DrawIndexed(PolygonTypes PolygonType, uint32 IndexCount)
			{
				CoreDebugAssert(Categories::RenderSystem, IndexCount != 0, "IndexCount cannot be zero");

				m_Buffer.Append(CommandTypes::DrawIndexed);

				DrawIndexedCommandData data = {};
				data.PolygonType = PolygonType;
				data.IndexCount = IndexCount;

				m_Buffer.Append(data);
			}

			void OpenGLCommandBuffer::DrawArray(PolygonTypes PolygonType, uint32 VertexCount)
			{
				CoreDebugAssert(Categories::RenderSystem, VertexCount != 0, "VertexCount cannot be zero");

				m_Buffer.Append(CommandTypes::DrawArray);

				DrawArrayCommandData data = {};
				data.PolygonType = PolygonType;
				data.VertexCount = VertexCount;

				m_Buffer.Append(data);
			}

			void OpenGLCommandBuffer::BeginEvent(cwstr Label)
			{
				m_Buffer.Append(CommandTypes::BeginEvent);

				BeginEventCommandData data = {};
				data.Length = CharacterUtility::GetLength(Label);
				CharacterUtility::ChangeType(Label, data.Label, data.Length);

				m_Buffer.Append(data);
			}

			void OpenGLCommandBuffer::EndEvent(void)
			{
				m_Buffer.Append(CommandTypes::EndEvent);

				EndEventCommandData data = {};

				m_Buffer.Append(data);
			}

			void OpenGLCommandBuffer::SetMarker(cwstr Label)
			{
				m_Buffer.Append(CommandTypes::SetMarker);

				SetMarkerCommandData data = {};
				data.Length = CharacterUtility::GetLength(Label);
				CharacterUtility::ChangeType(Label, data.Label, data.Length);

				m_Buffer.Append(data);
			}

			bool OpenGLCommandBuffer::Execute(void)
			{
				m_Buffer.ResetRead();
				CommandTypes commandType;
				while (m_Buffer.Read(commandType))
				{
					switch (commandType)
					{
					case CommandTypes::CopyBuffer:
					{
						CopyBufferCommandData data = {};
						m_Buffer.Read(data);

						ExecuteCopy(data);

					} break;

					case CommandTypes::SetProgram:
					{
						SetProgramCommandData data = {};
						m_Buffer.Read(data);

						glUseProgram(data.Program->Handle);

					} break;

					case CommandTypes::SetProgramConstantBuffer:
					{
						SetProgramConstantBufferCommandData data = {};
						m_Buffer.Read(data);

						glBindBufferBase(GetBufferType(GPUBufferTypes::Constant), data.Handle, data.Value->Handle);

					} break;

					case CommandTypes::SetProgramTexture:
					{
						SetProgramTextureCommandData data = {};
						m_Buffer.Read(data);

						glActiveTexture(GL_TEXTURE0 + data.Handle);

						glBindTexture(GetTextureType(data.Value->Type), ReinterpretCast(TextureBufferInfo*, data.Value)->Handle);

						glUniform1i(data.Handle, data.Handle);

					} break;

					case CommandTypes::SetState:
					{
						SetStateCommandData data = {};
						m_Buffer.Read(data);

#define SET_STATE_FOR_CULL(CullMode) \
						{ \
							RenderState::FaceState& state = data.State.GetFaceState(CullMode); \
							if (state.StencilTestFunction == TestFunctions::Never) \
								glDisable(GL_STENCIL_TEST); \
							else \
							{ \
								glEnable(GL_STENCIL_TEST); \
								glStencilFuncSeparate(GetCullingMode(CullMode), GetTestFunction(state.StencilTestFunction), state.StencilTestFunctionReference, state.StencilTestFunctionMask); \
							} \
							glStencilMaskSeparate(GetCullingMode(CullMode), state.StencilMask);\
							glStencilOpSeparate(GetCullingMode(CullMode), GetStencilingOperation(state.StencilOperationStencilFailed), GetStencilingOperation(state.StencilOperationDepthFailed), GetStencilingOperation(state.StencilOperationDepthPassed)); \
							glPolygonMode(GetCullingMode(CullMode), GetPolygonRenderMode(state.PolygonMode)); \
						}

						glFrontFace(GetFaceOrdering(data.State.FaceOrder));

						if (data.State.CullMode == CullModes::None)
							glDisable(GL_CULL_FACE);
						else
						{
							glEnable(GL_CULL_FACE);
							glCullFace(GetCullingMode(data.State.CullMode));
						}

						if (data.State.DepthTestFunction == TestFunctions::Never)
							glDisable(GL_DEPTH_TEST);
						else
						{
							glEnable(GL_DEPTH_TEST);
							glDepthFunc(GetTestFunction(data.State.DepthTestFunction));
						}

						SET_STATE_FOR_CULL(CullModes::None);
						SET_STATE_FOR_CULL(CullModes::Back);
						SET_STATE_FOR_CULL(CullModes::Front);
						SET_STATE_FOR_CULL(CullModes::Both);

						glBlendEquation(GetBlendingEquation(data.State.BlendEquation));

						if (data.State.BlendFunctionSourceFactor == BlendFunctions::One && data.State.BlendFunctionDestinationFactor == BlendFunctions::Zero)
							glDisable(GL_BLEND);
						else
						{
							glEnable(GL_BLEND);
							glBlendFunc(GetBlendingFunction(data.State.BlendFunctionSourceFactor), GetBlendingFunction(data.State.BlendFunctionDestinationFactor));
						}

#undef SET_STATE_FOR_CULL
					} break;

					case CommandTypes::SetRenderTarget:
					{
						SetRenderTargetCommandData data = {};
						m_Buffer.Read(data);

						ResourceHandle handle = 0;
						if (data.RenderTarget != nullptr)
							handle = data.RenderTarget->Handle;

						glBindFramebuffer(GL_FRAMEBUFFER, handle);

					} break;

					case CommandTypes::SetViewport:
					{
						SetViewportCommandData data = {};
						m_Buffer.Read(data);

						glViewport(data.Position.X, data.Position.Y, data.Size.X, data.Size.Y);

					} break;

					case CommandTypes::Clear:
					{
						ClearCommandData data = {};
						m_Buffer.Read(data);

						Vector4F col;
						Helper::GetNormalizedColor(data.Color, col);

						glClearColor(col.X, col.Y, col.Z, col.W);

						glClear(GetClearingFlags(data.Flags));

					} break;

					case CommandTypes::SetMesh:
					{
						SetMeshCommandData data = {};
						m_Buffer.Read(data);

						RenderContextInfo* currentContext = m_Device->GetCurrentContext();

						if (currentContext == nullptr)
							return false;

						CoreDebugAssert(Categories::RenderSystem, currentContext->IsActive, "Context is not active");

						ResourceHandle vao = 0;
						if (currentContext->VertexArrays.Contains(data.Mesh))
							vao = currentContext->VertexArrays[data.Mesh];
						else
						{
							if (!CreateVertexArray(*data.Mesh, vao))
								return false;

							currentContext->VertexArrays[data.Mesh] = vao;
						}

						glBindVertexArray(vao);

					} break;

					case CommandTypes::DrawIndexed:
					{
						DrawIndexedCommandData data = {};
						m_Buffer.Read(data);

						glDrawElements(GetPolygonType(data.PolygonType), data.IndexCount, GL_UNSIGNED_INT, nullptr);

					} break;

					case CommandTypes::DrawArray:
					{
						DrawArrayCommandData data = {};
						m_Buffer.Read(data);

						glDrawArrays(GetPolygonType(data.PolygonType), 0, data.VertexCount);

					} break;

					case CommandTypes::BeginEvent:
					{
						BeginEventCommandData data = {};
						m_Buffer.Read(data);

						glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, data.Length, data.Label);

					} break;

					case CommandTypes::EndEvent:
					{
						EndEventCommandData data = {};
						m_Buffer.Read(data);

						glPopDebugGroup();
					} break;

					case CommandTypes::SetMarker:
					{
						SetMarkerCommandData data = {};
						m_Buffer.Read(data);

						glInsertEventMarkerEXT(data.Length, data.Label);

					} break;

					default:
						CoreDebugAssert(Categories::RenderSystem, false, "CommandType is not recognized");
					}
				}

				return true;
			}

			void OpenGLCommandBuffer::ExecuteCopy(const CopyBufferCommandData& Data)
			{
				switch (Data.Type)
				{
				case GPUBufferTypes::Vertex:
				{
					if (Data.SourceIsABuffer)
					{
						BufferInfo* info = ReinterpretCast(BufferInfo*, Data.Source);
						MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Data.Destination);

						GPUBufferTypes type = GPUBufferTypes::Vertex;
						uint32 target = GetBufferType(type);

						byte* buffer = nullptr;
						if (!LockBufferInternal(info, type, GPUBufferAccess::ReadOnly, &buffer))
							return;

						UnlockBufferInternal(info, type);

						glBindBuffer(target, meshBufferInfo->VertexBufferObject->Handle);

						glBufferData(target, info->Size, buffer, GL_STATIC_COPY);

						glBindBuffer(target, 0);

					}
					else
					{
						MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Data.Source);
						BufferInfo* info = ReinterpretCast(BufferInfo*, Data.Destination);

						GPUBufferTypes type = GPUBufferTypes::Vertex;
						uint32 target = GetBufferType(type);

						byte* buffer = nullptr;
						if (!LockBufferInternal(meshBufferInfo->VertexBufferObject, type, GPUBufferAccess::ReadOnly, &buffer))
							return;

						UnlockBufferInternal(meshBufferInfo->VertexBufferObject, type);

						glBindBuffer(target, info->Handle);

						glBufferData(target, info->Size, buffer, GL_STATIC_COPY);

						glBindBuffer(target, 0);
					}
				} break;

				case GPUBufferTypes::Index:
				{
					if (Data.SourceIsABuffer)
					{
						BufferInfo* info = ReinterpretCast(BufferInfo*, Data.Source);
						MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Data.Destination);

						GPUBufferTypes type = GPUBufferTypes::Index;
						uint32 target = GetBufferType(type);

						byte* buffer = nullptr;
						if (!LockBufferInternal(info, type, GPUBufferAccess::ReadOnly, &buffer))
							return;

						UnlockBufferInternal(info, type);

						glBindBuffer(target, meshBufferInfo->IndexBufferObject->Handle);

						glBufferData(target, info->Size, buffer, GL_STATIC_COPY);

						glBindBuffer(target, 0);
					}
					else
					{
						MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Data.Source);
						BufferInfo* info = ReinterpretCast(BufferInfo*, Data.Destination);

						GPUBufferTypes type = GPUBufferTypes::Vertex;
						uint32 target = GetBufferType(type);

						byte* buffer = nullptr;
						if (!LockBufferInternal(meshBufferInfo->IndexBufferObject, type, GPUBufferAccess::ReadOnly, &buffer))
							return;

						UnlockBufferInternal(meshBufferInfo->IndexBufferObject, type);

						glBindBuffer(target, info->Handle);

						glBufferData(target, info->Size, buffer, GL_STATIC_COPY);

						glBindBuffer(target, 0);
					}
				} break;

				case GPUBufferTypes::Pixel:
				{
					if (Data.SourceIsABuffer)
					{
						BufferInfo* info = ReinterpretCast(BufferInfo*, Data.Source);

						uint32 target = GetBufferType(GPUBufferTypes::Pixel);

						TextureBufferInfo* texInfo = ReinterpretCast(TextureBufferInfo*, Data.Destination);
						glBindTexture(GetTextureType(texInfo->Type), texInfo->Handle);

						glBindBuffer(target, info->Handle);

						glTexSubImage2D(GetTextureType(texInfo->Type), 0, 0, 0, texInfo->Width, texInfo->Height, GetTextureFormat(texInfo->Format), GetTexturePixelType(texInfo->Format), 0);

						glBindBuffer(target, 0);

						glBindTexture(GetTextureType(texInfo->Type), 0);
					}
					else
					{
						TextureBufferInfo* info = ReinterpretCast(TextureBufferInfo*, Data.Source);
						BufferInfo* texInfo = ReinterpretCast(BufferInfo*, Data.Destination);

						glBindBuffer(GL_PIXEL_PACK_BUFFER, info->Handle);

						glBufferData(GL_PIXEL_PACK_BUFFER, info->Size, nullptr, GL_STATIC_COPY);

						glActiveTexture(GL_TEXTURE0);

						bool result = true;

						glBindTexture(GetTextureType(info->Type), texInfo->Handle);

						glGetTexImage(GetTextureType(info->Type), 0, GetTextureFormat(info->Format), GetTexturePixelType(info->Format), nullptr);

						glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
					}
				} break;

				default:
					CoreDebugAssert(Categories::RenderSystem, false, "Type is not recognized");
				}
			}

			//bool OpenGLDevice::GenerateTextureMipMap(ResourceHandle Handle, TextureTypes Type)
			//{
			//	if (Handle == 0)
			//		return false;

			//	TextureBufferInfo* info = ReinterpretCast(TextureBufferInfo*, Handle);

			//	glBindTexture(GetTextureType(Type), info->Handle);

			//	glGenerateMipmap(GetTextureType(Type));

			//	return true;
			//}
		}
	}
}