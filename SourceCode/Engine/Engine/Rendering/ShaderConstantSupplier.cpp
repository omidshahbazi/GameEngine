// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\ShaderConstantSupplier.h>
#include <Rendering\IDevice.h>
#include <Rendering\Shader.h>
#include <Containers\Strings.h>
#include <Utility\HighResolutionTime.h>
#include <Rendering\RenderingManager.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		SINGLETON_DEFINITION(ShaderConstantSupplier)

			void ShaderConstantSupplier::Initialize(void)
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

		void ShaderConstantSupplier::SupplyConstants(IDevice* Device, Shader* Shader) const
		{
			const auto& constants = Shader->GetConstants();

			for each (const auto & constant in constants)
			{
				if (!m_Infos.Contains(constant.Name))
					continue;

				auto& info = m_Infos[constant.Name];

				auto& value = (*info.Function)();

				switch (info.DataType)
				{
				case ShaderDataType::Types::Float:
					Device->SetShaderFloat32(constant.Handle, value.Get<float32>());
					break;

				case ShaderDataType::Types::Float2:
					Device->SetShaderVector2(constant.Handle, value.Get<Vector2F>());
					break;

				case ShaderDataType::Types::Float3:
					Device->SetShaderVector3(constant.Handle, value.Get<Vector3F>());
					break;

				case ShaderDataType::Types::Matrix4:
					Device->SetShaderMatrix4(constant.Handle, value.Get<Matrix4F>());
					break;

				case ShaderDataType::Types::Texture2D:
				{
					Texture* texture = ReinterpretCast(Texture*, value.Get<void*>());

					Device->SetShaderTexture(constant.Handle, (texture == nullptr ? Texture::Types::TwoD : texture->GetType()), (texture == nullptr ? 0 : texture->GetHandle()));
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