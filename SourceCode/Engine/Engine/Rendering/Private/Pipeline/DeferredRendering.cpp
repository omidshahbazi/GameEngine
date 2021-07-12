// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\Pipeline\DeferredRendering.h>
#include <Rendering\ProgramInfo.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Pipeline
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
					"		1.0, 0.0, 0.0, 0.0,"
					"		0.0, 1.0, 0.0, 0.0,"
					"		0.0, 0.0, 1.0, 0.0,"
					"		0.0, 0.0, 0.0, 1.0);"
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
					"	float3 dir = normalize(-data.Direction);"
					"	float3 normal = Sample(NormalTexture, inputData.UV).rgb;"
					"	float angle = max(dot(normal, dir), 0.0);"
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
					"	float distance = length(data.WorldPosition - gPos);"
					""
					"	float3 result = float3(0,0,0);"
					"	if (distance < data.Radius)"
					"	{"
					"		float4 gAlbedoSpec = Sample(AlbedoSpecTexture, uv);"
					"		float3 gDiffuse = gAlbedoSpec.rgb;"
					"		float gSpecular = gAlbedoSpec.a;"
					""
					"		float3 toLightDir = normalize(data.WorldPosition - gPos);"
					"		float3 normal = Sample(NormalTexture, uv).rgb;"
					""
					"		float3 diffuse = max(dot(normal, toLightDir), 0) * data.Color.rgb * gDiffuse * data.Strength;"
					""
					"		float3 viewDir = normalize(data.ViewPosition - gPos);"
					"		float3 halfwayDir = normalize(toLightDir + viewDir);"
					"		float3 specular = pow(max(dot(normal, halfwayDir), 0), 16) * data.Color.rgb * gSpecular;"
					""
					"		float attenuation = 1 / (data.ConstantAttenuation + (data.LinearAttenuation * distance) + (data.QuadraticAttenuation * distance * distance));"
					""
					"		diffuse *= attenuation;"
					"		specular *= attenuation;"
					"		result = diffuse + specular;"
					"	}"
					"	return float4(result , 1);"
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
					"	float coneRadius = sin(data.OuterCutOff / 2) * (data.Radius / cos(data.OuterCutOff / 2));"
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
					"	float3 toFragDir = normalize(gPos - data.WorldPosition);"
					"	float theta = acos(dot(toFragDir, data.Direction) / length(toFragDir) * length(data.Direction));"
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
					"		float3 toLightDir = normalize(data.WorldPosition - gPos);"
					""
					"		float3 diffuse = max(dot(gNormal, toLightDir), 0) * data.Color.rgb * gDiffuse * data.Strength;"
					""
					"		float3 toViewDir = normalize(data.ViewPosition - gPos);"
					"		float3 reflectDir = reflect(-toLightDir, gNormal);"
					"		float specular = pow(max(dot(toViewDir, reflectDir), 0), 16) * gSpecular;"
					""
					"		float epsilon = (halfOuterCutOff - halfInnerCutOff);"
					"		float intensity = clamp((halfOuterCutOff - theta) / epsilon, 0, 1);"
					""
					"		diffuse *= intensity;"
					"		specular *= intensity;"
					"		result = diffuse + specular;"
					"	}"
					"	return float4(result, 1);"
					"}";

				DeferredRendering::DeferredRendering(DeviceInterface* DeviceInterface) :
					m_DeviceInterface(DeviceInterface),
					m_ActiveInfo(nullptr)
				{
					ProgramInfo info;
					info.Source = AmbientLightShader;
					m_AmbientLightProgram = ProgramResource(m_DeviceInterface->CreateProgram(&info));

					info.Source = DirectionalLightShader;
					m_DirectionalLightProgram = ProgramResource(m_DeviceInterface->CreateProgram(&info));

					info.Source = PointLightShader;
					m_PointLightProgram = ProgramResource(m_DeviceInterface->CreateProgram(&info));

					info.Source = SpotLightShader;
					m_SpotLightProgram = ProgramResource(m_DeviceInterface->CreateProgram(&info));
				}

				DeferredRendering::~DeferredRendering(void)
				{
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

					m_DeviceInterface->SetRenderTarget(nullptr, RenderQueues::Default);
					m_DeviceInterface->Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer, ColorUI8::Black, RenderQueues::Default);

					m_DeviceInterface->SetRenderTarget(m_ActiveInfo->RenderTarget, RenderQueues::Geometry);
					m_DeviceInterface->Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer, ColorUI8::Black, RenderQueues::Geometry);

					m_DeviceInterface->SetRenderTarget(nullptr, RenderQueues::Lighting);
				}

				void DeferredRendering::EndRender(void)
				{
				}

				void DeferredRendering::OnWindowChanged(Window* Window)
				{
					if (Window == nullptr)
						return;

					if (m_RenderTargets.Contains(Window))
					{
						m_ActiveInfo = &m_RenderTargets[Window];
						return;
					}

					m_RenderTargets[Window] = {};
					m_ActiveInfo = &m_RenderTargets[Window];

					RefreshRenderTarget(Window);
				}

				void DeferredRendering::OnWindowResized(Window* Window)
				{
					RefreshRenderTarget(Window);
				}

				void DeferredRendering::SetPassConstants(Material* Material)
				{
					static const Pass::ConstantHash ConstantHash_PositionTexture = Pass::GetHash("PositionTexture");
					static const Pass::ConstantHash ConstantHash_NormalTexture = Pass::GetHash("NormalTexture");
					static const Pass::ConstantHash ConstantHash_AlbedoSpecTexture = Pass::GetHash("AlbedoSpecTexture");

					Material->SetTexture(ConstantHash_PositionTexture, &m_ActiveInfo->PositionTexture);
					Material->SetTexture(ConstantHash_NormalTexture, &m_ActiveInfo->NormalTexture);
					Material->SetTexture(ConstantHash_AlbedoSpecTexture, &m_ActiveInfo->AlbedoSpecularTexture);
				}

				void DeferredRendering::RefreshRenderTarget(Window* Window)
				{
					WindowRenderTargetInfo& info = m_RenderTargets[Window];

					if (info.Size == Window->GetClientSize())
						return;

					if (info.RenderTarget != nullptr)
						m_DeviceInterface->DestroyRenderTarget(info.RenderTarget);

					info.Size = Window->GetClientSize();

					RenderTargetInfo gbuffer;

					RenderTextureInfo tex0;
					tex0.Format = Formats::RGB32F;
					tex0.Point = RenderTarget::AttachmentPoints::Color0;
					tex0.Dimension = { info.Size.X,info.Size.Y };
					gbuffer.Textures.Add(tex0);

					RenderTextureInfo tex1;
					tex1.Format = Formats::RGB16F;
					tex1.Point = RenderTarget::AttachmentPoints::Color1;
					tex1.Dimension = { info.Size.X,info.Size.Y };
					gbuffer.Textures.Add(tex1);

					RenderTextureInfo tex2;
					tex2.Format = Formats::RGBA8;
					tex2.Point = RenderTarget::AttachmentPoints::Color2;
					tex2.Dimension = { info.Size.X,info.Size.Y };
					gbuffer.Textures.Add(tex2);

					RenderTextureInfo depthTex;
					depthTex.Format = Formats::Depth16;
					depthTex.Point = RenderTarget::AttachmentPoints::Depth;
					depthTex.Dimension = { info.Size.X,info.Size.Y };
					gbuffer.Textures.Add(depthTex);

					info.RenderTarget = m_DeviceInterface->CreateRenderTarget(&gbuffer);
					info.RenderTarget->SetName(L"GBuffer");
					info.PositionTexture = TextureResource((*info.RenderTarget)[0]);
					info.NormalTexture = TextureResource((*info.RenderTarget)[1]);
					info.AlbedoSpecularTexture = TextureResource((*info.RenderTarget)[2]);
				}
			}
		}
	}
}
