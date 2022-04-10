// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DeferredPipeline\Private\DeferredRendering.h>
#include <RenderDevice\ProgramInfo.h>
#include <RenderSystem\DeviceInterface.h>
#include <RenderSystem\RenderTargetPool.h>
#include <RenderSystem\RenderContext.h>
#include <RenderSystem\Material.h>
#include <RenderSystem\RenderTarget.h>
#include <BuiltInResources\BuiltInResourceManager.h>

namespace Engine
{
	using namespace RenderDevice;
	using namespace BuiltInResources;

	namespace DeferredPipeline
	{
		namespace Private
		{
			DeferredRendering::DeferredRendering(void) :
				m_DeviceInterface(nullptr),
				m_AmbientLightProgram(nullptr),
				m_DirectionalLightProgram(nullptr),
				m_PointLightProgram(nullptr),
				m_SpotLightProgram(nullptr),
				m_ActiveInfo(nullptr),
				m_CommandBufferGBuffer("GBuffer Pass"),
				m_CommandBufferLighting("Lighting Pass"),
				m_CommandBufferFinal("Final Pass")
			{
			}

			DeferredRendering::~DeferredRendering(void)
			{
			}

			void DeferredRendering::Initialize(DeviceInterface* DeviceInterface)
			{
				m_DeviceInterface = DeviceInterface;

				auto builtInResourceManager = BuiltInResourceManager::GetInstance();
				m_AmbientLightProgram = builtInResourceManager->Load<Program>("Programs/Pipelines/Deferred/AmbientLight.program");
				m_DirectionalLightProgram = builtInResourceManager->Load<Program>("Programs/Pipelines/Deferred/DirectionalLight.program");
				m_PointLightProgram = builtInResourceManager->Load<Program>("Programs/Pipelines/Deferred/PointLight.program");
				m_SpotLightProgram = builtInResourceManager->Load<Program>("Programs/Pipelines/Deferred/SpotLight.program");

				m_DeviceInterface->OnContextChangedEvent += EventListener_OnContextChanged;
				m_DeviceInterface->OnContextResizedEvent += EventListener_OnContextResized;
			}

			void DeferredRendering::Uninitialize(void)
			{
				m_DeviceInterface->OnContextChangedEvent -= EventListener_OnContextChanged;
				m_DeviceInterface->OnContextResizedEvent -= EventListener_OnContextResized;

				for (auto& info : m_RenderTargets)
					m_DeviceInterface->DestroyRenderTarget(info.GetSecond().RenderTarget);
			}

			void DeferredRendering::BeginRender(void)
			{
				if (m_ActiveInfo == nullptr)
					return;

				m_CommandBufferGBuffer.Clear();
				m_CommandBufferGBuffer.SetRenderTarget(m_ActiveInfo->RenderTarget);
				m_CommandBufferGBuffer.Clear(ClearFlags::ColorBuffer | ClearFlags::DepthBuffer, ColorUI8::Black);

				m_CommandBufferLighting.Clear();
				m_CommandBufferLighting.SetRenderTarget(RenderTargets::Context);

				m_CommandBufferFinal.Clear();
				m_CommandBufferFinal.SetRenderTarget(RenderTargets::Context);
				m_CommandBufferFinal.Clear(ClearFlags::ColorBuffer | ClearFlags::DepthBuffer, ColorUI8::Black);
			}

			void DeferredRendering::EndRender(void)
			{
				m_DeviceInterface->SubmitCommandBuffer(&m_CommandBufferGBuffer);
				m_DeviceInterface->SubmitCommandBuffer(&m_CommandBufferLighting);
				m_DeviceInterface->SubmitCommandBuffer(&m_CommandBufferFinal);
			}

			void DeferredRendering::OnContextChanged(const RenderContext* Context)
			{
				if (Context == nullptr)
					return;

				if (m_RenderTargets.Contains(Context))
				{
					m_ActiveInfo = &m_RenderTargets[Context];
					return;
				}

				m_RenderTargets[Context] = {};
				m_ActiveInfo = &m_RenderTargets[Context];
			}

			void DeferredRendering::OnContextResized(const Vector2I& Size)
			{
				ContextRenderTargetInfo& info = *m_ActiveInfo;

				if (info.Size == Size)
					return;

				if (info.RenderTarget != nullptr)
					RenderTargetPool::GetInstance()->Back(info.RenderTarget);

				info.Size = Size;

				RenderTargetInfo gbuffer;

				RenderTextureInfo tex0;
				tex0.Format = Formats::RGB32F;
				tex0.Point = AttachmentPoints::Color0;
				tex0.Dimension = { info.Size.X,info.Size.Y };
				gbuffer.Textures.Add(tex0);

				RenderTextureInfo tex1;
				tex1.Format = Formats::RGB16F;
				tex1.Point = AttachmentPoints::Color1;
				tex1.Dimension = { info.Size.X,info.Size.Y };
				gbuffer.Textures.Add(tex1);

				RenderTextureInfo tex2;
				tex2.Format = Formats::RGBA8;
				tex2.Point = AttachmentPoints::Color2;
				tex2.Dimension = { info.Size.X,info.Size.Y };
				gbuffer.Textures.Add(tex2);

				RenderTextureInfo depthTex;
				depthTex.Format = Formats::Depth24;
				depthTex.Point = AttachmentPoints::Depth;
				depthTex.Dimension = { info.Size.X,info.Size.Y };
				gbuffer.Textures.Add(depthTex);

				info.RenderTarget = RenderTargetPool::GetInstance()->Get(&gbuffer);
				info.RenderTarget->SetName(L"GBuffer");
				info.PositionTexture = TextureResource((*info.RenderTarget)[0]);
				info.NormalTexture = TextureResource((*info.RenderTarget)[1]);
				info.AlbedoSpecularTexture = TextureResource((*info.RenderTarget)[2]);
			}

			void DeferredRendering::SetPassConstants(Material* Material)
			{
				static const ProgramConstantHash ConstantHash_PositionTexture = Material::GetHash("PositionTexture");
				static const ProgramConstantHash ConstantHash_NormalTexture = Material::GetHash("NormalTexture");
				static const ProgramConstantHash ConstantHash_AlbedoSpecTexture = Material::GetHash("AlbedoSpecTexture");

				Material->SetTexture(ConstantHash_PositionTexture, &m_ActiveInfo->PositionTexture);
				Material->SetTexture(ConstantHash_NormalTexture, &m_ActiveInfo->NormalTexture);
				Material->SetTexture(ConstantHash_AlbedoSpecTexture, &m_ActiveInfo->AlbedoSpecularTexture);
			}
		}
	}
}