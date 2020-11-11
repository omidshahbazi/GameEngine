// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\ShaderConstantSupplier.h>
#include <Rendering\Private\ThreadedDevice.h>
#include <Rendering\Shader.h>
#include <Containers\Strings.h>
#include <Utility\HighResolutionTime.h>
#include <Rendering\RenderingManager.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		SINGLETON_DEFINITION(ShaderConstantSupplier);

		void ShaderConstantSupplier::Initialize(DeviceInterface* DeviceInterface)
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

		void ShaderConstantSupplier::RegisterFloatConstant(const String& Name, FetchConstantFunction Function)
		{
			m_Infos[Name] = ConstantSupplierInfo{ ShaderDataType::Types::Float, std::make_shared<FetchConstantFunction>(Function) };
		}

		void ShaderConstantSupplier::RegisterFloat2Constant(const String& Name, FetchConstantFunction Function)
		{
			m_Infos[Name] = ConstantSupplierInfo{ ShaderDataType::Types::Float2, std::make_shared<FetchConstantFunction>(Function) };
		}

		void ShaderConstantSupplier::RegisterFloat3Constant(const String& Name, FetchConstantFunction Function)
		{
			m_Infos[Name] = ConstantSupplierInfo{ ShaderDataType::Types::Float3, std::make_shared<FetchConstantFunction>(Function) };
		}

		void ShaderConstantSupplier::RegisterMatrix4Constant(const String& Name, FetchConstantFunction Function)
		{
			m_Infos[Name] = ConstantSupplierInfo{ ShaderDataType::Types::Matrix4, std::make_shared<FetchConstantFunction>(Function) };
		}

		void ShaderConstantSupplier::RegisterTextureConstant(const String& Name, FetchConstantFunction Function)
		{
			m_Infos[Name] = ConstantSupplierInfo{ ShaderDataType::Types::Texture2D, std::make_shared<FetchConstantFunction>(Function) };
		}

		void ShaderConstantSupplier::SupplyConstants(Shader* Shader) const
		{
			Shader::ConstantDataMap& constants = Shader->GetConstants();

			for (auto& item : constants)
			{
				Shader::ConstantData& constant = item.GetSecond();

				if (!m_Infos.Contains(constant.Name))
					continue;

				auto& value = (*m_Infos[constant.Name].Function)();

				switch (constant.Type)
				{
				case ShaderDataType::Types::Float:
					constant.Value = value.Get<float32>();
					break;

				case ShaderDataType::Types::Float2:
					constant.Value = value.Get<Vector2F>();
					break;

				case ShaderDataType::Types::Float3:
					constant.Value = value.Get<Vector3F>();
					break;

				case ShaderDataType::Types::Matrix4:
					constant.Value = value.Get<Matrix4F>();
					break;

				case ShaderDataType::Types::Texture2D:
				{
					constant.Value = value.Get<void*>();
				} break;
				}
			}
		}

		void ShaderConstantSupplier::OnWindowChanged(Window* Window)
		{
			if (Window == nullptr)
				return;

			m_FrameSize = Window->GetClientSize();
		}

		void ShaderConstantSupplier::OnWindowResized(Window* Window)
		{
			m_FrameSize = Window->GetClientSize();
		}
	}
}