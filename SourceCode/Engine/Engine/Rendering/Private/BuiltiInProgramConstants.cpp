// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\BuiltiInProgramConstants.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Rendering\ProgramConstantSupplier.h>
#include <Rendering\Program.h>
#include <Utility\HighResolutionTime.h>
#include <Rendering\CPUConstantBuffer.h>

namespace Engine
{
	using namespace Utility;

	namespace Rendering
	{
		namespace Private
		{
			SINGLETON_DEFINITION(BuiltiInProgramConstants);

			BuiltiInProgramConstants::~BuiltiInProgramConstants(void)
			{
				RenderingAllocators::ContainersAllocator_Deallocate(m_TransformDataBuffer);

				m_DeviceInterface->RemoveListener(this);

				m_DeviceInterface = nullptr;
			}

			void BuiltiInProgramConstants::Initialize(DeviceInterface* DeviceInterface)
			{
				m_DeviceInterface = DeviceInterface;

				m_TransformDataBuffer = RenderingAllocators::ContainersAllocator_Allocate<CPUConstantBuffer>();
				Construct(m_TransformDataBuffer, sizeof(TransformData));
				ProgramConstantSupplier::GetInstance()->RegisterBufferConstant("_TransformData", [this]()
					{
						return m_TransformDataBuffer;
					});

				static HighResolutionTime timer;
				//RegisterFloat2Constant("_Time", []() -> AnyDataType
				//	{
				//		float32 time = timer.GetTime().GetSeconds();
				//		float32 sinTime = Mathematics::Sin(time);
				//		return Vector2F(time, sinTime);
				//	});

				//RegisterFloat2Constant("_FrameSize", [&]() -> AnyDataType
				//	{
				//		return Vector2F(m_FrameSize.X, m_FrameSize.Y);
				//	});

				m_DeviceInterface->AddListener(this);

				OnWindowChanged(m_DeviceInterface->GetWindow());
			}

			void BuiltiInProgramConstants::SetTransfomData(const TransformData& Data)
			{
				m_TransformDataBuffer->Lock(GPUBuffer::Access::WriteOnly);
				m_TransformDataBuffer->Set(&Data);
				m_TransformDataBuffer->Unlock();
			}

			void BuiltiInProgramConstants::OnWindowChanged(Window* Window)
			{
				if (Window == nullptr)
					return;

				m_FrameSize = Window->GetClientSize();
			}

			void BuiltiInProgramConstants::OnWindowResized(Window* Window)
			{
				m_FrameSize = Window->GetClientSize();
			}
		}
	}
}