// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\BuiltiInProgramConstantSupplier.h>
#include <Rendering\Program.h>
#include <Utility\HighResolutionTime.h>

namespace Engine
{
	using namespace Utility;

	namespace Rendering
	{
		namespace Private
		{
			SINGLETON_DEFINITION(BuiltiInProgramConstantSupplier);

			BuiltiInProgramConstantSupplier::~BuiltiInProgramConstantSupplier(void)
			{
				m_DeviceInterface->RemoveListener(this);

				m_DeviceInterface = nullptr;
			}

			void BuiltiInProgramConstantSupplier::Initialize(DeviceInterface* DeviceInterface)
			{
				m_DeviceInterface = DeviceInterface;

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

			void BuiltiInProgramConstantSupplier::OnWindowChanged(Window* Window)
			{
				if (Window == nullptr)
					return;

				m_FrameSize = Window->GetClientSize();
			}

			void BuiltiInProgramConstantSupplier::OnWindowResized(Window* Window)
			{
				m_FrameSize = Window->GetClientSize();
			}
		}
	}
}