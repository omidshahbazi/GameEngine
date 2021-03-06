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
		class ConstantBuffer;

		namespace Private
		{
			namespace Commands
			{
				class DrawCommand;
			}

			using namespace Commands;

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

				struct ViewportData
				{
				public:
					GPUAlignedVector2F FrameSize;
				};

				struct TimeData
				{
				public:
					GPUAlignedFloat32 Time;
					GPUAlignedFloat32 TimeSin;
				};

				SINGLETON_DECLARATION(BuiltiInProgramConstants);

				friend class DeviceInterface;
				friend class DrawCommand;

			private:
				BuiltiInProgramConstants(void) :
					m_DeviceInterface(nullptr),
					m_TransformDataBuffer(nullptr),
					m_ViewportDataBuffer(nullptr),
					m_TimeDataBuffer(nullptr)
				{
				}

				~BuiltiInProgramConstants(void);

				void Initialize(DeviceInterface* DeviceInterface);

			private:
				void SetTransfomData(const TransformData& Data);

				void OnWindowChanged(Window* Window) override;
				void OnWindowResized(Window* Window) override
				{
					OnWindowChanged(Window);
				}

			private:
				DeviceInterface* m_DeviceInterface;

				ConstantBuffer* m_TransformDataBuffer;
				ConstantBuffer* m_ViewportDataBuffer;
				ConstantBuffer* m_TimeDataBuffer;
				Vector2I m_FrameSize;
			};
		}
	}
}

#endif