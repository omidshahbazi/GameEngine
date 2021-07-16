// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\BuiltiInProgramConstants.h>
#include <Rendering\Private\RenderingAllocators.h>
#include <Rendering\ProgramConstantSupplier.h>
#include <Rendering\Program.h>
#include <Utility\HighResolutionTime.h>
#include <Rendering\ConstantBuffer.h>

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
				m_DeviceInterface->DestroyConstantBuffer(m_TransformDataBuffer);
				m_DeviceInterface->DestroyConstantBuffer(m_InverseTransformDataBuffer);
				m_DeviceInterface->DestroyConstantBuffer(m_ViewportDataBuffer);
				m_DeviceInterface->DestroyConstantBuffer(m_TimeDataBuffer);

				m_DeviceInterface->RemoveListener(this);

				m_DeviceInterface = nullptr;
			}

			void BuiltiInProgramConstants::Initialize(DeviceInterface* DeviceInterface)
			{
				m_DeviceInterface = DeviceInterface;

				m_TransformDataBuffer = m_DeviceInterface->CreateConstantBuffer(sizeof(TransformData));
				ProgramConstantSupplier::GetInstance()->RegisterBufferConstant("_TransformData", [this]()
					{
						return m_TransformDataBuffer;
					});

				m_InverseTransformDataBuffer = m_DeviceInterface->CreateConstantBuffer(sizeof(TransformData));
				ProgramConstantSupplier::GetInstance()->RegisterBufferConstant("_InverseTransformData", [this]()
					{
						return m_InverseTransformDataBuffer;
					});

				m_ViewportDataBuffer = m_DeviceInterface->CreateConstantBuffer(sizeof(ViewportData));
				ProgramConstantSupplier::GetInstance()->RegisterBufferConstant("_ViewportData", [this]()
					{
						return m_ViewportDataBuffer;
					});

				m_TimeDataBuffer = m_DeviceInterface->CreateConstantBuffer(sizeof(TimeData));
				ProgramConstantSupplier::GetInstance()->RegisterBufferConstant("_TimeData", [this]()
					{
						static HighResolutionTime timer;

						TimeData* data = m_TimeDataBuffer->Get<TimeData>();
						data->Time = timer.GetTime().GetSeconds();
						data->TimeSin = Mathematics::Sin<float32>(data->Time);

						return m_TimeDataBuffer;
					});

				m_DeviceInterface->AddListener(this);

				OnWindowChanged(m_DeviceInterface->GetWindow());
			}

			void BuiltiInProgramConstants::SetTransfomData(const TransformData& Data)
			{
				m_TransformDataBuffer->Set(&Data);

				TransformData inverseData;
				inverseData.Model = (*Data.Model).GetInverse();
				inverseData.View = (*Data.View).GetInverse();
				inverseData.Projection = (*Data.Projection).GetInverse();
				inverseData.MVP = (*Data.MVP).GetInverse();

				m_InverseTransformDataBuffer->Set(&inverseData);
			}

			void BuiltiInProgramConstants::OnWindowChanged(Window* Window)
			{
				if (Window == nullptr)
					return;

				Vector2I size = Window->GetClientSize();

				ViewportData* data = m_ViewportDataBuffer->Get<ViewportData>();
				data->FrameSize = Vector2F(size.X, size.Y);
			}
		}
	}
}