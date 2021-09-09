// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef I_DEVICE_H
#define I_DEVICE_H

#include <Containers\Color.h>
#include <RenderCommon\RenderCommon.h>
#include <RenderCommon\Enums.h>
#include <RenderDevice\TextureInfo.h>
#include <RenderDevice\RenderTargetInfo.h>
#include <RenderDevice\MeshInfo.h>
#include <Platform\PlatformWindow.h>
#include <Common\BitwiseUtils.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;
	using namespace Containers;
	using namespace RenderCommon;

	namespace RenderDevice
	{
		class IDevice
		{
		public:
			enum class ResourceTypes
			{
				Buffer = 0,
				Program,
				Mesh,
				Texture,
				RenderTarget
			};

			enum class ClearFlags
			{
				ColorBuffer = 2,
				DepthBuffer = 4,
				StencilBuffer = 8
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
				API = 0,
				WindowSystem,
				ProgramCompiler,
				ThirdParty,
				Application,
				Other
			};

			enum class DebugTypes
			{
				Error = 0,
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
				Notification = 0,
				Low,
				Medium,
				High
			};

			typedef std::function<void(int32 ID, DebugSources Source, cstr Message, DebugTypes Type, DebugSeverities Severity)> DebugFunction;

			struct Shaders
			{
			public:
				cstr VertexShader;
				cstr TessellationShader;
				cstr GeometryShader;
				cstr FragmentShader;
				cstr ComputeShader;
			};

			struct CompiledShaders
			{
			public:
				struct CompiledShader
				{
					byte* Buffer;
					uint16 Size;
				};

			public:
				CompiledShader VertexShader;
				CompiledShader TessellationShader;
				CompiledShader GeometryShader;
				CompiledShader FragmentShader;
				CompiledShader ComputeShader;
			};

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
					if (BitwiseUtils::IsEnabled(CullMode, IDevice::CullModes::Front))
					{
						FrontFaceState.StencilTestFunction = Function;
						FrontFaceState.StencilTestFunctionMask = Reference;
						FrontFaceState.StencilMask = Mask;
					}

					if (BitwiseUtils::IsEnabled(CullMode, IDevice::CullModes::Back))
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
					if (BitwiseUtils::IsEnabled(CullMode, IDevice::CullModes::Front))
					{
						FrontFaceState.StencilOperationStencilFailed = StencilFailed;
						FrontFaceState.StencilOperationDepthFailed = DepthFailed;
						FrontFaceState.StencilOperationDepthPassed = DepthPassed;
					}

					if (BitwiseUtils::IsEnabled(CullMode, IDevice::CullModes::Back))
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
					if (BitwiseUtils::IsEnabled(CullMode, IDevice::CullModes::Front))
						FrontFaceState.PolygonMode = Mode;

					if (BitwiseUtils::IsEnabled(CullMode, IDevice::CullModes::Back))
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

			typedef Vector<ResourceHandle> TextureList;

		public:
			virtual ~IDevice(void)
			{
			}

			virtual bool Initialize(void) = 0;
			virtual bool Uninitialize(void) = 0;

			virtual cstr GetVersion(void) = 0;
			virtual cstr GetVendorName(void) = 0;
			virtual cstr GetRendererName(void) = 0;
			virtual cstr GetShadingLanguageVersion(void) = 0;

			virtual bool CreateContext(PlatformWindow::WindowHandle WindowHandle, ResourceHandle& Handle) = 0;
			virtual bool DestroyContext(ResourceHandle Handle) = 0;
			virtual bool SetContext(ResourceHandle Handle) = 0;
			virtual bool SetContextSize(const Vector2I& Size) = 0;

			virtual bool SetViewport(const Vector2I& Position, const Vector2I& Size) = 0;

			virtual bool ResetState(void) = 0;
			virtual const State& GetState(void) const = 0;
			virtual void SetState(const State& State) = 0;

			virtual bool SetResourceName(ResourceHandle Handle, ResourceTypes Type, cwstr Name) = 0;

			virtual bool CreateBuffer(ResourceHandle& Handle) = 0;
			virtual	bool DestroyBuffer(ResourceHandle Handle) = 0;
			virtual bool InitializeConstantBuffer(ResourceHandle Handle, const byte* Data, uint32 Size) = 0;
			virtual bool CopyFromVertexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size) = 0;
			virtual bool CopyFromBufferToVertex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size) = 0;
			virtual bool CopyFromIndexToBuffer(ResourceHandle Handle, ResourceHandle FromMeshHandle, uint32 Size) = 0;
			virtual bool CopyFromBufferToIndex(ResourceHandle Handle, ResourceHandle ToMeshHandle, uint32 Size) = 0;
			virtual bool CopyFromTextureToBuffer(ResourceHandle Handle, ResourceHandle FromTextureHandle, uint32 Size, TextureTypes TextureType, Formats TextureFormat, uint32 Level) = 0;
			virtual bool CopyFromBufferToTexture(ResourceHandle Handle, ResourceHandle ToTextureHandle, TextureTypes TextureType, uint32 Width, uint32 Height, Formats TextureFormat) = 0;
			virtual bool LockBuffer(ResourceHandle Handle, GPUBufferTypes Type, GPUBufferAccess Access, byte** Buffer) = 0;
			virtual	bool UnlockBuffer(ResourceHandle Handle, GPUBufferTypes Type) = 0;

			virtual bool CreateProgram(const CompiledShaders* Shaders, ResourceHandle& Handle, cstr* ErrorMessage) = 0;
			virtual bool DestroyProgram(ResourceHandle Handle) = 0;
			virtual bool BindProgram(ResourceHandle Handle) = 0;
			virtual bool SetProgramConstantBuffer(ProgramConstantHandle Handle, ResourceHandle Value) = 0;
			virtual bool SetProgramTexture(ProgramConstantHandle Handle, TextureTypes Type, ResourceHandle Value) = 0;

			virtual bool CreateTexture(const TextureInfo* Info, ResourceHandle& Handle) = 0;
			virtual bool DestroyTexture(ResourceHandle Handle) = 0;
			virtual bool SetTextureVerticalWrapping(ResourceHandle Handle, TextureTypes Type, TextureWrapModes Mode) = 0;
			virtual bool SetTextureHorizontalWrapping(ResourceHandle Handle, TextureTypes Type, TextureWrapModes Mode) = 0;
			virtual bool SetTextureMinifyFilter(ResourceHandle Handle, TextureTypes Type, TextureMinifyFilters Filter) = 0;
			virtual bool SetTextureMagnifyFilter(ResourceHandle Handle, TextureTypes Type, TextureMagnfyFilters Filter) = 0;
			virtual	bool GenerateTextureMipMap(ResourceHandle Handle, TextureTypes Type) = 0;

			virtual bool CreateRenderTarget(const RenderTargetInfo* Info, ResourceHandle& Handle, TextureList& Textures) = 0;
			virtual bool DestroyRenderTarget(ResourceHandle Handle) = 0;
			virtual bool BindRenderTarget(ResourceHandle Handle) = 0;

			virtual bool CreateMesh(const SubMeshInfo* Info, ResourceHandle& Handle) = 0;
			virtual bool DestroyMesh(ResourceHandle Handle) = 0;
			virtual bool BindMesh(ResourceHandle Handle) = 0;

			virtual bool Clear(ClearFlags Flags, const ColorUI8& Color) = 0;

			virtual bool DrawIndexed(PolygonTypes PolygonType, uint32 IndexCount) = 0;
			virtual bool DrawArray(PolygonTypes PolygonType, uint32 VertexCount) = 0;

			virtual bool BeginExecute(void) = 0;
			virtual bool EndExecute(void) = 0;

			virtual bool SwapBuffers(void) = 0;

			virtual bool BeginEvent(cwstr Label) = 0;
			virtual bool EndEvent(void) = 0;
			virtual bool SetMarker(cwstr Label) = 0;

			virtual bool SetDebugCallback(DebugFunction Callback) = 0;
		};
	}
}

#endif