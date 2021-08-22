// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\Pipeline\PipelineManager.h>
#include <Rendering\Private\Pipeline\DeferredRendering.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Debugging;

	namespace Rendering
	{
		namespace Private
		{
			namespace Pipeline
			{
				SINGLETON_DEFINITION(PipelineManager);

				PipelineManager::PipelineManager(void) :
					m_Initialized(false),
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
					CoreDebugAssert(Categories::Rendering, !m_Initialized, "Core already initialized");

					m_DeviceInterface = DeviceInterface;

					m_DeviceInterface->AddListener(this);

					DeferredRendering* deferredRenderingPipeline = RenderingAllocators::RenderingSystemAllocator_Allocate<DeferredRendering>();
					Construct(deferredRenderingPipeline, m_DeviceInterface);

					m_SelectedPipeline = deferredRenderingPipeline;

					OnContextChanged(m_DeviceInterface->GetContext());

					m_Initialized = true;
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

				void PipelineManager::OnContextChanged(RenderContext* Context)
				{
					if (m_SelectedPipeline == nullptr)
						return;

					m_SelectedPipeline->OnContextChanged(Context);
				}

				void PipelineManager::OnContextResized(RenderContext* Context)
				{
					if (m_SelectedPipeline == nullptr)
						return;

					m_SelectedPipeline->OnContextResized(Context);
				}
			}
		}
	}
}