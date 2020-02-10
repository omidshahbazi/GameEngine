// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\Pipeline\PipelineManager.h>
#include <Rendering\Private\Pipeline\DeferredRendering.h>
#include <Rendering\Private\RenderingAllocators.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Pipeline
			{
				SINGLETON_DEFINITION(PipelineManager)

					PipelineManager::PipelineManager(void) :
					m_DeviceInterface(nullptr),
					m_SelectedPipeline(nullptr)
				{
				}

				void PipelineManager::Initialize(DeviceInterface* DeviceInterface)
				{
					m_DeviceInterface = DeviceInterface;

					m_DeviceInterface->AddListener(this);

					DeferredRendering* deferredRenderingPipeline = ReinterpretCast(DeferredRendering*, AllocateMemory(&RenderingAllocators::RenderingSystemAllocator, sizeof(DeferredRendering)));
					Construct(deferredRenderingPipeline, m_DeviceInterface);

					m_SelectedPipeline = deferredRenderingPipeline;
				}

				void PipelineManager::BeginRender(void)
				{
					m_SelectedPipeline->BeginRender();
				}

				void PipelineManager::EndRender(void)
				{
					m_SelectedPipeline->EndRender();
				}

				void PipelineManager::OnDeviceInterfaceResized(DeviceInterface* DeviceInterface)
				{
					m_SelectedPipeline->OnDeviceInterfaceResized();
				}
			}
		}
	}
}