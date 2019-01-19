// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Pass.h>

namespace Engine
{
	namespace Rendering
	{
		Pass::Pass(Program * Program) :
			m_FaceOrder(IDevice::FaceOrders::CounterClockwise),
			m_CullMode(IDevice::CullModes::Back),
			m_DepthTestFunction(IDevice::TestFunctions::Less),
			m_StencilTestFunction(IDevice::TestFunctions::Always),
			m_StencilTestFunctionReference(0),
			m_StencilTestFunctionMask(0xFF),
			m_StencilMask(0xFF),
			m_StencilOperationStencilFailed(IDevice::StencilOperations::Keep),
			m_StencilOperationDepthFailed(IDevice::StencilOperations::Keep),
			m_StencilOperationDepthPassed(IDevice::StencilOperations::Keep),
			m_BlendFunctionSourceFactor(IDevice::BlendFunctions::One),
			m_BlendFunctionDestinationFactor(IDevice::BlendFunctions::Zero),
			m_PolygonMode(IDevice::PolygonModes::Fill),
			m_PolygonModeCullMode(IDevice::CullModes::Both)
		{
			SetProgram(Program);
		}

		void Pass::SetProgram(Program * Program)
		{
			m_Program = Program;

			m_Constants.Clear();

			if (m_Program == nullptr)
				return;

			m_Constants = m_Program->GetConstants();
		}

		bool Pass::SetFloat32(const String &Name, float32 Value)
		{
			return SetConstantValue(Name, Value);
		}

		bool Pass::SetVector2(const String & Name, const Vector2F & Value)
		{
			return SetConstantValue(Name, Value);
		}

		bool Pass::SetVector3(const String & Name, const Vector3F & Value)
		{
			return SetConstantValue(Name, Value);
		}

		bool Pass::SetVector4(const String & Name, const Vector4F & Value)
		{
			return SetConstantValue(Name, Value);
		}

		bool Pass::SetColor(const String &Name, Color Value)
		{
			return SetConstantValue(Name, Vector4F(Value.GetFloat32R(), Value.GetFloat32G(), Value.GetFloat32B(), Value.GetFloat32A()));
		}

		bool Pass::SetMatrix4(const String &Name, const Matrix4F &Value)
		{
			return SetConstantValue(Name, Value);
		}

		bool Pass::SetTexture(const String & Name, const Texture * Value)
		{
			return SetConstantValue(Name, Value);
		}

		Program::ConstantData *Pass::GetConstantData(const String & Name)
		{
			for each (auto &constant in m_Constants)
				if (constant.Name == Name)
					return ConstCast(Program::ConstantData*, &constant);

			return nullptr;
		}

		bool Pass::SetConstantValue(const String & Name, const AnyDataType & Value)
		{
			Program::ConstantData *data = GetConstantData(Name);
			if (data == nullptr)
				return false;

			data->Value = Value;

			return true;
		}
	}
}