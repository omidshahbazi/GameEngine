// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DirectX12RenderDevice\Private\DirectX12CommandBuffer.h>
#include <DirectX12RenderDevice\Private\DirectX12Wrapper.h>
#include <pix.h>

namespace Engine
{
	namespace DirectX12RenderDevice
	{
		namespace Private
		{
			DirectX12CommandBuffer::DirectX12CommandBuffer(void)
			{
			}

			DirectX12CommandBuffer::~DirectX12CommandBuffer(void)
			{
			}

			void DirectX12CommandBuffer::SetViewport(const Vector2I& Position, const Vector2I& Size)
			{
				m_Viewport.TopLeftX = Position.X;
				m_Viewport.TopLeftY = Position.Y;
				m_Viewport.Width = Size.X;
				m_Viewport.Height = Size.Y;
				m_Viewport.MinDepth = 0;
				m_Viewport.MaxDepth = 1;

				if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetViewportCommand(m_RenderCommandSet.List, &m_Viewport)))
					return false;

				return true;
			}

			void DirectX12CommandBuffer::SetState(const RenderState& State)
			{
				PlatformMemory::Copy(&State, &m_State, 1);
			}

			void DirectX12CommandBuffer::CopyFromVertexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size)
			{
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
				if (Handle == 0)
					return false;

				BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

				return CopyBuffer(GPUBufferTypes::Pixel, &boundBufferInfo->Buffer, true, boundBufferInfo->Resource, false);

			}

			void DirectX12CommandBuffer::CopyFromIndexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size)
			{
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
				if (Handle == 0)
					return false;

				BoundBuffersInfo* boundBufferInfo = ReinterpretCast(BoundBuffersInfo*, Handle);

				return CopyBuffer(GPUBufferTypes::Pixel, &boundBufferInfo->Buffer, true, boundBufferInfo->Resource, false);
			}

			void DirectX12CommandBuffer::CopyFromTextureToBuffer(ResourceHandle Handle, ResourceHandle FromTextureHandle, uint32 Size, TextureTypes TextureType, Formats TextureFormat, uint32 Level)
			{
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
#define IMPLEMENT_SET_SHADER_DATA(StageName) desc.StageName = { programInfos->StageName.Buffer, programInfos->StageName.Size }

				m_CurrentDescriptorHeapCount = 0;

				m_AnyProgramBound = false;

				if (Handle == 0)
					return true;

				ProgramInfos* programInfos = ReinterpretCast(ProgramInfos*, Handle);

				uint32 stateHash = GetStateHash();

				ID3D12PipelineState* pipelineState = nullptr;

				if (programInfos->Pipelines.Contains(stateHash))
					pipelineState = programInfos->Pipelines[stateHash];
				else
				{
					DirectX12Wrapper::PipelineStateObject::GraphicsPipelineStateDesc desc = {};
					IMPLEMENT_SET_SHADER_DATA(VertexShader);
					IMPLEMENT_SET_SHADER_DATA(TessellationShader);
					IMPLEMENT_SET_SHADER_DATA(GeometryShader);
					IMPLEMENT_SET_SHADER_DATA(FragmentShader);
					//IMPLEMENT_SET_SHADER_DATA(ComputeShader);

					FillGraphicsPipelineState(m_State, desc);

					CHECK_CALL(DirectX12Wrapper::PipelineStateObject::Create(m_Device, &desc, &pipelineState));

					programInfos->Pipelines[stateHash] = pipelineState;
				}

				if (pipelineState == nullptr)
					return false;

				if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetGraphicsRootSignature(m_RenderCommandSet.List, programInfos->RootSignature)))
					return false;

				m_AnyProgramBound = CHECK_CALL(DirectX12Wrapper::Command::AddSetPipelineState(m_RenderCommandSet.List, pipelineState));

				return m_AnyProgramBound;

#undef IMPLEMENT_SET_SHADER_DATA
			}

			void DirectX12CommandBuffer::SetProgramConstantBuffer(ProgramConstantHandle Handle, ResourceHandle Value)
			{
				if (!m_AnyProgramBound)
					return false;

				if (Value == 0)
					return false;

				BoundBuffersInfo* bufferInfo = ReinterpretCast(BoundBuffersInfo*, Value);

				return CHECK_CALL(DirectX12Wrapper::Command::AddSetGraphicsConstantBuffer(m_RenderCommandSet.List, Handle, bufferInfo->Buffer.Resource.Resource->GetGPUVirtualAddress()));

			}

			void DirectX12CommandBuffer::SetProgramTexture(ProgramConstantHandle Handle, TextureTypes Type, ResourceHandle Value)
			{
				if (!m_AnyProgramBound)
					return false;

				if (Value == 0)
					return false;

				TextureResourceInfo* resourceInfo = ReinterpretCast(TextureResourceInfo*, Value);

				if (!AddTransitionResourceBarrier(m_RenderCommandSet, resourceInfo, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE))
					return false;

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

					if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetDescriptorHeap(m_RenderCommandSet.List, m_CurrentDescriptorHeaps, m_CurrentDescriptorHeapCount)))
						return false;
				}

				if (!DirectX12Wrapper::Command::AddSetGraphicsRootDescriptorTable(m_RenderCommandSet.List, Handle + 1, resourceInfo->SamplerView.GPUHandle))
					return false;

				return CHECK_CALL(DirectX12Wrapper::Command::AddSetGraphicsRootDescriptorTable(m_RenderCommandSet.List, Handle, resourceInfo->View.GPUHandle));
			}

			void DirectX12CommandBuffer::SetRenderTarget(ResourceHandle Handle)
			{
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
					if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetTargets(m_RenderCommandSet.List, renderTargetHandles, m_CurrentRenderTargetViewCount, (m_CurrentDepthStencilView == nullptr ? nullptr : &m_CurrentDepthStencilView->TargetView.CPUHandle))))
						return false;

				return true;
			}

			void DirectX12CommandBuffer::SetMesh(ResourceHandle Handle)
			{
				if (Handle == 0)
					return false;

				if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetPrimitiveTopologyCommand(m_RenderCommandSet.List, GetPolygonTopology(PolygonTypes::Triangles))))
					return false;

				MeshBufferInfo* meshBufferInfo = ReinterpretCast(MeshBufferInfo*, Handle);

				BufferInfo& vertextBufferInfo = meshBufferInfo->VertexBuffer;
				if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetVertexBufferCommand(m_RenderCommandSet.List, vertextBufferInfo.Resource.Resource, vertextBufferInfo.Size, vertextBufferInfo.Stride)))
					return false;

				BufferInfo& indextBufferInfo = meshBufferInfo->IndexBuffer;
				if (indextBufferInfo.Resource.Resource != nullptr)
					if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetIndexBufferCommand(m_RenderCommandSet.List, indextBufferInfo.Resource.Resource, indextBufferInfo.Size)))
						return false;

				return true;
			}

			void DirectX12CommandBuffer::Clear(ClearFlags Flags, const ColorUI8& Color)
			{
				m_ClearColor = Color;

				if (BitwiseUtils::IsEnabled(Flags, ClearFlags::ColorBuffer))
				{
					Vector4F color;
					Helper::GetNormalizedColor(m_ClearColor, color);

					for (uint8 i = 0; i < m_CurrentRenderTargetViewCount; ++i)
					{
						if (!CHECK_CALL(DirectX12Wrapper::Command::AddClearRenderTargetCommand(m_RenderCommandSet.List, m_CurrentRenderTargetViews[i]->TargetView.CPUHandle, &color.X)))
							return false;
					}
				}

				bool shouldClearDepth = BitwiseUtils::IsEnabled(Flags, ClearFlags::DepthBuffer);
				bool shouldClearStencil = BitwiseUtils::IsEnabled(Flags, ClearFlags::StencilBuffer);
				if (m_CurrentDepthStencilView != nullptr && (shouldClearDepth || shouldClearStencil))
				{
					D3D12_CLEAR_FLAGS flags = (D3D12_CLEAR_FLAGS)0;
					if (shouldClearDepth) flags |= D3D12_CLEAR_FLAG_DEPTH;
					if (shouldClearStencil) flags |= D3D12_CLEAR_FLAG_STENCIL;

					if (!CHECK_CALL(DirectX12Wrapper::Command::AddClearDepthStencilCommand(m_RenderCommandSet.List, m_CurrentDepthStencilView->TargetView.CPUHandle, flags, 1, 1)))
						return false;
				}

				return true;
			}

			void DirectX12CommandBuffer::DrawIndexed(PolygonTypes PolygonType, uint32 IndexCount)
			{
				if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetPrimitiveTopologyCommand(m_RenderCommandSet.List, GetPolygonTopology(PolygonType))))
					return false;

				if (!CHECK_CALL(DirectX12Wrapper::Command::AddDrawIndexedCommand(m_RenderCommandSet.List, IndexCount)))
					return false;

				return true;
			}

			void DirectX12CommandBuffer::DrawArray(PolygonTypes PolygonType, uint32 VertexCount)
			{
				if (!CHECK_CALL(DirectX12Wrapper::Command::AddSetPrimitiveTopologyCommand(m_RenderCommandSet.List, GetPolygonTopology(PolygonType))))
					return false;

				if (!CHECK_CALL(DirectX12Wrapper::Command::AddDrawCommand(m_RenderCommandSet.List, VertexCount)))
					return false;

				return true;
			}

			void DirectX12CommandBuffer::BeginEvent(cwstr Label)
			{
				PIXBeginEvent(m_RenderCommandSet.List, 0, Label);
			}

			void DirectX12CommandBuffer::EndEvent(void)
			{
				PIXEndEvent(m_RenderCommandSet.List);
			}

			void DirectX12CommandBuffer::SetMarker(cwstr Label)
			{
				PIXSetMarker(m_RenderCommandSet.List, 0, Label);
			}
		}
	}
}