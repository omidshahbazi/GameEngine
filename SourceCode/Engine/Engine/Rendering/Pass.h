// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef PASS_H
#define PASS_H

#include <Rendering\IDevice.h>

namespace Engine
{
	namespace Rendering
	{
		class Texture;

		class RENDERING_API Pass
		{
		public:
			Pass(Program *Program);

			INLINE IDevice::FaceOrders GetFaceOrder(void) const
			{
				return m_FaceOrder;
			}

			INLINE void SetFaceOrder(IDevice::FaceOrders Order)
			{
				m_FaceOrder = Order;
			}

			INLINE IDevice::CullModes GetCullMode(void) const
			{
				return m_CullMode;
			}

			INLINE void SetCullMode(IDevice::CullModes Mode)
			{
				m_CullMode = Mode;
			}

			INLINE IDevice::TestFunctions GetDepthTestFunction(void) const
			{
				return m_DepthTestFunction;
			}

			INLINE void SetDepthTestFunction(IDevice::TestFunctions Function)
			{
				m_DepthTestFunction = Function;
			}

			INLINE void GetStencilTestFunction(IDevice::TestFunctions &Function, int32 &Reference, uint32 &Mask) const
			{
				Function = m_StencilTestFunction;
				Reference = m_StencilTestFunctionReference;
				Mask = m_StencilTestFunctionMask;
			}

			INLINE void SetStencilTestFunction(IDevice::TestFunctions Function, int32 Reference, uint32 Mask)
			{
				m_StencilTestFunction = Function;
				m_StencilTestFunctionReference = Reference;
				m_StencilTestFunctionMask = Mask;
			}

			INLINE uint32 GetStencilMask(void) const
			{
				return m_StencilMask;
			}

			INLINE void SetStencilMask(uint32 Mask)
			{
				m_StencilMask = Mask;
			}

			INLINE void GetStencilOperation(IDevice::StencilOperations &StencilFailed, IDevice::StencilOperations &DepthFailed, IDevice::StencilOperations &DepthPassed) const
			{
				StencilFailed = m_StencilOperationStencilFailed;
				DepthFailed = m_StencilOperationDepthFailed;
				DepthPassed = m_StencilOperationDepthPassed;
			}

			INLINE void SetStencilOperation(IDevice::StencilOperations StencilFailed, IDevice::StencilOperations DepthFailed, IDevice::StencilOperations DepthPassed)
			{
				m_StencilOperationStencilFailed = StencilFailed;
				m_StencilOperationDepthFailed = DepthFailed;
				m_StencilOperationDepthPassed = DepthPassed;
			}

			INLINE void GetBlendFunction(IDevice::BlendFunctions &SourceFactor, IDevice::BlendFunctions &DestinationFactor) const
			{
				SourceFactor = m_BlendFunctionSourceFactor;
				DestinationFactor = m_BlendFunctionDestinationFactor;
			}

			INLINE void SetBlendFunction(IDevice::BlendFunctions SourceFactor, IDevice::BlendFunctions DestinationFactor)
			{
				m_BlendFunctionSourceFactor = SourceFactor;
				m_BlendFunctionDestinationFactor = DestinationFactor;
			}

			INLINE void GetPolygonMode(IDevice::CullModes &CullMode, IDevice::PolygonModes &PolygonMode) const
			{
				CullMode = m_PolygonModeCullMode;
				PolygonMode = m_PolygonMode;
			}

			INLINE void SetPolygonMode(IDevice::CullModes CullMode, IDevice::PolygonModes PolygonMode)
			{
				m_PolygonModeCullMode = CullMode;
				m_PolygonMode = PolygonMode;
			}

			bool SetFloat32(const String &Name, float32 Value);
			bool SetVector2(const String &Name, const Vector2F &Value);
			bool SetVector3(const String &Name, const Vector3F &Value);
			bool SetVector4(const String &Name, const Vector4F &Value);
			bool SetColor(const String &Name, Color Value);
			bool SetMatrix4(const String &Name, const Matrix4F &Value);
			bool SetTexture(const String &Name, const Texture *Value);

			INLINE Program *GetProgram(void) const
			{
				return m_Program;
			}

			void SetProgram(Program *Program);

			INLINE Program::ConstantDataList &GetConstants(void)
			{
				return m_Constants;
			}

			INLINE const Program::ConstantDataList &GetConstants(void) const
			{
				return m_Constants;
			}

		private:
			Program::ConstantData *GetConstantData(const String &Name);

			bool SetConstantValue(const String &Name, const AnyDataType &Value);

		private:
			Program * m_Program;
			IDevice::FaceOrders m_FaceOrder;
			IDevice::CullModes m_CullMode;
			IDevice::TestFunctions m_DepthTestFunction;
			IDevice::TestFunctions m_StencilTestFunction;
			int32 m_StencilTestFunctionReference;
			uint32 m_StencilTestFunctionMask;
			uint32 m_StencilMask;
			IDevice::StencilOperations m_StencilOperationStencilFailed;
			IDevice::StencilOperations m_StencilOperationDepthFailed;
			IDevice::StencilOperations m_StencilOperationDepthPassed;
			IDevice::BlendFunctions m_BlendFunctionSourceFactor;
			IDevice::BlendFunctions m_BlendFunctionDestinationFactor;
			IDevice::PolygonModes m_PolygonMode;
			IDevice::CullModes m_PolygonModeCullMode;
			Program::ConstantDataList m_Constants;
		};
	}
}

#endif