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
				RenderingAllocators::ContainersAllocator_Deallocate(m_ViewportDataBuffer);
				RenderingAllocators::ContainersAllocator_Deallocate(m_TimeDataBuffer);

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

				m_ViewportDataBuffer = RenderingAllocators::ContainersAllocator_Allocate<CPUConstantBuffer>();
				Construct(m_ViewportDataBuffer, sizeof(ViewportData));
				ProgramConstantSupplier::GetInstance()->RegisterBufferConstant("_ViewportData", [this]()
					{
						return m_ViewportDataBuffer;
					});

				m_TimeDataBuffer = RenderingAllocators::ContainersAllocator_Allocate<CPUConstantBuffer>();
				Construct(m_TimeDataBuffer, sizeof(TimeData));
				ProgramConstantSupplier::GetInstance()->RegisterBufferConstant("_TimeData", [this]()
					{
						static HighResolutionTime timer;

						m_TimeDataBuffer->Lock(GPUBuffer::Access::WriteOnly);

						TimeData* data = m_TimeDataBuffer->Get<TimeData>();
						data->Time = timer.GetTime().GetSeconds();
						data->TimeSin = Mathematics::Sin<float32>(data->Time);

						m_TimeDataBuffer->Unlock();

						return m_TimeDataBuffer;
					});

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

				Vector2I size = Window->GetClientSize();

				m_ViewportDataBuffer->Lock(GPUBuffer::Access::WriteOnly);

				ViewportData* data = m_ViewportDataBuffer->Get<ViewportData>();
				data->FrameSize = Vector2F(size.X, size.Y);

				m_ViewportDataBuffer->Unlock();
			}
		}
	}
}