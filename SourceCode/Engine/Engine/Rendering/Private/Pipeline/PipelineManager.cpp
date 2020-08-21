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
				SINGLETON_DEFINITION(PipelineManager);

				PipelineManager::PipelineManager(void) :
					m_DeviceInterface(nullptr),
					m_SelectedPipeline(nullptr)
				{
				}

				PipelineManager::~PipelineManager(void)
				{
					m_DeviceInterface->RemoveListener(this);

					RenderingAllocators::RenderingSystemAllocator_Deallocate(m_SelectedPipeline);
				}

				void PipelineManager::Initialize(DeviceInterface* DeviceInterface)
				{
					m_DeviceInterface = DeviceInterface;

					m_DeviceInterface->AddListener(this);

					DeferredRendering* deferredRenderingPipeline = RenderingAllocators::RenderingSystemAllocator_Allocate<DeferredRendering>();
					Construct(deferredRenderingPipeline, m_DeviceInterface);

					m_SelectedPipeline = deferredRenderingPipeline;
				}

				void PipelineManager::BeginRender(void)
				{
					if (m_SelectedPipeline == nullptr)
						return;

					m_SelectedPipeline->BeginRender();
				}

				void PipelineManager::EndRender(void)
				{
					if (m_SelectedPipeline == nullptr)
						return;

					m_SelectedPipeline->EndRender();
				}

				void PipelineManager::OnWindowChanged(Window* Window)
				{
					if (m_SelectedPipeline == nullptr)
						return;

					m_SelectedPipeline->OnWindowChanged(Window);
				}

				void PipelineManager::OnWindowResized(Window* Window)
				{
					if (m_SelectedPipeline == nullptr)
						return;

					m_SelectedPipeline->OnWindowResized(Window);
				}
			}
		}
	}
}