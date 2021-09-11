// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PIPELINE_H
#define PIPELINE_H

#include <MemoryManagement\Singleton.h>
#include <RenderSystem\DeviceInterface.h>
#include <RenderSystem\Private\Pipeline\IPipeline.h>

namespace Engine
{
	namespace RenderSystem
	{
		namespace Private
		{
			namespace Pipeline
			{
				class RENDERSYSTEM_API PipelineManager
				{
					SINGLETON_DECLARATION(PipelineManager)

				private:
					PipelineManager(void);
					~PipelineManager(void);

				public:
					void Initialize(DeviceInterface* DeviceInterface);

					void BeginRender(void);
					void EndRender(void);

					IPipeline* GetSelectedPipeline(void)
					{
						return m_SelectedPipeline;
					}

				private:
					void OnContextChanged(RenderContext* Context);
					DECLARE_MEMBER_EVENT_LISTENER(PipelineManager, OnContextChanged);

					void OnContextResized(RenderContext* Context);
					DECLARE_MEMBER_EVENT_LISTENER(PipelineManager, OnContextResized);

				private:
					bool m_Initialized;

					DeviceInterface* m_DeviceInterface;
					IPipeline* m_SelectedPipeline;
				};
			}
		}
	}
}

#endif