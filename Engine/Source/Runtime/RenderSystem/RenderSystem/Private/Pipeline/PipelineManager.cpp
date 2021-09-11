// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Private\Pipeline\PipelineManager.h>
#include <RenderSystem\Private\Pipeline\DeferredRendering.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Debugging;
	using namespace RenderCommon::Private;

	namespace RenderSystem
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
					m_DeviceInterface->OnContextChangedEvent -= EventListener_OnContextChanged;
					m_DeviceInterface->OnContextResizedEvent -= EventListener_OnContextResized;

					RenderSystemAllocators::RenderSystemAllocator_Deallocate(m_SelectedPipeline);
				}

				void PipelineManager::Initialize(DeviceInterface* DeviceInterface)
				{
					CoreDebugAssert(Categories::RenderSystem, !m_Initialized, "Core already initialized");

					m_DeviceInterface = DeviceInterface;

					m_DeviceInterface->OnContextChangedEvent += EventListener_OnContextChanged;
					m_DeviceInterface->OnContextResizedEvent += EventListener_OnContextResized;

					DeferredRendering* deferredRenderingPipeline = RenderSystemAllocators::RenderSystemAllocator_Allocate<DeferredRendering>();
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