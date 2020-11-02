// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PIPELINE_H
#define PIPELINE_H

#include <MemoryManagement\Singleton.h>
#include <Rendering\DeviceInterface.h>
#include <Rendering\Private\Pipeline\IPipeline.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Pipeline
			{
				class RENDERING_API PipelineManager : public DeviceInterface::IListener
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
					void OnWindowChanged(Window* Window) override;
					void OnWindowResized(Window* Window) override;

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