// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BUILT_IN_PROGRAM_CONSTANTS_H
#define BUILT_IN_PROGRAM_CONSTANTS_H

#include <MemoryManagement\Singleton.h>
#include <Rendering\DeviceInterface.h>
#include <Rendering\GPUAlignedType.h>

namespace Engine
{
	namespace Rendering
	{
		class CPUConstantBuffer;

		namespace Private
		{
			class BuiltiInProgramConstants : private DeviceInterface::IListener
			{
			public:
				struct TransformData
				{
				public:
					GPUAlignedMatrix4F Model;
					GPUAlignedMatrix4F View;
					GPUAlignedMatrix4F Projection;
					GPUAlignedMatrix4F MVP;
				};

				SINGLETON_DECLARATION(BuiltiInProgramConstants);

				friend class DeviceInterface;
				friend class DrawCommand;

			private:
				BuiltiInProgramConstants(void) :
					m_DeviceInterface(nullptr)
				{
				}

				~BuiltiInProgramConstants(void);

				void Initialize(DeviceInterface* DeviceInterface);

			private:
				void SetTransfomData(const TransformData& Data);

				void OnWindowChanged(Window* Window) override;
				void OnWindowResized(Window* Window) override;

			private:
				DeviceInterface* m_DeviceInterface;

				CPUConstantBuffer* m_TransformDataBuffer;
				Vector2I m_FrameSize;
			};
		}
	}
}

#endif