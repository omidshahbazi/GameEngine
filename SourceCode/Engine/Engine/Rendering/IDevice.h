// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef I_DEVICE_H
#define I_DEVICE_H

#include <Rendering\RenderContext.h>
#include <Rendering\MeshInfo.h>
#include <Rendering\RenderTargetInfo.h>
#include <Platform\PlatformWindow.h>
#include <Rendering\RenderTarget.h>
#include <Rendering\Program.h>
#include <Rendering\GPUBuffer.h>
#include <Rendering\Color.h>
#include <Rendering\Vertex.h>
#include <Common\BitwiseUtils.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;
	using namespace Containers;

	namespace Rendering
	{
		class IDevice
		{
		public:
			enum class ClearFlags
			{
				ColorBuffer = 2,
				DepthBuffer = 4,
				AccumulationBuffer = 8,
				StencilBuffer = 16
			};

			enum class BufferUsages
			{
				StreamDraw = 0,
				StreamRead,
				StreamCopy,
				StaticDraw,
				StaticRead,
				StaticCopy,
				DynamicDraw,
				DynamicRead,
				DynamicCopy
			};

			enum class FaceOrders
			{
				Clockwise = 0,
				CounterClockwise
			};

			enum class CullModes
			{
				None = 2,
				Front = 4,
				Back = 8,
				Both = Front | Back
			};

			enum class TestFunctions
			{
				Never = 0,
				Less,
				LessEqual,
				Equal,
				NotEqual,
				GreaterEqual,
				Greater,
				Always
			};

			enum class StencilOperations
			{
				Keep = 0,
				Zero,
				Replace,
				Increament,
				IncreamentWrap,
				Decreament,
				DecreamentWrap,
				Invert
			};

			enum class BlendEquations
			{
				Add,
				Subtract,
				ReverseSubtract,
				Min,
				Max
			};

			enum class BlendFunctions
			{
				Zero = 0,
				One,
				SourceColor,
				OneMinusSourceColor,
				DestinationColor,
				OneMinusDestinationColor,
				SourceAlpha,
				OneMinusSourceAlpha,
				DestinationAlpha,
				OneMinusDestinationAlpha,
				ConstantColor,
				OneMinusConstantColor,
				ConstantAlpha,
				OneMinusConstantAlpha
			};

			enum class PolygonModes
			{
				Point = 0,
				Line,
				Fill
			};

			enum class DebugSources
			{
				All = 0,
				API,
				WindowSystem,
				ShaderCompiler,
				ThirdParty,
				Application,
				Other
			};

			enum class DebugTypes
			{
				All = 0,
				Error,
				DeprecatedBehavior,
				UndefinedBehavior,
				Portability,
				Performance,
				Marker,
				PushGroup,
				PopGroup,
				Other
			};

			enum class DebugSeverities
			{
				All = 0,
				Notification,
				Low,
				Medium,
				High
			};

			typedef std::function <void(int32 ID, DebugSources Source, cstr Message, DebugTypes Type, DebugSeverities Severity)> DebugProcedureType;

			struct State
			{
			public:
				struct FaceState
				{
				public:
					FaceState(void) :
						StencilTestFunction(TestFunctions::Always),
						StencilTestFunctionReference(0),
						StencilTestFunctionMask(0xFF),
						StencilMask(0),
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
				State(void) :
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
					switch (CullMode)
					{
					case CullModes::Front:
						FrontFaceState.StencilTestFunction = Function;
						FrontFaceState.StencilTestFunctionMask = Reference;
						FrontFaceState.StencilMask = Mask;
						break;
					case CullModes::Back:
						BackFaceStace.StencilTestFunction = Function;
						BackFaceStace.StencilTestFunctionMask = Reference;
						BackFaceStace.StencilMask = Mask;
						break;
					case CullModes::Both:
						BothFaceState.StencilTestFunction = Function;
						BothFaceState.StencilTestFunctionMask = Reference;
						BothFaceState.StencilMask = Mask;
						break;
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
					switch (CullMode)
					{
					case CullModes::Front:
						FrontFaceState.StencilOperationStencilFailed = StencilFailed;
						FrontFaceState.StencilOperationDepthFailed = DepthFailed;
						FrontFaceState.StencilOperationDepthPassed = DepthPassed;
						break;
					case CullModes::Back:
						BackFaceStace.StencilOperationStencilFailed = StencilFailed;
						BackFaceStace.StencilOperationDepthFailed = DepthFailed;
						BackFaceStace.StencilOperationDepthPassed = DepthPassed;
						break;
					case CullModes::Both:
						BothFaceState.StencilOperationStencilFailed = StencilFailed;
						BothFaceState.StencilOperationDepthFailed = DepthFailed;
						BothFaceState.StencilOperationDepthPassed = DepthPassed;
						break;
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
					switch (CullMode)
					{
					case CullModes::Front:
						FrontFaceState.PolygonMode = Mode;
						break;
					case CullModes::Back:
						BackFaceStace.PolygonMode = Mode;
						break;
					case CullModes::Both:
						BothFaceState.PolygonMode = Mode;
						break;
					}
				}

				FaceState& GetFaceState(CullModes Mode)
				{
					switch (Mode)
					{
					case CullModes::Front:
						return FrontFaceState;

					case CullModes::Back:
						return BackFaceStace;
					}

					return BothFaceState;
				}

				const FaceState& GetFaceState(CullModes Mode) const
				{
					switch (Mode)
					{
					case CullModes::Front:
						return FrontFaceState;

					case CullModes::Back:
						return BackFaceStace;
					}

					return BothFaceState;
				}

			public:
				FaceOrders FaceOrder;
				CullModes CullMode;
				TestFunctions DepthTestFunction;
				BlendEquations BlendEquation;
				BlendFunctions BlendFunctionSourceFactor;
				BlendFunctions BlendFunctionDestinationFactor;
				FaceState FrontFaceState;
				FaceState BackFaceStace;
				FaceState BothFaceState;
			};

			typedef Vector<Texture::Handle> TextureList;

		public:
			virtual ~IDevice(void)
			{
			}

			virtual bool Initialize(void) = 0;

			virtual cstr GetVersion(void) = 0;
			virtual cstr GetVendorName(void) = 0;
			virtual cstr GetRendererName(void) = 0;
			virtual cstr GetShadingLanguageVersion(void) = 0;

			virtual RenderContext* CreateContext(PlatformWindow::WindowHandle Handle) = 0;
			virtual bool DestroyContext(RenderContext* Context) = 0;
			virtual bool SetContext(RenderContext* Context) = 0;

			virtual bool SetViewport(const Vector2I& Position, const Vector2I& Size) = 0;

			virtual bool SetClearColor(Color Color) = 0;

			virtual bool SetFaceOrder(FaceOrders Order) = 0;

			virtual bool SetCullMode(CullModes Mode) = 0;

			virtual bool SetDepthTestFunction(TestFunctions Function) = 0;

			virtual bool SetStencilTestFunction(CullModes CullMode, TestFunctions Function, int32 Reference, uint32 Mask) = 0;

			virtual bool SetStencilMask(CullModes CullMode, uint32 Mask) = 0;

			virtual bool SetStencilOperation(CullModes CullMode, StencilOperations StencilFailed, StencilOperations DepthFailed, StencilOperations DepthPassed) = 0;

			virtual bool SetBlendEquation(BlendEquations Equation) = 0;

			virtual bool SetBlendFunction(BlendFunctions SourceFactor, BlendFunctions DestinationFactor) = 0;

			virtual bool SetPolygonMode(CullModes CullMode, PolygonModes PolygonMode) = 0;

			virtual bool ResetState(void) = 0;
			virtual const State& GetState(void) const = 0;
			virtual void SetState(const State& Stae) = 0;

			virtual bool CreateProgram(cstr VertexShader, cstr FragmentShader, Program::Handle& Handle) = 0;
			virtual bool DestroyProgram(Program::Handle Handle) = 0;
			virtual bool BindProgram(Program::Handle Handle) = 0;
			virtual	bool QueryProgramActiveConstants(Program::Handle Handle, Program::ConstantDataList& Constants) = 0;
			virtual bool GetProgramConstantHandle(Program::Handle Handle, const String& Name, Program::ConstantHandle& ConstantHandle) = 0;
			virtual bool SetProgramFloat32(Program::ConstantHandle Handle, float32 Value) = 0;
			virtual bool SetProgramVector2(Program::ConstantHandle Handle, const Vector2F& Value) = 0;
			virtual bool SetProgramVector3(Program::ConstantHandle Handle, const Vector3F& Value) = 0;
			virtual bool SetProgramVector4(Program::ConstantHandle Handle, const Vector4F& Value) = 0;
			virtual bool SetProgramMatrix4(Program::ConstantHandle Handle, const Matrix4F& Value) = 0;
			virtual bool SetProgramTexture(Program::ConstantHandle Handle, Texture::Handle Value) = 0;

			virtual bool CreateTexture2D(const byte* Data, uint32 Width, uint32 Height, Texture::Formats Format, Texture::Handle& Handle) = 0;
			virtual bool DestroyTexture(Texture::Handle Handle) = 0;
			virtual bool BindTexture2D(Program::Handle Handle) = 0;
			virtual bool SetTexture2DVerticalWrapping(Texture::Handle Handle, Texture::WrapModes Mode) = 0;
			virtual bool SetTexture2DHorizontalWrapping(Texture::Handle Handle, Texture::WrapModes Mode) = 0;
			virtual bool SetTexture2DMinifyFilter(Texture::Handle Handle, Texture::MinifyFilters Filter) = 0;
			virtual bool SetTexture2DMagnifyFilter(Texture::Handle Handle, Texture::MagnfyFilters Filter) = 0;
			virtual	bool GenerateMipMap(Texture::Handle Handle) = 0;

			virtual bool CreateRenderTarget(const RenderTargetInfo* Info, RenderTarget::Handle& Handle, TextureList& Textures) = 0;
			virtual bool DestroyRenderTarget(RenderTarget::Handle Handle) = 0;
			virtual bool BindRenderTarget(RenderTarget::Handle Handle) = 0;

			virtual bool CreateMesh(const SubMeshInfo* Info, BufferUsages Usage, GPUBuffer::Handle& Handle) = 0;
			virtual bool DestroyMesh(GPUBuffer::Handle Handle) = 0;
			virtual bool BindMesh(GPUBuffer::Handle Handle) = 0;

			virtual bool Clear(ClearFlags Flags) = 0;

			virtual bool DrawIndexed(Mesh::SubMesh::PolygonTypes PolygonType, uint32 IndexCount) = 0;
			virtual bool DrawArray(Mesh::SubMesh::PolygonTypes PolygonType, uint32 VertexCount) = 0;

			virtual bool SwapBuffers(void) = 0;

			virtual bool SetDebugCallback(DebugProcedureType Callback) = 0;
		};
	}
}

#endif