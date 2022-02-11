// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DirectX12RenderDevice\Private\DirectX12CommandBuffer.h>
#include <DirectX12RenderDevice\Private\DirectX12DebugInfo.h>
#include <RenderCommon\Helper.h>
#include <pix.h>

namespace Engine
{
	namespace DirectX12RenderDevice
	{
		namespace Private
		{
#define FILL_RENDER_VIEWS_USING_CONTEXT() \
			{ \
				auto currentContex = m_Device->GetCurrentContext(); \
				if (currentContex != nullptr && currentContex->Initialized) \
				{ \
					m_CurrentRenderTargetViews[0] = &currentContex->IntermediateViews[RenderContextInfo::RENDER_TARGET_VIEW_INDEX]; \
					m_CurrentRenderTargetViewCount = 1; \
					m_CurrentDepthStencilView = &currentContex->IntermediateViews[RenderContextInfo::DEPTH_STENCIL_VIEW_INDEX]; \
				} \
			}

#ifdef DEBUG_MODE
#define ADD_TRANSITION_STATE(Info, AfterState) \
			{ \
				DirectX12Wrapper::Command::AddResourceStateAssertion(m_DebugList, Info->Resource.Resource, Info->State); \
				if (Info->State != AfterState) \
				{ \
					DirectX12Wrapper::Command::AddTransitionResourceBarrier(m_List, Info->Resource.Resource, Info->State, AfterState); \
					Info->State = AfterState; \
				} \
				DirectX12Wrapper::Command::AddResourceStateAssertion(m_DebugList, Info->Resource.Resource, AfterState); \
			}
#else
#define ADD_TRANSITION_STATE(ResourceInfo, AfterState) \
			if (Info->State != AfterState) \
			{ \
				DirectX12Wrapper::Command::AddTransitionResourceBarrier(m_List, ResourceInfo->Resource.Resource, ResourceInfo->State, AfterState); \
				ResourceInfo->State = AfterState; \
			}
#endif

#define ADD_TRANSITION_STATE_FOR_TARGET_BUFFERS(RenderTargetState, DepthStencilState) \
			{ \
				for (uint8 i = 0; i < m_CurrentRenderTargetViewCount; ++i) \
					ADD_TRANSITION_STATE(m_CurrentRenderTargetViews[i], RenderTargetState); \
				if (m_CurrentDepthStencilView != nullptr) \
					ADD_TRANSITION_STATE(m_CurrentDepthStencilView, DepthStencilState); \
			}

			D3D12_CULL_MODE GetCullMode(CullModes CullMode)
			{
				switch (CullMode)
				{
				case CullModes::None:
					return D3D12_CULL_MODE_NONE;

				case CullModes::Front:
					return D3D12_CULL_MODE_FRONT;

				case CullModes::Back:
					return D3D12_CULL_MODE_BACK;
				}
			}

			D3D12_PRIMITIVE_TOPOLOGY GetPolygonTopology(PolygonTypes PolygonType)
			{
				switch (PolygonType)
				{
				case PolygonTypes::Lines:
					return D3D_PRIMITIVE_TOPOLOGY_LINELIST;

				case PolygonTypes::LineLoop:
					return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ;

				case PolygonTypes::LineStrip:
					return D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

				case PolygonTypes::Triangles:
					return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

				case PolygonTypes::TriangleStrip:
					return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

				case PolygonTypes::TriangleFan:
					return D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ;
				}

				return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
			}

			D3D12_BLEND_OP GetBlendEquation(BlendEquations BlendEquation)
			{
				switch (BlendEquation)
				{
				case BlendEquations::Add:
					return D3D12_BLEND_OP_ADD;

				case BlendEquations::Subtract:
					return D3D12_BLEND_OP_SUBTRACT;

				case BlendEquations::ReverseSubtract:
					return D3D12_BLEND_OP_REV_SUBTRACT;

				case BlendEquations::Min:
					return D3D12_BLEND_OP_MIN;

				case BlendEquations::Max:
					return D3D12_BLEND_OP_MAX;
				}
			}

			D3D12_BLEND GetBlendFunction(BlendFunctions BlendFunction)
			{
				switch (BlendFunction)
				{
				case BlendFunctions::Zero:
					return D3D12_BLEND_ZERO;

				case BlendFunctions::One:
					return D3D12_BLEND_ONE;

				case BlendFunctions::SourceColor:
					return D3D12_BLEND_SRC_COLOR;

				case BlendFunctions::OneMinusSourceColor:
					return D3D12_BLEND_INV_SRC_COLOR;

				case BlendFunctions::DestinationColor:
					return D3D12_BLEND_DEST_COLOR;

				case BlendFunctions::OneMinusDestinationColor:
					return D3D12_BLEND_INV_DEST_COLOR;

				case BlendFunctions::SourceAlpha:
					return D3D12_BLEND_SRC_ALPHA;

				case BlendFunctions::OneMinusSourceAlpha:
					return D3D12_BLEND_INV_SRC_ALPHA;

				case BlendFunctions::DestinationAlpha:
					return D3D12_BLEND_DEST_ALPHA;

				case BlendFunctions::OneMinusDestinationAlpha:
					return D3D12_BLEND_INV_DEST_ALPHA;

				case BlendFunctions::ConstantColor:
					return D3D12_BLEND_SRC1_COLOR;

				case BlendFunctions::OneMinusConstantColor:
					return D3D12_BLEND_INV_SRC1_COLOR;

				case BlendFunctions::ConstantAlpha:
					return D3D12_BLEND_SRC1_ALPHA;

				case BlendFunctions::OneMinusConstantAlpha:
					return D3D12_BLEND_INV_SRC1_ALPHA;
				}
			}

			D3D12_FILL_MODE GetFillMode(PolygonModes PolygonMode)
			{
				switch (PolygonMode)
				{
				case PolygonModes::Point:
					return D3D12_FILL_MODE_WIREFRAME;

				case PolygonModes::Line:
					return D3D12_FILL_MODE_WIREFRAME;

				case PolygonModes::Fill:
					return D3D12_FILL_MODE_SOLID;
				}
			}

			D3D12_STENCIL_OP GetStencilOperation(StencilOperations StencilOperation)
			{
				switch (StencilOperation)
				{
				case StencilOperations::Keep:
					return D3D12_STENCIL_OP_KEEP;

				case StencilOperations::Zero:
					return D3D12_STENCIL_OP_ZERO;

				case StencilOperations::Replace:
					return D3D12_STENCIL_OP_REPLACE;

				case StencilOperations::Increament:
					return D3D12_STENCIL_OP_INCR;

				case StencilOperations::IncreamentWrap:
					return D3D12_STENCIL_OP_INCR_SAT;

				case StencilOperations::Decreament:
					return D3D12_STENCIL_OP_DECR;

				case StencilOperations::DecreamentWrap:
					return D3D12_STENCIL_OP_DECR_SAT;

				case StencilOperations::Invert:
					return D3D12_STENCIL_OP_INVERT;
				}
			}

			DirectX12CommandBuffer::DirectX12CommandBuffer(DirectX12Device* Device, Types Type) :
				m_Device(Device),
				m_NativeDevice(Device->GetDevice()),
				m_Type(Type),
				m_Queue(nullptr),
				m_Allocator(nullptr),
				m_List(nullptr),
				m_Fence(nullptr),
				m_FenceValue(0),
				m_FenceEvent(0),
				m_InputLayout(Device->GetInputLayout()),
				m_InputLayoutCount(Device->GetInputLayoutCount()),
				m_CurrentRenderTargetViewCount(0),
				m_CurrentDepthStencilView(nullptr),
				m_CurrentDescriptorHeapCount(0)

#ifdef DEBUG_MODE
				, m_DebugList(nullptr)
#endif
			{
				D3D12_COMMAND_LIST_TYPE type;

				switch (m_Type)
				{
				case ICommandBuffer::Types::Graphics:
					type = D3D12_COMMAND_LIST_TYPE_DIRECT;
					break;

				case ICommandBuffer::Types::Compute:
					type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
					break;

				case ICommandBuffer::Types::Copy:
					type = D3D12_COMMAND_LIST_TYPE_COPY;
					break;

				default:
					CoreDebugAssert(Categories::RenderSystem, false, "Unhandled command buffer type");
				}

				if (!CHECK_CALL(DirectX12Wrapper::Command::CreateCommandQueue(m_NativeDevice, type, &m_Queue)))
					return;

				if (!CHECK_CALL(DirectX12Wrapper::Command::CreateCommandAllocator(m_NativeDevice, type, &m_Allocator)))
					return;

				if (!CHECK_CALL(DirectX12Wrapper::Command::CreateCommandList(m_NativeDevice, m_Allocator, type, &m_List)))
					return;

#ifdef DEBUG_MODE
				if (!CHECK_CALL(DirectX12Wrapper::Debugging::GetDebugCommandList(m_List, &m_DebugList)))
					return;
#endif

				if (!CHECK_CALL(DirectX12Wrapper::Fence::Create(m_NativeDevice, &m_Fence)))
					return;

				m_FenceValue = 1;
				m_FenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			}

			DirectX12CommandBuffer::~DirectX12CommandBuffer(void)
			{
				CloseHandle(m_FenceEvent);

				if (!CHECK_CALL(DirectX12Wrapper::DestroyInstance(m_Fence)))
					return;

#ifdef DEBUG_MODE
				if (!CHECK_CALL(DirectX12Wrapper::DestroyInstance(m_DebugList)))
					return;
#endif

				if (!CHECK_CALL(DirectX12Wrapper::DestroyInstance(m_List)))
					return;

				if (!CHECK_CALL(DirectX12Wrapper::DestroyInstance(m_Allocator)))
					return;

				if (!CHECK_CALL(DirectX12Wrapper::DestroyInstance(m_Queue)))
					return;
			}

			void DirectX12CommandBuffer::SetName(cwstr Name)
			{
				if (!CHECK_CALL(DirectX12Wrapper::Debugging::SetObjectName(m_Queue, Name)))
					return;

				if (!CHECK_CALL(DirectX12Wrapper::Debugging::SetObjectName(m_Allocator, Name)))
					return;

				if (!CHECK_CALL(DirectX12Wrapper::Debugging::SetObjectName(m_List, Name)))
					return;
			}

			void DirectX12CommandBuffer::Clear(void)
			{
				if (!CHECK_CALL(DirectX12Wrapper::Command::ResetCommandAllocator(m_Allocator)))
					return;

				CHECK_CALL(DirectX12Wrapper::Command::ResetCommandList(m_List, m_Allocator));
			}

			void DirectX12CommandBuffer::CopyBuffer(ResourceHandle SourceHandle, ResourceHandle DestinationHandle)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Copy, "Command buffer type is not Copy");
				CoreDebugAssert(Categories::RenderSystem, SourceHandle != 0, "SourceHandle is invalid");
				CoreDebugAssert(Categories::RenderSystem, DestinationHandle != 0, "DestinationHandle is invalid");

				BufferInfo* sourceInfo = ReinterpretCast(BufferInfo*, SourceHandle);
				BufferInfo* destInfo = ReinterpretCast(BufferInfo*, DestinationHandle);

				GPUBufferTypes type = sourceInfo->Type;

				if (sourceInfo->Type == GPUBufferTypes::Vertex)
				{
					if (destInfo->Type == GPUBufferTypes::Index)
						type = GPUBufferTypes::Index;
				}

				//D3D12_RESOURCE_STATES sourceState = sourceInfo->State;
				//D3D12_RESOURCE_STATES destinationState = destInfo->State;

				ADD_TRANSITION_STATE(sourceInfo, D3D12_RESOURCE_STATE_COPY_SOURCE);
				ADD_TRANSITION_STATE(destInfo, D3D12_RESOURCE_STATE_COPY_DEST);

				if (type == GPUBufferTypes::Constant || type == GPUBufferTypes::Vertex || type == GPUBufferTypes::Index)
				{
					DirectX12Wrapper::Command::AddCopyBufferCommand(m_List, sourceInfo->Resource.Resource, destInfo->Resource.Resource, destInfo->Size);
				}
				else if (type == GPUBufferTypes::Pixel)
				{
					if (sourceInfo->IsIntermediate && !destInfo->IsIntermediate)
						DirectX12Wrapper::Command::AddCopyBufferToTextureCommand(m_List, sourceInfo->Resource.Resource, destInfo->Resource.Resource);
					else if (!sourceInfo->IsIntermediate && destInfo->IsIntermediate)
						DirectX12Wrapper::Command::AddCopyTextureToBufferCommand(m_List, sourceInfo->Resource.Resource, destInfo->Resource.Resource);
				}

				//ADD_TRANSITION_STATE(sourceInfo, sourceState);
				//ADD_TRANSITION_STATE(destInfo, destinationState);
			}

			void DirectX12CommandBuffer::CopyTexture(ResourceHandle SourceHandle, const Vector2I& SourcePosition, ResourceHandle DestinationHandle, const Vector2I& DestinationPosition, const Vector2I& Size)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Copy, "Command buffer type is not Copy");
				CoreDebugAssert(Categories::RenderSystem, SourceHandle != 0, "SourceHandle is invalid");
				CoreDebugAssert(Categories::RenderSystem, SourcePosition >= Vector2I::Zero, "SourcePosition is invalid");
				CoreDebugAssert(Categories::RenderSystem, DestinationHandle != 0, "DestinationHandle is invalid");
				CoreDebugAssert(Categories::RenderSystem, DestinationPosition >= Vector2I::Zero, "SourcePosition is invalid");
				CoreDebugAssert(Categories::RenderSystem, Size > Vector2I::Zero, "Size is invalid");

				TextureResourceInfo* sourceInfo = ReinterpretCast(TextureResourceInfo*, SourceHandle);
				TextureResourceInfo* destInfo = ReinterpretCast(TextureResourceInfo*, DestinationHandle);

				CoreDebugAssert(Categories::RenderSystem, sourceInfo->Format != destInfo->Format, "Texture formats are not the same");
				CoreDebugAssert(Categories::RenderSystem, SourcePosition + Size <= Vector2I(sourceInfo->Width, sourceInfo->Height), "SourcePosition+Size is invalid");
				CoreDebugAssert(Categories::RenderSystem, DestinationPosition + Size <= Vector2I(destInfo->Width, destInfo->Height), "DestinationPosition+Size is invalid");

				//D3D12_RESOURCE_STATES sourceState = sourceInfo->State;
				//D3D12_RESOURCE_STATES destinationState = destInfo->State;

				ADD_TRANSITION_STATE(sourceInfo, D3D12_RESOURCE_STATE_COPY_SOURCE);
				ADD_TRANSITION_STATE(destInfo, D3D12_RESOURCE_STATE_COPY_DEST);

				DirectX12Wrapper::Command::AddCopyTextureCommand(m_List, sourceInfo->Resource.Resource, SourcePosition.X, SourcePosition.Y, destInfo->Resource.Resource, DestinationPosition.X, DestinationPosition.Y, Size.X, Size.Y);

				//ADD_TRANSITION_STATE(sourceInfo, sourceState);
				//ADD_TRANSITION_STATE(destInfo, destinationState);
			}

			void DirectX12CommandBuffer::GenerateMipMap(ResourceHandle Handle)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Compute, "Command buffer type is not Compute");
				CoreDebugAssert(Categories::RenderSystem, Handle != 0, "Handle is invalid");

			}

			void DirectX12CommandBuffer::SetRenderTarget(ResourceHandle Handle)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Graphics, "Command buffer type is not Graphics");

				m_CurrentRenderTargetViewCount = 0;
				m_CurrentDepthStencilView = nullptr;

				if (Handle == 0)
				{
					FILL_RENDER_VIEWS_USING_CONTEXT();
				}
				else
				{
					RenderTargetInfos* renderTarget = ReinterpretCast(RenderTargetInfos*, Handle);

					for (auto& viewInfo : renderTarget->Views)
					{
						uint8 colorPointIndex = (uint8)viewInfo.Point - (uint8)AttachmentPoints::Color0;

						if (Helper::IsColorPoint(viewInfo.Point))
						{
							m_CurrentRenderTargetViews[colorPointIndex] = &viewInfo;
							++m_CurrentRenderTargetViewCount;
						}
						else
							m_CurrentDepthStencilView = &viewInfo;
					}
				}

				ADD_TRANSITION_STATE_FOR_TARGET_BUFFERS(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_DEPTH_WRITE);

				D3D12_CPU_DESCRIPTOR_HANDLE renderTargetHandles[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT];
				for (uint8 i = 0; i < m_CurrentRenderTargetViewCount; ++i)
					renderTargetHandles[i] = m_CurrentRenderTargetViews[i]->TargetView.CPUHandle;

				if (m_CurrentRenderTargetViewCount != 0 || m_CurrentDepthStencilView != nullptr)
					DirectX12Wrapper::Command::AddSetTargets(m_List, renderTargetHandles, m_CurrentRenderTargetViewCount, (m_CurrentDepthStencilView == nullptr ? nullptr : &m_CurrentDepthStencilView->TargetView.CPUHandle));
			}

			void DirectX12CommandBuffer::SetViewport(const Vector2I& Position, const Vector2I& Size)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Graphics, "Command buffer type is not Graphics");
				CoreDebugAssert(Categories::RenderSystem, Position >= Vector2I::Zero, "Position is invalid");
				CoreDebugAssert(Categories::RenderSystem, Size >= Vector2I::Zero, "Size is invalid");

				D3D12_VIEWPORT viewport;
				viewport.TopLeftX = Position.X;
				viewport.TopLeftY = Position.Y;
				viewport.Width = Size.X;
				viewport.Height = Size.Y;
				viewport.MinDepth = 0;
				viewport.MaxDepth = 1;

				DirectX12Wrapper::Command::AddSetViewportCommand(m_List, &viewport);
			}

			void DirectX12CommandBuffer::Clear(ClearFlags Flags, const ColorUI8& Color)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Graphics, "Command buffer type is not Graphics");

				if (BitwiseUtils::IsEnabled(Flags, ClearFlags::ColorBuffer))
				{
					Vector4F color;
					Helper::GetNormalizedColor(Color, color);

					for (uint8 i = 0; i < m_CurrentRenderTargetViewCount; ++i)
						DirectX12Wrapper::Command::AddClearRenderTargetCommand(m_List, m_CurrentRenderTargetViews[i]->TargetView.CPUHandle, &color.X);
				}

				bool shouldClearDepth = BitwiseUtils::IsEnabled(Flags, ClearFlags::DepthBuffer);
				bool shouldClearStencil = BitwiseUtils::IsEnabled(Flags, ClearFlags::StencilBuffer);
				if (m_CurrentDepthStencilView != nullptr && (shouldClearDepth || shouldClearStencil))
				{
					D3D12_CLEAR_FLAGS flags = (D3D12_CLEAR_FLAGS)0;
					if (shouldClearDepth) flags |= D3D12_CLEAR_FLAG_DEPTH;
					if (shouldClearStencil) flags |= D3D12_CLEAR_FLAG_STENCIL;

					DirectX12Wrapper::Command::AddClearDepthStencilCommand(m_List, m_CurrentDepthStencilView->TargetView.CPUHandle, flags, 1, 1);
				}
			}

			void DirectX12CommandBuffer::SetState(const RenderState& State)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Graphics, "Command buffer type is not Graphics");

				PlatformMemory::Copy(&State, &m_State, 1);
			}

			void DirectX12CommandBuffer::SetProgram(ResourceHandle Handle)
			{
#define IMPLEMENT_SET_SHADER_DATA(StageName) desc.StageName = { programInfos->StageName.Buffer, programInfos->StageName.Size }

				CoreDebugAssert(Categories::RenderSystem, m_Type != Types::Copy, "Command buffer type is not Graphics/Compute");
				CoreDebugAssert(Categories::RenderSystem, Handle != 0, "Program is invalid");

				m_CurrentDescriptorHeapCount = 0;

				ProgramInfos* programInfos = ReinterpretCast(ProgramInfos*, Handle);
				bool isGraphics = (programInfos->ComputeShader.Size == 0);
				CoreDebugAssert(Categories::RenderSystem, (isGraphics && m_Type == Types::Graphics) || (!isGraphics && m_Type == Types::Compute), "Program is not compatible with buffer type");

				uint32 stateHash = 0;
				if (isGraphics)
				{
					stateHash = Helper::GetRenderStateHash(m_State);

					stateHash += (m_CurrentDepthStencilView == nullptr ? 0 : m_CurrentDepthStencilView->Format);

					for (uint8 i = 0; i < m_CurrentRenderTargetViewCount; ++i)
						stateHash += m_CurrentRenderTargetViews[i]->Format;
				}

				ID3D12PipelineState* pipelineState = nullptr;

				if (programInfos->Pipelines.Contains(stateHash))
					pipelineState = programInfos->Pipelines[stateHash];
				else
				{
					if (isGraphics)
					{
						DirectX12Wrapper::PipelineStateObject::GraphicsPipelineStateDesc desc = {};
						IMPLEMENT_SET_SHADER_DATA(VertexShader);
						IMPLEMENT_SET_SHADER_DATA(TessellationShader);
						IMPLEMENT_SET_SHADER_DATA(GeometryShader);
						IMPLEMENT_SET_SHADER_DATA(FragmentShader);

						FillGraphicsPipelineState(desc);

						CHECK_CALL(DirectX12Wrapper::PipelineStateObject::Create(m_NativeDevice, &desc, &pipelineState));
					}
					else
					{
						DirectX12Wrapper::PipelineStateObject::ComputePipelineStateDesc desc = {};
						IMPLEMENT_SET_SHADER_DATA(ComputeShader);

						CHECK_CALL(DirectX12Wrapper::PipelineStateObject::Create(m_NativeDevice, &desc, &pipelineState));
					}

					if (!CHECK_CALL(DirectX12Wrapper::Debugging::SetObjectName(pipelineState, programInfos->PipelineName.GetValue())))
						return;

					programInfos->Pipelines[stateHash] = pipelineState;
				}

				if (pipelineState == nullptr)
					return;

				if (isGraphics)
					DirectX12Wrapper::Command::AddSetGraphicsRootSignature(m_List, programInfos->RootSignature);
				else
					DirectX12Wrapper::Command::AddSetComputeRootSignature(m_List, programInfos->RootSignature);

				DirectX12Wrapper::Command::AddSetPipelineState(m_List, pipelineState);

#undef IMPLEMENT_SET_SHADER_DATA
			}

			void DirectX12CommandBuffer::SetProgramConstantBuffer(ProgramConstantHandle Handle, ResourceHandle Value)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type != Types::Copy, "Command buffer type is not Graphics/Compute");

				if (Value == 0)
					return;

				BufferInfo* bufferInfo = ReinterpretCast(BufferInfo*, Value);

				DirectX12Wrapper::Command::AddSetGraphicsConstantBuffer(m_List, Handle, bufferInfo->Resource.Resource->GetGPUVirtualAddress());
			}

			void DirectX12CommandBuffer::SetProgramTexture(ProgramConstantHandle Handle, ResourceHandle Value)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type != Types::Copy, "Command buffer type is not Graphics/Compute");

				if (Value == 0)
					return;

				TextureResourceInfo* resourceInfo = ReinterpretCast(TextureResourceInfo*, Value);

				ADD_TRANSITION_STATE(resourceInfo, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

				bool found = false;
				for (uint8 i = 0; i < m_CurrentDescriptorHeapCount; ++i)
				{
					if (m_CurrentDescriptorHeaps[i] != resourceInfo->View.DescriptorHeap)
						continue;

					found = true;
					break;
				}

				if (!found)
				{
					CoreDebugAssert(Categories::RenderSystem, m_CurrentDescriptorHeapCount <= MAX_DESCRIPTOR_HEAP_COUNT, "Exceeding MAX_DESCRIPTOR_HEAP_COUNT is invalid");

					m_CurrentDescriptorHeaps[m_CurrentDescriptorHeapCount++] = resourceInfo->View.DescriptorHeap;
					m_CurrentDescriptorHeaps[m_CurrentDescriptorHeapCount++] = resourceInfo->SamplerView.DescriptorHeap;

					DirectX12Wrapper::Command::AddSetDescriptorHeap(m_List, m_CurrentDescriptorHeaps, m_CurrentDescriptorHeapCount);
				}

				DirectX12Wrapper::Command::AddSetGraphicsRootDescriptorTable(m_List, Handle + 1, resourceInfo->SamplerView.GPUHandle);

				DirectX12Wrapper::Command::AddSetGraphicsRootDescriptorTable(m_List, Handle, resourceInfo->View.GPUHandle);
			}

			void DirectX12CommandBuffer::SetMesh(ResourceHandle Handle)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Graphics, "Command buffer type is not Graphics");
				CoreDebugAssert(Categories::RenderSystem, Handle != 0, "Handle is invalid");

				DirectX12Wrapper::Command::AddSetPrimitiveTopologyCommand(m_List, GetPolygonTopology(PolygonTypes::Triangles));

				MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Handle);

				BufferInfo& vertextBufferInfo = *meshBufferInfo;
				DirectX12Wrapper::Command::AddSetVertexBufferCommand(m_List, vertextBufferInfo.Resource.Resource, vertextBufferInfo.Size, vertextBufferInfo.Stride);

				BufferInfo& indextBufferInfo = meshBufferInfo->IndexBuffer;
				if (indextBufferInfo.Resource.Resource != nullptr)
					DirectX12Wrapper::Command::AddSetIndexBufferCommand(m_List, indextBufferInfo.Resource.Resource, indextBufferInfo.Size);
			}

			void DirectX12CommandBuffer::DrawIndexed(PolygonTypes PolygonType, uint32 IndexCount)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Graphics, "Command buffer type is not Graphics");
				CoreDebugAssert(Categories::RenderSystem, IndexCount != 0, "IndexCount cannot be zero");

				DirectX12Wrapper::Command::AddSetPrimitiveTopologyCommand(m_List, GetPolygonTopology(PolygonType));

				DirectX12Wrapper::Command::AddDrawIndexedCommand(m_List, IndexCount);
			}

			void DirectX12CommandBuffer::DrawArray(PolygonTypes PolygonType, uint32 VertexCount)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Graphics, "Command buffer type is not Graphics");
				CoreDebugAssert(Categories::RenderSystem, VertexCount != 0, "VertexCount cannot be zero");

				DirectX12Wrapper::Command::AddSetPrimitiveTopologyCommand(m_List, GetPolygonTopology(PolygonType));

				DirectX12Wrapper::Command::AddDrawCommand(m_List, VertexCount);
			}

			void DirectX12CommandBuffer::BeginEvent(cwstr Label)
			{
				PIXBeginEvent(m_List, 0, Label);
			}

			void DirectX12CommandBuffer::EndEvent(void)
			{
				PIXEndEvent(m_List);
			}

			void DirectX12CommandBuffer::SetMarker(cwstr Label)
			{
				PIXSetMarker(m_List, 0, Label);
			}

			bool DirectX12CommandBuffer::Execute(void)
			{
				if (!CHECK_CALL(DirectX12Wrapper::Command::CloseCommandList(m_List)))
					return false;

				//RENDERING
				//if (Set.SkipFrameCount != 0)
				//	--Set.SkipFrameCount;
				//else
				{
					DirectX12Wrapper::Command::ExecuteCommandList(m_Queue, m_List);
				}

				return CHECK_CALL(DirectX12Wrapper::Fence::SignalAndWait(m_Queue, m_Fence, m_FenceEvent, m_FenceValue));
			}

			void DirectX12CommandBuffer::FillGraphicsPipelineState(DirectX12Wrapper::PipelineStateObject::GraphicsPipelineStateDesc& Desc)
			{
				auto FillDepthStencilOperation = [](const RenderState::FaceState& State, D3D12_DEPTH_STENCILOP_DESC& Desc)
				{
					Desc.StencilFailOp = GetStencilOperation(State.StencilOperationStencilFailed);
					Desc.StencilDepthFailOp = GetStencilOperation(State.StencilOperationDepthFailed);
					Desc.StencilPassOp = GetStencilOperation(State.StencilOperationDepthPassed);
					Desc.StencilFunc = GetComparisonFunction(State.StencilTestFunction);
				};

				D3D12_RASTERIZER_DESC rasterizerDesc = {};
				{
					rasterizerDesc.FrontCounterClockwise = (m_State.FaceOrder == FaceOrders::Clockwise ? false : true);
					rasterizerDesc.CullMode = GetCullMode(m_State.CullMode);
					rasterizerDesc.FillMode = GetFillMode(m_State.GetFaceState(m_State.CullMode).PolygonMode);
					rasterizerDesc.DepthClipEnable = (m_State.DepthTestFunction != TestFunctions::Never);

					Desc.RasterizerState = rasterizerDesc;
				}

				D3D12_DEPTH_STENCIL_DESC1 depthStencilDesc = {};
				{
					depthStencilDesc.DepthEnable = (m_State.DepthTestFunction != TestFunctions::Never);
					depthStencilDesc.DepthWriteMask = (m_State.DepthTestFunction == TestFunctions::Never ? D3D12_DEPTH_WRITE_MASK_ZERO : D3D12_DEPTH_WRITE_MASK_ALL);
					depthStencilDesc.DepthFunc = GetComparisonFunction(m_State.DepthTestFunction);

					FillDepthStencilOperation(m_State.FrontFaceState, depthStencilDesc.FrontFace);
					FillDepthStencilOperation(m_State.BackFaceState, depthStencilDesc.BackFace);

					depthStencilDesc.StencilWriteMask = m_State.FrontFaceState.StencilTestFunctionMask;

					depthStencilDesc.StencilReadMask = m_State.GetFaceState(m_State.CullMode).StencilMask;

					Desc.DepthStencil = depthStencilDesc;
				}

				D3D12_BLEND_DESC blendDesc = {};
				{
					for (uint8 i = 0; i < m_CurrentRenderTargetViewCount; ++i)
					{
						D3D12_RENDER_TARGET_BLEND_DESC& desc = blendDesc.RenderTarget[i];

						desc.BlendEnable = !(m_State.BlendFunctionSourceFactor == BlendFunctions::One && m_State.BlendFunctionDestinationFactor == BlendFunctions::Zero);
						desc.BlendOp = desc.BlendOpAlpha = GetBlendEquation(m_State.BlendEquation);
						desc.SrcBlend = desc.SrcBlendAlpha = GetBlendFunction(m_State.BlendFunctionSourceFactor);
						desc.DestBlend = desc.DestBlendAlpha = GetBlendFunction(m_State.BlendFunctionDestinationFactor);
						desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
					}

					Desc.BlendState = blendDesc;
				}

				D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
				{
					inputLayoutDesc.NumElements = m_InputLayoutCount;
					inputLayoutDesc.pInputElementDescs = m_InputLayout;

					Desc.InputLayout = inputLayoutDesc;
				}

				Desc.PrimitiveToplogy = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

				if (m_CurrentDepthStencilView != nullptr)
					Desc.DepthStencilFormat = m_CurrentDepthStencilView->Format;

				D3D12_RT_FORMAT_ARRAY rtvFormats = {};
				{
					rtvFormats.NumRenderTargets = m_CurrentRenderTargetViewCount;

					for (uint8 i = 0; i < m_CurrentRenderTargetViewCount; ++i)
						rtvFormats.RTFormats[i] = m_CurrentRenderTargetViews[i]->Format;

					Desc.RenderTargetFormats = rtvFormats;
				}
			}

#undef FILL_RENDER_VIEWS_USING_CONTEXT
#undef ADD_TRANSITION_STATE
#undef ADD_TRANSITION_STATE_FOR_TARGET_BUFFERS
		}
	}
}