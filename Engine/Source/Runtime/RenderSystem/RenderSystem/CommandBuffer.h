// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef COMMAND_BUFFER_H
#define COMMAND_BUFFER_H

#include <RenderDevice\ICommandBuffer.h>
#include <Containers\Strings.h>
#include <RenderCommon\CommandBufferContainer.h>
#include <RenderCommon\RenderState.h>

namespace Engine
{
	using namespace Containers;
	using namespace MathContainers;
	using namespace RenderCommon;
	using namespace RenderDevice;

	namespace RenderSystem
	{
		namespace Private
		{
			class FrameConstantBuffers;
		}

		using namespace Private;

		class CommandBufferFence;
		class RenderContext;
		class RenderTarget;
		class Mesh;
		class Material;
		class Texture;

		class RENDERSYSTEM_API CommandBuffer
		{
			friend class DeviceInterface;

		public:
			CommandBuffer(void) :
				CommandBuffer("Command Buffer")
			{
			}

			CommandBuffer(const String& Name);

			String GetName(void) const
			{
				return m_Name.ChangeType<char8>();
			}
			void SetName(const String& Name)
			{
				m_Name = Name.ChangeType<char16>();
			}

			void Clear(void)
			{
				m_Buffer.Clear();
			}

			INLINE uint32 GetSize(void)
			{
				return m_Buffer.GetSize();
			}

			bool CopyTexture(const Texture* Source, Texture* Destination);
			bool CopyTexture(const Texture* Source, Texture* Destination, const Vector2I& Position, const Vector2I& Size);
			bool CopyTexture(const Texture* Source, const Vector2I& SourcePosition, Texture* Destination, const Vector2I& DestinationPosition, const Vector2I& Size);

			bool BlitMaterial(const Material* Material, RenderTargets RenderTarget);
			bool BlitMaterial(const Material* Material, RenderTarget* RenderTarget);

			bool GenerateMipMap(Texture* Texture);

			bool SetRenderTarget(RenderTargets RenderTarget);
			bool SetRenderTarget(RenderTarget* RenderTarget);
			bool SetViewport(const Vector2I& Position, const Vector2I& Size);
			void Clear(ClearFlags Flags, const ColorUI8& Color);

			bool DrawMesh(const Mesh* Mesh, const Matrix4F& Transform, const Material* Material);
			bool DrawMesh(const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Material* Material);
			bool DrawMesh(const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material);

			void BeginEvent(const String& Label);
			void BeginEvent(const WString& Label);
			void EndEvent(void);

			void SetMarker(const String& Label);
			void SetMarker(const WString& Label);

			bool WaitForFence(CommandBufferFence* const Fence)
			{
				return WaitForFences(&Fence, 1);
			}
			bool WaitForFences(CommandBufferFence* const* Fences, uint16 Count);

			bool SignalFence(CommandBufferFence* const Fence)
			{
				return SignalFences(&Fence, 1);
			}
			bool SignalFences(CommandBufferFence* const* Fences, uint16 Count);

			//Dispatch

			//UNDONE:RENDERING -> Features
			//BlitTexture -> to render target
			//RenderTargetPool

			//private void RenderSourceToMask(ScriptableRenderContext context, ILOSSource losSource, RenderTexture normaldepthBuffer, ref RenderTexture maskRenderTexture)
			//{
			//	Camera sourceCamera1 = losSource.SourceCamera;

			//	if (sourceCamera1 == null || sourceCamera1.cameraType != CameraType.Game) return;

			//	context.SetupCameraProperties(sourceCamera1);

			//	sourceCamera1.TryGetCullingParameters(out ScriptableCullingParameters cullingParameters);
			//	CullingResults cullingResults = context.Cull(ref cullingParameters);

			//	DrawingSettings drawingSettings = new DrawingSettings();

			//	drawingSettings.SetShaderPassName(0, new ShaderTagId("DepthOnly"));
			//	drawingSettings.SetShaderPassName(1, new ShaderTagId("HRPDefaultUnlit"));
			//	drawingSettings.SetShaderPassName(2, new ShaderTagId("Vertex"));

			//	// Set "skybox" material farplane.
			//	Materials.SkyBox.SetVector(ShaderID.FarPlane, new Vector4(sourceCamera1.farClipPlane, sourceCamera1.farClipPlane, sourceCamera1.farClipPlane, sourceCamera1.farClipPlane));

			//	// Create temporary rendertexture for rendering linear depth.

			//	RenderTexture sourceBuffer = CreateDepthsRenderTexture();
			//	CommandBuffer cmd = new CommandBuffer();

			//	cmd.SetRenderTarget(new RenderTargetIdentifier(sourceBuffer));

			//	context.ExecuteCommandBuffer(cmd);

			//	drawingSettings.overrideMaterial = Materials.Depth;

			//	FilteringSettings filteringSettings = new FilteringSettings(RenderQueueRange.all);
			//	context.DrawRenderers(cullingResults, ref drawingSettings, ref filteringSettings);

			//	context.DrawSkybox(sourceCamera1);

			//	context.Submit();

			//	RenderTexture.active = null;

			//	//Push LOS source specific parameters.
			//	Materials.Mask.SetTexture(ShaderID.SourceDepthTex, sourceBuffer);
			//	Materials.Mask.SetTexture(ShaderID.CameraNormalDepthTex, normaldepthBuffer);
			//	Materials.Mask.SetMatrix(ShaderID.SourceWorldProj, sourceCamera1.projectionMatrix * sourceCamera1.worldToCameraMatrix);
			//	Materials.Mask.SetVector(ShaderID.SourceInfo, losSource.SourceInfo);
			//	Materials.Mask.SetVector(ShaderID.Settings, new Vector4(losSource.DistanceFade, losSource.EdgeFade, losSource.MinVariance, losSource.BackfacesFade));
			//	Materials.Mask.SetVector(ShaderID.Flags, new Vector4(PixelOperation.Clamp == losSource.OutOfBoundArea ? 0.0f : 1.0f, PixelOperation.Exclude == losSource.OutOfBoundArea ? -1.0f : 1.0f, losSource.MaskInvert ? 1.0f : 0.0f, losSource.ExcludeBackfaces ? 1.0f : 0.0f));
			//	Materials.Mask.SetColor(ShaderID.ColorMask, losSource.MaskColor * losSource.MaskIntensity);

			//	// Set Correct material pass.
			//	Materials.Mask.SetPass(0);

			//	// Render Mask.
			//	IndexedGraphicsBlit(maskRenderTexture);

			//	// Release linear depth render texture.
			//	RenderTexture.ReleaseTemporary(sourceBuffer);

		private:
			bool PrepareNativeBuffers(ICommandBuffer* CommandBuffer, FrameConstantBuffers* ConstantBuffers, Texture* DefaultTexture, const RenderContext* RenderContext);

			static void InsertDrawCommand(ICommandBuffer* CommandBuffer, FrameConstantBuffers* ConstantBuffers, Texture* DefaultTexture, const Mesh* Mesh, const Matrix4F& Model, const Matrix4F& View, const Matrix4F& Projection, const Matrix4F& MVP, const Material* Material);

		private:
			WString m_Name;
			CommandBufferContainer m_Buffer;
		};
	}
}

#endif