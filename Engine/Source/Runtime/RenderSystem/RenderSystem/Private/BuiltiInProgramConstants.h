// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BUILT_IN_PROGRAM_CONSTANTS_H
#define BUILT_IN_PROGRAM_CONSTANTS_H

#include <RenderSystem\ConstantBuffer.h>
#include <MemoryManagement\Singleton.h>
#include <RenderCommon\GPUAlignedType.h>
#include <Containers\Delegate.h>

namespace Engine
{
	using namespace Containers;

	namespace RenderSystem
	{
		class DeviceInterface;
		class RenderContext;

		namespace Private
		{
			namespace Commands
			{
				class DrawCommand;
			}

			using namespace Commands;

			class BuiltiInProgramConstants
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

				friend class DrawCommand;

			private:
				BuiltiInProgramConstants(void);
				~BuiltiInProgramConstants(void);

			public:
				void Initialize(DeviceInterface* DeviceInterface);

			private:
				void SetTransfomData(const TransformData& Data);

				void OnContextChanged(RenderContext* Context);
				DECLARE_MEMBER_EVENT_LISTENER(BuiltiInProgramConstants, OnContextChanged);

			private:
				DeviceInterface* m_DeviceInterface;

				ConstantBuffer m_TransformDataBuffer;
				ConstantBuffer m_InverseTransformDataBuffer;
				ConstantBuffer m_ViewportDataBuffer;
				ConstantBuffer m_TimeDataBuffer;
				Vector2I m_FrameSize;
			};
		}
	}
}

#endif