// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\ProgramConstantSupplier.h>
#include <Rendering\Private\ThreadedDevice.h>
#include <Rendering\Program.h>
#include <Containers\Strings.h>
#include <Utility\HighResolutionTime.h>
#include <Rendering\RenderingManager.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		SINGLETON_DEFINITION(ProgramConstantSupplier);

		void ProgramConstantSupplier::Initialize(DeviceInterface* DeviceInterface)
		{
			Assert(!m_Initialized, "Core already initialized");

			static Utility::HighResolutionTime timer;

			RegisterFloat2Constant("_Time", []() -> AnyDataType
				{
					float32 time = timer.GetTime().GetSeconds();
					float32 sinTime = Mathematics::Sin(time);
					return Vector2F(time, sinTime);
				});

			RegisterFloat2Constant("_FrameSize", [&]() -> AnyDataType
				{
					return Vector2F(m_FrameSize.X, m_FrameSize.Y);
				});

			DeviceInterface->AddListener(this);

			OnWindowChanged(DeviceInterface->GetWindow());

			m_Initialized = true;
		}

		void ProgramConstantSupplier::RegisterFloatConstant(const String& Name, FetchConstantFunction Function)
		{
			m_Infos[Name] = ConstantSupplierInfo{ ProgramDataTypes::Float, std::make_shared<FetchConstantFunction>(Function) };
		}

		void ProgramConstantSupplier::RegisterFloat2Constant(const String& Name, FetchConstantFunction Function)
		{
			m_Infos[Name] = ConstantSupplierInfo{ ProgramDataTypes::Float2, std::make_shared<FetchConstantFunction>(Function) };
		}

		void ProgramConstantSupplier::RegisterFloat3Constant(const String& Name, FetchConstantFunction Function)
		{
			m_Infos[Name] = ConstantSupplierInfo{ ProgramDataTypes::Float3, std::make_shared<FetchConstantFunction>(Function) };
		}

		void ProgramConstantSupplier::RegisterMatrix4Constant(const String& Name, FetchConstantFunction Function)
		{
			m_Infos[Name] = ConstantSupplierInfo{ ProgramDataTypes::Matrix4, std::make_shared<FetchConstantFunction>(Function) };
		}

		void ProgramConstantSupplier::RegisterTextureConstant(const String& Name, FetchConstantFunction Function)
		{
			m_Infos[Name] = ConstantSupplierInfo{ ProgramDataTypes::Texture2D, std::make_shared<FetchConstantFunction>(Function) };
		}

		void ProgramConstantSupplier::SupplyConstants(Program* Program) const
		{
			Program::ConstantDataMap& constants = Program->GetConstants();

			for (auto& item : constants)
			{
				Program::ConstantData& constant = item.GetSecond();

				if (!m_Infos.Contains(constant.Name))
					continue;

				auto& value = (*m_Infos[constant.Name].Function)();

				switch (constant.Type)
				{
				case ProgramDataTypes::Float:
					constant.Value = value.Get<float32>();
					break;

				case ProgramDataTypes::Float2:
					constant.Value = value.Get<Vector2F>();
					break;

				case ProgramDataTypes::Float3:
					constant.Value = value.Get<Vector3F>();
					break;

				case ProgramDataTypes::Matrix4:
					constant.Value = value.Get<Matrix4F>();
					break;

				case ProgramDataTypes::Texture2D:
				{
					constant.Value = value.Get<void*>();
				} break;
				}
			}
		}

		void ProgramConstantSupplier::OnWindowChanged(Window* Window)
		{
			if (Window == nullptr)
				return;

			m_FrameSize = Window->GetClientSize();
		}

		void ProgramConstantSupplier::OnWindowResized(Window* Window)
		{
			m_FrameSize = Window->GetClientSize();
		}
	}
}