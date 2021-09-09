// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Private\BuiltiInProgramConstants.h>
#include <RenderCommon\Private\RenderSystemAllocators.h>
#include <RenderSystem\ProgramConstantSupplier.h>
#include <RenderSystem\DeviceInterface.h>
#include <RenderSystem\RenderContext.h>
#include <RenderSystem\Program.h>
#include <TimeUtility\HighResolutionTime.h>
#include <RenderSystem\ConstantBuffer.h>

namespace Engine
{
	using namespace TimeUtility;
	using namespace RenderCommon::Private;

	namespace RenderSystem
	{
		namespace Private
		{
			SINGLETON_DEFINITION(BuiltiInProgramConstants);

			BuiltiInProgramConstants::BuiltiInProgramConstants(void) :
				m_DeviceInterface(nullptr),
				m_TransformDataBuffer(sizeof(TransformData)),
				m_InverseTransformDataBuffer(sizeof(TransformData)),
				m_ViewportDataBuffer(sizeof(ViewportData)),
				m_TimeDataBuffer(sizeof(ViewportData))
			{
			}

			BuiltiInProgramConstants::~BuiltiInProgramConstants(void)
			{
				m_DeviceInterface->OnContextChangedEvent -= EventListener_OnContextChanged;
				m_DeviceInterface->OnContextResizedEvent -= EventListener_OnContextChanged;

				m_DeviceInterface = nullptr;
			}

			void BuiltiInProgramConstants::Initialize(DeviceInterface* DeviceInterface)
			{
				m_DeviceInterface = DeviceInterface;

				ProgramConstantSupplier::GetInstance()->RegisterBufferConstant("_TransformData", [this]()
					{
						return &m_TransformDataBuffer;
					});

				ProgramConstantSupplier::GetInstance()->RegisterBufferConstant("_InverseTransformData", [this]()
					{
						return &m_InverseTransformDataBuffer;
					});

				ProgramConstantSupplier::GetInstance()->RegisterBufferConstant("_ViewportData", [this]()
					{
						return &m_ViewportDataBuffer;
					});

				ProgramConstantSupplier::GetInstance()->RegisterBufferConstant("_TimeData", [this]()
					{
						static HighResolutionTime timer;

						TimeData* data = m_TimeDataBuffer.Get<TimeData>();
						data->Time = timer.GetTime().GetSeconds();
						data->TimeSin = Math::Sin<float32>(data->Time);

						return &m_TimeDataBuffer;
					});

				m_DeviceInterface->OnContextChangedEvent += EventListener_OnContextChanged;
				m_DeviceInterface->OnContextResizedEvent += EventListener_OnContextChanged;

				OnContextChanged(m_DeviceInterface->GetContext());
			}

			void BuiltiInProgramConstants::SetTransfomData(const TransformData& Data)
			{
				m_TransformDataBuffer.Set(&Data);

				TransformData inverseData;
				inverseData.Model = (*Data.Model).GetInverse();
				inverseData.View = (*Data.View).GetInverse();
				inverseData.Projection = (*Data.Projection).GetInverse();
				inverseData.MVP = (*Data.MVP).GetInverse();

				m_InverseTransformDataBuffer.Set(&inverseData);
			}

			void BuiltiInProgramConstants::OnContextChanged(RenderContext* Context)
			{
				if (Context == nullptr)
					return;

				Vector2I size = Context->GetWindow()->GetClientSize();

				ViewportData* data = m_ViewportDataBuffer.Get<ViewportData>();
				data->FrameSize = Vector2F(size.X, size.Y);
			}
		}
	}
}