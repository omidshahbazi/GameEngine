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

				public:
					void Initialize(DeviceInterface* DeviceInterface);

					void BeginRender(void);
					void EndRender(void);

					void OnDeviceInterfaceResized(DeviceInterface* DeviceInterface);

					IPipeline* GetSelectedPipeline(void)
					{
						return m_SelectedPipeline;
					}

				private:
					DeviceInterface* m_DeviceInterface;
					IPipeline* m_SelectedPipeline;
				};
			}
		}
	}
}

#endif