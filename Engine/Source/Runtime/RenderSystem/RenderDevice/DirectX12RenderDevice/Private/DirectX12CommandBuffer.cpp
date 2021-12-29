// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DirectX12RenderDevice\Private\DirectX12CommandBuffer.h>
#include <DirectX12RenderDevice\Private\DirectX12DebugInfo.h>
#include <DirectX12RenderDevice\Private\DirectX12Common.h>
#include <RenderCommon\Helper.h>
#include <pix.h>

namespace Engine
{
	namespace DirectX12RenderDevice
	{
		namespace Private
		{
			void FillGraphicsPipelineState(const RenderState& State, DirectX12Wrapper::PipelineStateObject::GraphicsPipelineStateDesc& Desc)
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
					rasterizerDesc.FrontCounterClockwise = (State.FaceOrder == FaceOrders::Clockwise ? false : true);
					rasterizerDesc.CullMode = GetCullMode(State.CullMode);
					rasterizerDesc.FillMode = GetFillMode(State.GetFaceState(State.CullMode).PolygonMode);
					rasterizerDesc.DepthClipEnable = (State.DepthTestFunction != TestFunctions::Never);

					Desc.RasterizerState = rasterizerDesc;
				}

				D3D12_DEPTH_STENCIL_DESC1 depthStencilDesc = {};
				{
					depthStencilDesc.DepthEnable = (State.DepthTestFunction != TestFunctions::Never);
					depthStencilDesc.DepthWriteMask = (State.DepthTestFunction == TestFunctions::Never ? D3D12_DEPTH_WRITE_MASK_ZERO : D3D12_DEPTH_WRITE_MASK_ALL);
					depthStencilDesc.DepthFunc = GetComparisonFunction(State.DepthTestFunction);

					FillDepthStencilOperation(State.FrontFaceState, depthStencilDesc.FrontFace);
					FillDepthStencilOperation(State.BackFaceState, depthStencilDesc.BackFace);

					depthStencilDesc.StencilWriteMask = State.FrontFaceState.StencilTestFunctionMask;

					depthStencilDesc.StencilReadMask = State.GetFaceState(State.CullMode).StencilMask;

					Desc.DepthStencil = depthStencilDesc;
				}

				D3D12_BLEND_DESC blendDesc = {};
				{
					for (uint8 i = 0; i < m_CurrentRenderTargetViewCount; ++i)
					{
						D3D12_RENDER_TARGET_BLEND_DESC& desc = blendDesc.RenderTarget[i];

						desc.BlendEnable = !(State.BlendFunctionSourceFactor == BlendFunctions::One && State.BlendFunctionDestinationFactor == BlendFunctions::Zero);
						desc.BlendOp = desc.BlendOpAlpha = GetBlendEquation(State.BlendEquation);
						desc.SrcBlend = desc.SrcBlendAlpha = GetBlendFunction(State.BlendFunctionSourceFactor);
						desc.DestBlend = desc.DestBlendAlpha = GetBlendFunction(State.BlendFunctionDestinationFactor);
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

			DirectX12CommandBuffer::DirectX12CommandBuffer(ID3D12Device5* Device, Types Type) :
				m_Device(Device),
				m_Type(Type)
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

				if (!CHECK_CALL(DirectX12Wrapper::Command::CreateCommandQueue(m_Device, type, &m_Queue)))
					return;

				if (!CHECK_CALL(DirectX12Wrapper::Command::CreateCommandAllocator(m_Device, type, &m_Allocator)))
					return;

				if (!CHECK_CALL(DirectX12Wrapper::Command::CreateCommandList(m_Device, m_Allocator, type, &m_List)))
					return;
#ifdef DEBUG_MODE
				if (!CHECK_CALL(DirectX12Wrapper::Debugging::GetDebugCommandList(m_List, &m_DebugList)))
					return;
#endif
			}

			DirectX12CommandBuffer::~DirectX12CommandBuffer(void)
			{
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

			void DirectX12CommandBuffer::Clear(void)
			{
				if (!CHECK_CALL(DirectX12Wrapper::Command::ResetCommandAllocator(m_Allocator)))
					return;

				CHECK_CALL(DirectX12Wrapper::Command::ResetCommandList(m_List, m_Allocator));
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

			void DirectX12CommandBuffer::SetState(const RenderState& State)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Graphics, "Command buffer type is not Graphics");

				PlatformMemory::Copy(&State, &m_State, 1);
			}

			void DirectX12CommandBuffer::CopyFromVertexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Copy, "Command buffer type is not Graphics");

				if (Handle == 0)
					return false;

				if (FromMeshHandle == 0)
					return false;

				BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

				if (!CreateIntermediateBuffer(Size, &boundBufferInfo->Buffer))
					return false;

				boundBufferInfo->Resource = ReinterpretCast(TextureResourceInfo*, FromMeshHandle);

				return true;
			}

			void DirectX12CommandBuffer::CopyFromBufferToVertex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Copy, "Command buffer type is not Graphics");

				if (Handle == 0)
					return false;

				BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

				return CopyBuffer(GPUBufferTypes::Pixel, &boundBufferInfo->Buffer, true, boundBufferInfo->Resource, false);

			}

			void DirectX12CommandBuffer::CopyFromIndexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Copy, "Command buffer type is not Graphics");

				if (Handle == 0)
					return false;

				if (FromMeshHandle == 0)
					return false;

				BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

				if (!CreateIntermediateBuffer(Size, &boundBufferInfo->Buffer))
					return false;

				boundBufferInfo->Resource = ReinterpretCast(TextureResourceInfo*, FromMeshHandle);

				return true;
			}

			void DirectX12CommandBuffer::CopyFromBufferToIndex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Copy, "Command buffer type is not Graphics");

				if (Handle == 0)
					return false;

				BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

				return CopyBuffer(GPUBufferTypes::Pixel, &boundBufferInfo->Buffer, true, boundBufferInfo->Resource, false);
			}

			void DirectX12CommandBuffer::CopyFromTextureToBuffer(ResourceHandle Handle, ResourceHandle FromTextureHandle, uint32 Size, TextureTypes TextureType, Formats TextureFormat, uint32 Level)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Copy, "Command buffer type is not Graphics");

				if (Handle == 0)
					return false;

				if (FromTextureHandle == 0)
					return false;

				BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

				if (!CreateIntermediateBuffer(Size, &boundBufferInfo->Buffer))
					return false;

				boundBufferInfo->Resource = ReinterpretCast(TextureResourceInfo*, FromTextureHandle);

				return true;
			}

			void DirectX12CommandBuffer::CopyFromBufferToTexture(ResourceHandle Handle, ResourceHandle ToTextureHandle, TextureTypes TextureType, uint32 Width, uint32 Height, Formats TextureFormat)
			{
				if (Handle == 0)
					return false;

				BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

				return CopyBuffer(GPUBufferTypes::Pixel, &boundBufferInfo->Buffer, true, boundBufferInfo->Resource, false);

			}

			void DirectX12CommandBuffer::SetProgram(ResourceHandle Handle)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type != Types::Copy, "Command buffer type is not Graphics/Compute");
				CoreDebugAssert(Categories::RenderSystem, Handle != 0, "Program is invalid");

#define IMPLEMENT_SET_SHADER_DATA(StageName) desc.StageName = { programInfos->StageName.Buffer, programInfos->StageName.Size }

				m_CurrentDescriptorHeapCount = 0;

				ProgramInfos* programInfos = ReinterpretCast(ProgramInfos*, Handle);
				bool isGraphics = (programInfos->ComputeShader.Size == 0);

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

						FillGraphicsPipelineState(m_State, desc);

						CHECK_CALL(DirectX12Wrapper::PipelineStateObject::Create(m_Device, &desc, &pipelineState));
					}
					else
					{
						DirectX12Wrapper::PipelineStateObject::ComputePipelineStateDesc desc = {};
						IMPLEMENT_SET_SHADER_DATA(ComputeShader);

						CHECK_CALL(DirectX12Wrapper::PipelineStateObject::Create(m_Device, &desc, &pipelineState));
					}

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
				CoreDebugAssert(Categories::RenderSystem, Value != 0, "Value is invalid");

				BoundBuffersInfo* bufferInfo = ReinterpretCast(BoundBuffersInfo*, Value);

				DirectX12Wrapper::Command::AddSetGraphicsConstantBuffer(m_List, Handle, bufferInfo->Buffer.Resource.Resource->GetGPUVirtualAddress());
			}

			void DirectX12CommandBuffer::SetProgramTexture(ProgramConstantHandle Handle, TextureTypes Type, ResourceHandle Value)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type != Types::Copy, "Command buffer type is not Graphics/Compute");
				CoreDebugAssert(Categories::RenderSystem, Value != 0, "Value is invalid");

				TextureResourceInfo* resourceInfo = ReinterpretCast(TextureResourceInfo*, Value);

				AddTransitionResourceBarrier(m_RenderCommandSet, resourceInfo, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

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
					if (m_CurrentDescriptorHeapCount >= MAX_DESCRIPTOR_HEAP_COUNT)
						return false;

					m_CurrentDescriptorHeaps[m_CurrentDescriptorHeapCount++] = resourceInfo->View.DescriptorHeap;
					m_CurrentDescriptorHeaps[m_CurrentDescriptorHeapCount++] = resourceInfo->SamplerView.DescriptorHeap;

					DirectX12Wrapper::Command::AddSetDescriptorHeap(m_List, m_CurrentDescriptorHeaps, m_CurrentDescriptorHeapCount);
				}

				DirectX12Wrapper::Command::AddSetGraphicsRootDescriptorTable(m_List, Handle + 1, resourceInfo->SamplerView.GPUHandle);

				DirectX12Wrapper::Command::AddSetGraphicsRootDescriptorTable(m_List, Handle, resourceInfo->View.GPUHandle);
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
					m_CurrentRenderTarget = ReinterpretCast(RenderTargetInfos*, Handle);

					for (auto& viewInfo : m_CurrentRenderTarget->Views)
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

			void DirectX12CommandBuffer::SetMesh(ResourceHandle Handle)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Graphics, "Command buffer type is not Graphics");

				if (Handle == 0)
					return false;

				DirectX12Wrapper::Command::AddSetPrimitiveTopologyCommand(m_List, GetPolygonTopology(PolygonTypes::Triangles));

				MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Handle);

				BufferInfo& vertextBufferInfo = meshBufferInfo->VertexBuffer;
				DirectX12Wrapper::Command::AddSetVertexBufferCommand(m_List, vertextBufferInfo.Resource.Resource, vertextBufferInfo.Size, vertextBufferInfo.Stride);

				BufferInfo& indextBufferInfo = meshBufferInfo->IndexBuffer;
				if (indextBufferInfo.Resource.Resource != nullptr)
					DirectX12Wrapper::Command::AddSetIndexBufferCommand(m_List, indextBufferInfo.Resource.Resource, indextBufferInfo.Size);
			}

			void DirectX12CommandBuffer::Clear(ClearFlags Flags, const ColorUI8& Color)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Graphics, "Command buffer type is not Graphics");

				m_ClearColor = Color;

				if (BitwiseUtils::IsEnabled(Flags, ClearFlags::ColorBuffer))
				{
					Vector4F color;
					Helper::GetNormalizedColor(m_ClearColor, color);

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

			void DirectX12CommandBuffer::DrawIndexed(PolygonTypes PolygonType, uint32 IndexCount)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Graphics, "Command buffer type is not Graphics");

				DirectX12Wrapper::Command::AddSetPrimitiveTopologyCommand(m_List, GetPolygonTopology(PolygonType));

				DirectX12Wrapper::Command::AddDrawIndexedCommand(m_List, IndexCount);
			}

			bool DirectX12CommandBuffer::DrawArray(PolygonTypes PolygonType, uint32 VertexCount)
			{
				CoreDebugAssert(Categories::RenderSystem, m_Type == Types::Graphics, "Command buffer type is not Graphics");

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
		}
	}
}