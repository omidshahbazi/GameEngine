// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef I_DEVICE_H
#define I_DEVICE_H

#include <Containers\Color.h>
#include <Rendering\RenderContext.h>
#include <Rendering\TextureInfo.h>
#include <Rendering\RenderTargetInfo.h>
#include <Rendering\MeshInfo.h>
#include <Platform\PlatformWindow.h>
#include <Rendering\RenderTarget.h>
#include <Rendering\Program.h>
#include <Rendering\ConstantBuffer.h>
#include <Rendering\Mesh.h>
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
				AccumulationBuffer = 8,
				StencilBuffer = 16
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
				ProgramCompiler,
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

			virtual bool CreateContext(PlatformWindow::WindowHandle WindowHandle, RenderContext::Handle& Handle) = 0;
			virtual bool DestroyContext(RenderContext::Handle Handle) = 0;
			virtual bool SetContext(RenderContext::Handle Handle) = 0;

			virtual bool SetViewport(const Vector2I& Position, const Vector2I& Size) = 0;

			virtual bool SetClearColor(const ColorUI8& Color) = 0;

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
			virtual void SetState(const State& State) = 0;

			virtual bool SetResourceName(NativeType::Handle Handle, ResourceTypes Type, cwstr Name) = 0;

			virtual bool CreateBuffer(GPUBuffer::Handle& Handle) = 0;
			virtual	bool DestroyBuffer(GPUBuffer::Handle Handle) = 0;
			virtual bool InitializeConstantBuffer(GPUBuffer::Handle Handle, const byte* Data, uint32 Size) = 0;
			virtual bool CopyFromBufferToBuffer(GPUBuffer::Handle Handle, GPUBuffer::Handle FromHandle, uint32 Size) = 0;
			virtual bool CopyFromVertexToBuffer(GPUBuffer::Handle Handle, SubMesh::Handle FromMeshHandle, uint32 Size) = 0;
			virtual bool CopyFromBufferToVertex(GPUBuffer::Handle Handle, SubMesh::Handle ToMeshHandle, uint32 Size) = 0;
			virtual bool CopyFromIndexToBuffer(GPUBuffer::Handle Handle, SubMesh::Handle FromMeshHandle, uint32 Size) = 0;
			virtual bool CopyFromBufferToIndex(GPUBuffer::Handle Handle, SubMesh::Handle ToMeshHandle, uint32 Size) = 0;
			virtual bool CopyFromTextureToBuffer(GPUBuffer::Handle Handle, Texture::Handle FromTextureHandle, uint32 Size, Texture::Types TextureType, Formats TextureFormat, uint32 Level) = 0;
			virtual bool CopyFromBufferToTexture(GPUBuffer::Handle Handle, Texture::Handle ToTextureHandle, Texture::Types TextureType, uint32 Width, uint32 Height, Formats TextureFormat) = 0;
			virtual bool LockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type, GPUBuffer::Access Access, byte** Buffer) = 0;
			virtual	bool UnlockBuffer(GPUBuffer::Handle Handle, GPUBuffer::Types Type) = 0;

			virtual bool CompileProgram(const Shaders* Shaders, CompiledShaders* CompiledShaders, cstr* ErrorMessage) = 0;
			virtual bool CreateProgram(const CompiledShaders* Shaders, Program::Handle& Handle, cstr* ErrorMessage) = 0;
			virtual bool DestroyProgram(Program::Handle Handle) = 0;
			virtual bool BindProgram(Program::Handle Handle) = 0;
			virtual bool SetProgramConstantBuffer(Program::ConstantHandle Handle, ConstantBuffer::Handle Value) = 0;
			virtual bool SetProgramTexture(Program::ConstantHandle Handle, Texture::Types Type, Texture::Handle Value) = 0;

			virtual bool CreateTexture(const TextureInfo* Info, Texture::Handle& Handle) = 0;
			virtual bool DestroyTexture(Texture::Handle Handle) = 0;
			virtual bool BindTexture(Texture::Handle Handle, Texture::Types Type) = 0;
			virtual bool SetTextureVerticalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode) = 0;
			virtual bool SetTextureHorizontalWrapping(Texture::Handle Handle, Texture::Types Type, Texture::WrapModes Mode) = 0;
			virtual bool SetTextureMinifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MinifyFilters Filter) = 0;
			virtual bool SetTextureMagnifyFilter(Texture::Handle Handle, Texture::Types Type, Texture::MagnfyFilters Filter) = 0;
			virtual	bool GenerateTextureMipMap(Texture::Handle Handle, Texture::Types Type) = 0;

			virtual bool CreateRenderTarget(const RenderTargetInfo* Info, RenderTarget::Handle& Handle, TextureList& Textures) = 0;
			virtual bool DestroyRenderTarget(RenderTarget::Handle Handle) = 0;
			virtual bool BindRenderTarget(RenderTarget::Handle Handle) = 0;

			virtual bool CreateMesh(const SubMeshInfo* Info, SubMesh::Handle& Handle) = 0;
			virtual bool DestroyMesh(SubMesh::Handle Handle) = 0;
			virtual bool BindMesh(SubMesh::Handle Handle) = 0;

			virtual bool Clear(ClearFlags Flags) = 0;

			virtual bool DrawIndexed(SubMesh::PolygonTypes PolygonType, uint32 IndexCount) = 0;
			virtual bool DrawArray(SubMesh::PolygonTypes PolygonType, uint32 VertexCount) = 0;

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