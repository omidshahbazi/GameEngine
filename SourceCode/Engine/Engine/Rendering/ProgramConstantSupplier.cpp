// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\ProgramConstantSupplier.h>
#include <Rendering\IDevice.h>
#include <Rendering\Program.h>
#include <Containers\Strings.h>
#include <Utility\HighResolutionTime.h>
#include <Rendering\RenderingManager.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		SINGLETON_DEFINITION(ProgramConstantSupplier)

			void ProgramConstantSupplier::Initialize(void)
		{
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

			auto device = RenderingManager::GetInstance()->GetActiveDevice();
			device->AddListener(this);
		}

		void ProgramConstantSupplier::RegisterFloatConstant(const String& Name, FetchConstantFunction Function)
		{
			m_Infos[Name] = ConstantSupplierInfo{ DataType::Types::Float, std::make_shared<FetchConstantFunction>(Function) };
		}

		void ProgramConstantSupplier::RegisterFloat2Constant(const String& Name, FetchConstantFunction Function)
		{
			m_Infos[Name] = ConstantSupplierInfo{ DataType::Types::Float2, std::make_shared<FetchConstantFunction>(Function) };
		}

		void ProgramConstantSupplier::RegisterFloat3Constant(const String& Name, FetchConstantFunction Function)
		{
			m_Infos[Name] = ConstantSupplierInfo{ DataType::Types::Float3, std::make_shared<FetchConstantFunction>(Function) };
		}

		void ProgramConstantSupplier::RegisterMatrix4Constant(const String& Name, FetchConstantFunction Function)
		{
			m_Infos[Name] = ConstantSupplierInfo{ DataType::Types::Matrix4, std::make_shared<FetchConstantFunction>(Function) };
		}

		void ProgramConstantSupplier::RegisterTextureConstant(const String& Name, FetchConstantFunction Function)
		{
			m_Infos[Name] = ConstantSupplierInfo{ DataType::Types::Texture2D, std::make_shared<FetchConstantFunction>(Function) };
		}

		void ProgramConstantSupplier::SupplyConstants(IDevice* Device, Program* Program) const
		{
			const auto& constants = Program->GetConstants();

			for each (const auto & constant in constants)
			{
				if (!m_Infos.Contains(constant.Name))
					continue;

				auto& info = m_Infos[constant.Name];

				auto& value = (*info.Function)();

				switch (info.DataType)
				{
				case DataType::Types::Float:
					Device->SetProgramFloat32(constant.Handle, value.Get<float32>());
					break;

				case DataType::Types::Float2:
					Device->SetProgramVector2(constant.Handle, value.Get<Vector2F>());
					break;

				case DataType::Types::Float3:
					Device->SetProgramVector3(constant.Handle, value.Get<Vector3F>());
					break;

				case DataType::Types::Matrix4:
					Device->SetProgramMatrix4(constant.Handle, value.Get<Matrix4F>());
					break;

				case DataType::Types::Texture2D:
				{
					Texture* texture = ReinterpretCast(Texture*, value.Get<void*>());

					Device->SetProgramTexture(constant.Handle, (texture == nullptr ? Texture::Types::TwoD : texture->GetType()), (texture == nullptr ? 0 : texture->GetHandle()));
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