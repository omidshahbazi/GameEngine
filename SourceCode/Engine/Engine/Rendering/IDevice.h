// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef I_DEVICE_H
#define I_DEVICE_H

#include <Rendering\MeshInfo.h>
#include <Platform\PlatformWindow.h>
#include <Rendering\Texture.h>
#include <Rendering\Program.h>
#include <Rendering\GPUBuffer.h>
#include <Rendering\Window.h>
#include <Rendering\Color.h>
#include <Rendering\Vertex.h>

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

			enum class DrawModes
			{
				Lines = 0,
				LineLoop,
				LineStrip,
				Triangles,
				TriangleStrip,
				TriangleFan,
				Quads,
				QuadStrip,
				Polygon
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

			enum class TextureFormats
			{
				R8 = 0,
				R16,
				R32,
				R8F,
				R16F,
				R32F,
				RG8,
				RG16,
				RG32,
				RGB8,
				RGB16,
				RGB32,
				RGBA8,
				RGBA16,
				RGBA32
			};

			enum class TextureWrapModes
			{
				Clamp = 0,
				Repeat,
				ClampToEdge,
				MirroredRepeat
			};

			enum class MinifyFilters
			{
				Nearest = 0,
				Linear,
				NearestMipMapNearest,
				LinearMipMapNearest,
				NearestMipMapLinear,
				LinearMipMapLinear
			};

			enum class MagnfyFilters
			{
				Nearest = 0,
				Linear
			};

			enum class PolygonModes
			{
				Point = 0,
				Line,
				Fill
			};

		public:
			virtual ~IDevice(void)
			{
			}

			virtual bool Initialize(void) = 0;

			virtual uint8 GetSampleCount(void) const = 0;
			virtual void SetSampleCount(uint8 Count) = 0;

			virtual bool GetForwardCompatible(void) const = 0;
			virtual void SetForwardCompatible(bool Value) = 0;

			virtual Color GetClearColor(void) const = 0;
			virtual void SetClearColor(Color Color) = 0;

			virtual ClearFlags GetClearFlags(void) const = 0;
			virtual void SetClearFlags(ClearFlags Flags) = 0;

			virtual FaceOrders GetFaceOrder(void) const = 0;
			virtual void SetFaceOrder(FaceOrders Order) = 0;

			virtual CullModes GetCullMode(void) const = 0;
			virtual void SetCullMode(CullModes Mode) = 0;

			virtual TestFunctions GetDepthTestFunction(void) const = 0;
			virtual void SetDepthTestFunction(TestFunctions Function) = 0;

			virtual void GetStencilTestFunction(TestFunctions &Function, int32 &Reference, uint32 &Mask) const = 0;
			virtual void SetStencilTestFunction(TestFunctions Function, int32 Reference, uint32 Mask) = 0;

			virtual uint32 GetStencilMask(void) const = 0;
			virtual void SetStencilMask(uint32 Mask) = 0;

			virtual void GetStencilOperation(StencilOperations &StencilFailed, StencilOperations &DepthFailed, StencilOperations &DepthPassed) const = 0;
			virtual void SetStencilOperation(StencilOperations StencilFailed, StencilOperations DepthFailed, StencilOperations DepthPassed) = 0;

			virtual void GetBlendFunction(BlendFunctions &SourceFactor, BlendFunctions &DestinationFactor) const = 0;
			virtual void SetBlendFunction(BlendFunctions SourceFactor, BlendFunctions DestinationFactor) = 0;

			virtual void GetPolygonMode(CullModes &CullMode, PolygonModes &PolygonMode) const = 0;
			virtual void SetPolygonMode(CullModes CullMode, PolygonModes PolygonMode) = 0;

			virtual bool CreateProgram(cstr VertexShader, cstr FragmentShader, Program::Handle &Handle) = 0;
			virtual bool DestroyProgram(Program::Handle Handle) = 0;
			virtual bool BindProgram(Program::Handle Handle) = 0;
			virtual	bool QueryProgramActiveConstants(Program::Handle Handle, Program::ConstantDataList &Constants) = 0;
			virtual bool GetProgramConstantHandle(Program::Handle Handle, const String &Name, Program::ConstantHandle &ConstantHandle) = 0;
			virtual bool SetProgramFloat32(Program::ConstantHandle Handle, float32 Value) = 0;
			virtual bool SetProgramVector2(Program::ConstantHandle Handle, const Vector2F &Value) = 0;
			virtual bool SetProgramVector3(Program::ConstantHandle Handle, const Vector3F &Value) = 0;
			virtual bool SetProgramVector4(Program::ConstantHandle Handle, const Vector4F &Value) = 0;
			virtual bool SetProgramMatrix4(Program::ConstantHandle Handle, const Matrix4F &Value) = 0;
			virtual bool SetProgramTexture(Program::ConstantHandle Handle, Texture::Handle Value) = 0;

			virtual bool CreateTexture2D(const byte *Data, uint32 Width, uint32 Height, uint8 ComponentCount, TextureFormats Format, Texture::Handle &Handle) = 0;
			virtual bool DestroyTexture(Texture::Handle Handle) = 0;
			virtual bool BindTexture2D(Program::Handle Handle) = 0;
			virtual bool SetTexture2DVerticalWrapping(Texture::Handle Handle, TextureWrapModes Mode) = 0;
			virtual bool SetTexture2DHorizontalWrapping(Texture::Handle Handle, TextureWrapModes Mode) = 0;
			virtual bool SetTexture2DMinifyFilter(Texture::Handle Handle, MinifyFilters Filter) = 0;
			virtual bool SetTexture2DMagnifyFilter(Texture::Handle Handle, MagnfyFilters Filter) = 0;

			virtual bool CreateMesh(const SubMeshInfo *Info, BufferUsages Usage, GPUBuffer::Handle &Handle) = 0;
			virtual bool DestroyMesh(GPUBuffer::Handle Handle) = 0;
			virtual bool BindBuffer(GPUBuffer::Handle Handle) = 0;

			virtual bool CreateWindow(uint16 Width, uint16 Height, cstr Title, Window::Handle &Handle) = 0;
			virtual bool DestroyWindow(Window::Handle Handle) = 0;

			virtual void Clear(void) = 0;

			virtual void Draw(DrawModes Mode, uint32 Count) = 0;

			virtual void SwapBuffers(Window::Handle Handle) = 0;

			virtual void PollEvents(void) = 0;
			virtual bool WindowShouldClose(Window::Handle Handle) = 0;

			virtual cstr GetLastError(void) const = 0;
		};
	}
}

#endif