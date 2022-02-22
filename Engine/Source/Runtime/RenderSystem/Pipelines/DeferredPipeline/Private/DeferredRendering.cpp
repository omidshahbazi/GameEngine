// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <DeferredPipeline\Private\DeferredRendering.h>
#include <RenderDevice\ProgramInfo.h>
#include <RenderSystem\DeviceInterface.h>
#include <RenderSystem\RenderTargetPool.h>
#include <RenderSystem\RenderContext.h>
#include <RenderSystem\Material.h>
#include <RenderSystem\RenderTarget.h>

namespace Engine
{
	using namespace RenderDevice;

	namespace DeferredPipeline
	{
		namespace Private
		{
			cstr AmbientLightShader =
				"struct InputData { float3 Position : POSITION; float2 UV : TEXCOORD; };"
				"struct TransformData { matrix4 Model; matrix4 View; matrix4 Projection; matrix4 MVP; };"
				"struct Data { float3 WorldPosition; float3 ViewPosition; float4 Color; float Strength; float Radius; float ConstantAttenuation; float LinearAttenuation; float QuadraticAttenuation; float InnerCutOff; float OuterCutOff; float3 Direction; };"
				"texture2D AlbedoSpecTexture;"
				"TransformData _TransformData;"
				"Data data;"
				"float4 VertexMain(InputData inputData)"
				"{"
				"	matrix4 mat = matrix4("
				"		1, 0, 0, 0,"
				"		0, 1, 0, 0,"
				"		0, 0, 1, 0,"
				"		0, 0, 0, 1);"
				"	return mat * float4(inputData.Position, 1);"
				"}"
				"float4 FragmentMain(InputData inputData)"
				"{"
				"	float3 diffuse = Sample(AlbedoSpecTexture, inputData.UV).rgb;"
				"	return float4(diffuse * data.Color.rgb * data.Strength, 1);"
				"}";

			cstr DirectionalLightShader =
				"struct InputData { float3 Position : POSITION; float2 UV : TEXCOORD; };"
				"struct TransformData { matrix4 Model; matrix4 View; matrix4 Projection; matrix4 MVP; };"
				"struct Data { float3 WorldPosition; float3 ViewPosition; float4 Color; float Strength; float Radius; float ConstantAttenuation; float LinearAttenuation; float QuadraticAttenuation; float InnerCutOff; float OuterCutOff; float3 Direction; };"
				"texture2D NormalTexture;"
				"texture2D AlbedoSpecTexture;"
				"TransformData _TransformData;"
				"Data data;"
				"float4 VertexMain(InputData inputData)"
				"{"
				"	matrix4 mat = matrix4("
				"		1, 0, 0, 0,"
				"		0, 1, 0, 0,"
				"		0, 0, 1, 0,"
				"		0, 0, 0, 1);"
				"	return mat * float4(inputData.Position, 1);"
				"}"
				"float4 FragmentMain(InputData inputData)"
				"{"
				"	float3 dir = Normalize(-data.Direction);"
				"	float3 normal = Sample(NormalTexture, inputData.UV).rgb;"
				"	float angle = Max(Dot(normal, dir), 0.0);"
				"	float3 diffuse = Sample(AlbedoSpecTexture, inputData.UV).rgb;"
				"	return float4(diffuse * data.Color.rgb * data.Strength * angle, 1);"
				"}";

			cstr PointLightShader =
				"struct InputData { float3 Position : POSITION; };"
				"struct TransformData { matrix4 Model; matrix4 View; matrix4 Projection; matrix4 MVP; };"
				"struct ViewportData { float2 FrameSize; };"
				"struct Data { float3 WorldPosition; float3 ViewPosition; float4 Color; float Strength; float Radius; float ConstantAttenuation; float LinearAttenuation; float QuadraticAttenuation; float InnerCutOff; float OuterCutOff; float3 Direction; };"
				"texture2D PositionTexture;"
				"texture2D NormalTexture;"
				"texture2D AlbedoSpecTexture;"
				"TransformData _TransformData;"
				"ViewportData _ViewportData;"
				"Data data;"
				"float4 VertexMain(InputData inputData)"
				"{"
				"	matrix4 mvp = _TransformData.MVP;"
				"	matrix4 scaleMat = matrix4("
				"		data.Radius * 2, 0, 0, 0,"
				"		0, data.Radius * 2, 0, 0,"
				"		0, 0, data.Radius * 2, 0,"
				"		0, 0, 0, 1);"
				"	mvp *= scaleMat;"
				"	return mvp * float4(inputData.Position, 1);"
				"}"
				"float4 FragmentMain(InputData inputData)"
				"{"
				"	float2 uv = _FragPosition / _ViewportData.FrameSize;"
				"	float3 gPos = Sample(PositionTexture, uv).rgb;"
				"	float distance = Length(data.WorldPosition - gPos);"
				""
				"	float3 result = float3(0,0,0);"
				"	if (distance < data.Radius)"
				"	{"
				"		float4 gAlbedoSpec = Sample(AlbedoSpecTexture, uv);"
				"		float3 gDiffuse = gAlbedoSpec.rgb;"
				"		float gSpecular = gAlbedoSpec.a;"
				""
				"		float3 toLightDir = Normalize(data.WorldPosition - gPos);"
				"		float3 normal = Sample(NormalTexture, uv).rgb;"
				""
				"		float3 diffuse = Max(Dot(normal, toLightDir), 0) * data.Color.rgb * gDiffuse * data.Strength;"
				""
				"		float3 viewDir = Normalize(data.ViewPosition - gPos);"
				"		float3 halfwayDir = Normalize(toLightDir + viewDir);"
				"		float3 specular = Pow(Max(Dot(normal, halfwayDir), 0), 16) * data.Color.rgb * gSpecular;"
				""
				"		float attenuation = 1 / (data.ConstantAttenuation + (data.LinearAttenuation * distance) + (data.QuadraticAttenuation * distance * distance));"
				""
				"		diffuse *= attenuation;"
				"		specular *= attenuation;"
				"		result = diffuse + specular;"
				"	}"
				"	return float4(result, 1);"
				"}";

			cstr SpotLightShader =
				"struct InputData { float3 Position : POSITION; };"
				"struct TransformData { matrix4 Model; matrix4 View; matrix4 Projection; matrix4 MVP; };"
				"struct ViewportData { float2 FrameSize; };"
				"struct AutoData { matrix4 MVP; float2 FrameSize; };"
				"struct Data { float3 WorldPosition; float3 ViewPosition; float4 Color; float Strength; float Radius; float ConstantAttenuation; float LinearAttenuation; float QuadraticAttenuation; float InnerCutOff; float OuterCutOff; float3 Direction; };"
				"texture2D PositionTexture;"
				"texture2D NormalTexture;"
				"texture2D AlbedoSpecTexture;"
				"TransformData _TransformData;"
				"ViewportData _ViewportData;"
				"Data data;"
				"float4 VertexMain(InputData inputData)"
				"{"
				"	matrix4 mvp = _TransformData.MVP;"
				"	float coneRadius = Sin(data.OuterCutOff / 2) * (data.Radius / Cos(data.OuterCutOff / 2));"
				"	matrix4 scaleMat = matrix4("
				"		coneRadius * 2, 0, 0, 0,"
				"		0, coneRadius * 2, 0, 0,"
				"		0, 0, data.Radius, 0,"
				"		0, 0, 0, 1);"
				"	mvp *= scaleMat;"
				"	return mvp * float4(inputData.Position, 1);"
				"}"
				""
				"float4 FragmentMain(InputData inputData)"
				"{"
				"	float2 uv = _FragPosition / _ViewportData.FrameSize;"
				"	float3 gPos = Sample(PositionTexture, uv).rgb;"
				"	float3 toFragDir = Normalize(gPos - data.WorldPosition);"
				"	float theta = ACos(Dot(toFragDir, data.Direction) / Length(toFragDir) * Length(data.Direction));"
				"	float halfOuterCutOff = data.OuterCutOff / 2;"
				""
				"	float3 result = float3(0,0,0);"
				"	if (theta <= halfOuterCutOff)"
				"	{"
				"		float halfInnerCutOff = data.InnerCutOff / 2;"
				"		float4 gAlbedoSpec = Sample(AlbedoSpecTexture, uv);"
				"		float3 gDiffuse = gAlbedoSpec.rgb;"
				"		float gSpecular = gAlbedoSpec.a;"
				"		float3 gNormal = Sample(NormalTexture, uv).rgb;"
				""
				"		float3 toLightDir = Normalize(data.WorldPosition - gPos);"
				""
				"		float3 diffuse = Max(Dot(gNormal, toLightDir), 0) * data.Color.rgb * gDiffuse * data.Strength;"
				""
				"		float3 toViewDir = Normalize(data.ViewPosition - gPos);"
				"		float3 reflectDir = Reflect(-toLightDir, gNormal);"
				"		float specular = Pow(Max(Dot(toViewDir, reflectDir), 0), 16) * gSpecular;"
				""
				"		float epsilon = (halfOuterCutOff - halfInnerCutOff);"
				"		float intensity = Clamp((halfOuterCutOff - theta) / epsilon, 0, 1);"
				""
				"		diffuse *= intensity;"
				"		specular *= intensity;"
				"		result = diffuse + specular;"
				"	}"
				"	return float4(result, 1);"
				"}";

			DeferredRendering::DeferredRendering(void) :
				m_DeviceInterface(nullptr),
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

				ProgramInfo info;

#ifdef DEBUG_MODE
				info.DebugMode = true;
#endif

				info.Source = AmbientLightShader;
				m_AmbientLightProgram = ProgramResource(m_DeviceInterface->CreateProgram(&info));

				info.Source = DirectionalLightShader;
				m_DirectionalLightProgram = ProgramResource(m_DeviceInterface->CreateProgram(&info));

				info.Source = PointLightShader;
				m_PointLightProgram = ProgramResource(m_DeviceInterface->CreateProgram(&info));

				info.Source = SpotLightShader;
				m_SpotLightProgram = ProgramResource(m_DeviceInterface->CreateProgram(&info));

				m_DeviceInterface->OnContextChangedEvent += EventListener_OnContextChanged;
				m_DeviceInterface->OnContextResizedEvent += EventListener_OnContextResized;
			}

			void DeferredRendering::Uninitialize(void)
			{
				m_DeviceInterface->OnContextChangedEvent -= EventListener_OnContextChanged;
				m_DeviceInterface->OnContextResizedEvent -= EventListener_OnContextResized;

				m_DeviceInterface->DestroyProgram(*m_AmbientLightProgram);
				m_DeviceInterface->DestroyProgram(*m_DirectionalLightProgram);
				m_DeviceInterface->DestroyProgram(*m_PointLightProgram);
				m_DeviceInterface->DestroyProgram(*m_SpotLightProgram);

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