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
					"float3 pos : POSITION;"
					"float2 uv : UV;"
					"const texture2D AlbedoSpecTex;"
					"const float4 color;"
					"const float strength;"
					""
					"float4 VertexMain()"
					"{"
					"	matrix4 mat = matrix4("
					"		1.0, 0.0, 0.0, 0.0,"
					"		0.0, 1.0, 0.0, 0.0,"
					"		0.0, 0.0, 1.0, 0.0,"
					"		0.0, 0.0, 0.0, 1.0);"
					"	return mat * float4(pos, 1);"
					"}"
					""
					"float4 FragmentMain()"
					"{"
					"	float3 diffuse = texture(AlbedoSpecTex, uv).rgb;"
					"	return float4(diffuse * color.rgb * strength, 1);"
					"}";

				cstr DirectionalLightShader =
					"float3 pos : POSITION;"
					"float2 uv : UV;"
					"const texture2D NormalTex;"
					"const texture2D AlbedoSpecTex;"
					"const float4 color;"
					"const float strength;"
					"const float3 direction;"
					""
					"float4 VertexMain()"
					"{"
					"	matrix4 mat = matrix4("
					"		1, 0, 0, 0,"
					"		0, 1, 0, 0,"
					"		0, 0, 1, 0,"
					"		0, 0, 0, 1);"
					"	return mat * float4(pos, 1);"
					"}"
					""
					"float4 FragmentMain()"
					"{"
					"	float3 dir = normalize(-direction);"
					"	float3 normal = texture(NormalTex, uv).rgb;"
					"	float angle = max(dot(normal, dir), 0.0);"
					"	float3 diffuse = texture(AlbedoSpecTex, uv).rgb;"
					"	return float4(diffuse * color.rgb * strength * angle, 1);"
					"}";

				cstr PointLightShader =
					"float3 pos : POSITION;"
					"const texture2D PositionTex;"
					"const texture2D NormalTex;"
					"const texture2D AlbedoSpecTex;"
					"const matrix4 _MVP;"
					"const float2 _FrameSize;"
					"const float3 worldPos;"
					"const float3 viewPos;"
					"const float4 color;"
					"const float strength;"
					"const float radius;"
					"const float constantAttenuation;"
					"const float linearAttenuation;"
					"const float quadraticAttenuation;"
					""
					"float4 VertexMain()"
					"{"
					"	matrix4 mvp = _MVP;"
					"	matrix4 scaleMat = matrix4("
					"		radius * 2, 0, 0, 0,"
					"		0, radius * 2, 0, 0,"
					"		0, 0, radius * 2, 0,"
					"		0, 0, 0, 1);"
					"	mvp *= scaleMat;"
					"	return mvp * float4(pos, 1);"
					"}"
					""
					"float4 FragmentMain()"
					"{"
					"	float2 uv = _FragPosition / _FrameSize;"
					"	float3 gPos = texture(PositionTex, uv).rgb;"
					"	float distance = length(worldPos - gPos);"
					""
					"	float3 result = float3(0,0,0);"
					"	if (distance < radius)"
					"	{"
					"		float4 gAlbedoSpec = texture(AlbedoSpecTex, uv);"
					"		float3 gDiffuse = gAlbedoSpec.rgb;"
					"		float gSpecular = gAlbedoSpec.a;"
					""
					"		float3 toLightDir = normalize(worldPos - gPos);"
					"		float3 normal = texture(NormalTex, uv).rgb;"
					""
					"		float3 diffuse = max(dot(normal, toLightDir), 0) * color.rgb * gDiffuse * strength;"
					""
					"		float3 viewDir = normalize(viewPos - gPos);"
					"		float3 halfwayDir = normalize(toLightDir + viewDir);"
					"		float3 specular = pow(max(dot(normal, halfwayDir), 0), 16) * color.rgb * gSpecular;"
					""
					"		float attenuation = 1 / (constantAttenuation + (linearAttenuation * distance) + (quadraticAttenuation * distance * distance));"
					""
					"		diffuse *= attenuation;"
					"		specular *= attenuation;"
					"		result = diffuse + specular;"
					"	}"
					"	return float4(result , 1);"
					"}";

				cstr SpotLightShader =
					"float3 pos : POSITION;"
					"const texture2D PositionTex;"
					"const texture2D NormalTex;"
					"const texture2D AlbedoSpecTex;"
					"const matrix4 _MVP;"
					"const float2 _FrameSize;"
					"const float3 worldPos;"
					"const float3 viewPos;"
					"const float4 color;"
					"const float strength;"
					"const float radius;"
					"const float innerCutOff;"
					"const float outerCutOff;"
					"const float3 direction;"
					""
					"float4 VertexMain()"
					"{"
					"	matrix4 mvp = _MVP;"
					"	float coneRadius = sin(outerCutOff / 2) * (radius / cos(outerCutOff / 2));"
					"	matrix4 scaleMat = matrix4("
					"		coneRadius * 2, 0, 0, 0,"
					"		0, coneRadius * 2, 0, 0,"
					"		0, 0, radius, 0,"
					"		0, 0, 0, 1);"
					"	mvp *= scaleMat;"
					"	return mvp * float4(pos, 1);"
					"}"
					""
					"float4 FragmentMain()"
					"{"
					"	float2 uv = _FragPosition / _FrameSize;"
					"	float3 gPos = texture(PositionTex, uv).rgb;"
					"	float3 toFragDir = normalize(gPos - worldPos);"
					"	float theta = acos(dot(toFragDir, direction) / length(toFragDir) * length(direction));"
					"	float halfOuterCutOff = outerCutOff / 2;"
					""
					"	float3 result = float3(0,0,0);"
					"	if (theta <= halfOuterCutOff)"
					"	{"
					"		float halfInnerCutOff = innerCutOff / 2;"
					"		float4 gAlbedoSpec = texture(AlbedoSpecTex, uv);"
					"		float3 gDiffuse = gAlbedoSpec.rgb;"
					"		float gSpecular = gAlbedoSpec.a;"
					"		float3 gNormal = texture(NormalTex, uv).rgb;"
					""
					"		float3 toLightDir = normalize(worldPos - gPos);"
					""
					"		float3 diffuse = max(dot(gNormal, toLightDir), 0) * color.rgb * gDiffuse * strength;"
					""
					"		float3 toViewDir = normalize(viewPos - gPos);"
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
					m_DeviceInterface->Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer, ColorUI8::Red, RenderQueues::Geometry);

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

				void DeferredRendering::SetPassConstants(Pass* Pass)
				{
					static Pass::ConstantHash ConstantHash_PositionTex = Pass::GetHash("PositionTex");
					static Pass::ConstantHash ConstantHash_NormalTex = Pass::GetHash("NormalTex");
					static Pass::ConstantHash ConstantHash_AlbedoSpecTex = Pass::GetHash("AlbedoSpecTex");

					Pass->SetTexture(ConstantHash_PositionTex, &m_ActiveInfo->PositionTexture);
					Pass->SetTexture(ConstantHash_NormalTex, &m_ActiveInfo->NormalTexture);
					Pass->SetTexture(ConstantHash_AlbedoSpecTex, &m_ActiveInfo->AlbedoSpecularTexture);
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
					tex0.Format = Texture::Formats::RGB32F;
					tex0.Point = RenderTarget::AttachmentPoints::Color0;
					tex0.Dimension = { info.Size.X,info.Size.Y };
					gbuffer.Textures.Add(tex0);

					RenderTextureInfo tex1;
					tex1.Format = Texture::Formats::RGB16F;
					tex1.Point = RenderTarget::AttachmentPoints::Color1;
					tex1.Dimension = { info.Size.X,info.Size.Y };
					gbuffer.Textures.Add(tex1);

					RenderTextureInfo tex2;
					tex2.Format = Texture::Formats::RGBA8;
					tex2.Point = RenderTarget::AttachmentPoints::Color2;
					tex2.Dimension = { info.Size.X,info.Size.Y };
					gbuffer.Textures.Add(tex2);

					RenderTextureInfo depthTex;
					depthTex.Format = Texture::Formats::Depth16;
					depthTex.Point = RenderTarget::AttachmentPoints::Depth;
					depthTex.Dimension = { info.Size.X,info.Size.Y };
					gbuffer.Textures.Add(depthTex);

					info.RenderTarget = m_DeviceInterface->CreateRenderTarget(&gbuffer);
					info.PositionTexture = TextureResource((*info.RenderTarget)[0]);
					info.NormalTexture = TextureResource((*info.RenderTarget)[1]);
					info.AlbedoSpecularTexture = TextureResource((*info.RenderTarget)[2]);
				}
			}
		}
	}
}
