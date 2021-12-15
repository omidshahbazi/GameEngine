// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RENDER_STATE_H
#define RENDER_STATE_H

#include <RenderCommon\Enums.h>
#include <Common\BitwiseUtils.h>

namespace Engine
{
	using namespace Common;

	namespace RenderCommon
	{
		struct RenderState
		{
		public:
			struct FaceState
			{
			public:
				FaceState(void) :
					StencilTestFunction(TestFunctions::Always),
					StencilTestFunctionReference(0),
					StencilTestFunctionMask(0xFF),
					StencilMask(0xFF),
					StencilOperationStencilFailed(StencilOperations::Keep),
					StencilOperationDepthFailed(StencilOperations::Keep),
					StencilOperationDepthPassed(StencilOperations::Keep),
					PolygonMode(PolygonModes::Fill)
				{
				}

			public:
				TestFunctions StencilTestFunction;
				int32 StencilTestFunctionReference;
				uint32 StencilTestFunctionMask;
				uint32 StencilMask;
				StencilOperations StencilOperationStencilFailed;
				StencilOperations StencilOperationDepthFailed;
				StencilOperations StencilOperationDepthPassed;
				PolygonModes PolygonMode;
			};

		public:
			RenderState(void) :
				FaceOrder(FaceOrders::CounterClockwise),
				CullMode(CullModes::None),
				DepthTestFunction(TestFunctions::Less),
				BlendEquation(BlendEquations::Add),
				BlendFunctionSourceFactor(BlendFunctions::One),
				BlendFunctionDestinationFactor(BlendFunctions::Zero)
			{
			}

			void SetStencilTestFunction(TestFunctions Function, int32 Reference, uint32 Mask)
			{
				SetStencilTestFunction(CullModes::Front, Function, Reference, Mask);
				SetStencilTestFunction(CullModes::Back, Function, Reference, Mask);
				SetStencilTestFunction(CullModes::Both, Function, Reference, Mask);
			}

			void SetStencilTestFunction(CullModes CullMode, TestFunctions Function, int32 Reference, uint32 Mask)
			{
				if (BitwiseUtils::IsEnabled(CullMode, CullModes::Front))
				{
					FrontFaceState.StencilTestFunction = Function;
					FrontFaceState.StencilTestFunctionMask = Reference;
					FrontFaceState.StencilMask = Mask;
				}

				if (BitwiseUtils::IsEnabled(CullMode, CullModes::Back))
				{
					BackFaceState.StencilTestFunction = Function;
					BackFaceState.StencilTestFunctionMask = Reference;
					BackFaceState.StencilMask = Mask;
				}
			}

			void SetStencilOperation(StencilOperations StencilFailed, StencilOperations DepthFailed, StencilOperations DepthPassed)
			{
				SetStencilOperation(CullModes::Front, StencilFailed, DepthFailed, DepthPassed);
				SetStencilOperation(CullModes::Back, StencilFailed, DepthFailed, DepthPassed);
				SetStencilOperation(CullModes::Both, StencilFailed, DepthFailed, DepthPassed);
			}

			void SetStencilOperation(CullModes CullMode, StencilOperations StencilFailed, StencilOperations DepthFailed, StencilOperations DepthPassed)
			{
				if (BitwiseUtils::IsEnabled(CullMode, CullModes::Front))
				{
					FrontFaceState.StencilOperationStencilFailed = StencilFailed;
					FrontFaceState.StencilOperationDepthFailed = DepthFailed;
					FrontFaceState.StencilOperationDepthPassed = DepthPassed;
				}

				if (BitwiseUtils::IsEnabled(CullMode, CullModes::Back))
				{
					BackFaceState.StencilOperationStencilFailed = StencilFailed;
					BackFaceState.StencilOperationDepthFailed = DepthFailed;
					BackFaceState.StencilOperationDepthPassed = DepthPassed;
				}
			}

			void SetPolygonMode(PolygonModes Mode)
			{
				SetPolygonMode(CullModes::Front, Mode);
				SetPolygonMode(CullModes::Back, Mode);
				SetPolygonMode(CullModes::Both, Mode);
			}

			void SetPolygonMode(CullModes CullMode, PolygonModes Mode)
			{
				if (BitwiseUtils::IsEnabled(CullMode, CullModes::Front))
					FrontFaceState.PolygonMode = Mode;

				if (BitwiseUtils::IsEnabled(CullMode, CullModes::Back))
					BackFaceState.PolygonMode = Mode;
			}

			FaceState& GetFaceState(CullModes Mode)
			{
				switch (Mode)
				{
				case CullModes::Front:
					return FrontFaceState;

				case CullModes::Back:
					return BackFaceState;
				}

				return FrontFaceState;
			}

			const FaceState& GetFaceState(CullModes Mode) const
			{
				switch (Mode)
				{
				case CullModes::Front:
					return FrontFaceState;

				case CullModes::Back:
					return BackFaceState;
				}

				return FrontFaceState;
			}

		public:
			FaceOrders FaceOrder;
			CullModes CullMode;
			TestFunctions DepthTestFunction;
			BlendEquations BlendEquation;
			BlendFunctions BlendFunctionSourceFactor;
			BlendFunctions BlendFunctionDestinationFactor;
			FaceState FrontFaceState;
			FaceState BackFaceState;
		};
	}
}

#endif