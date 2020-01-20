// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\Pipeline\DeferredRendering.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Pipeline
			{
				const String AmbientLightShader =
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

				const String DirectionalLightShader =
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

				const String PointLightShader =
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

				const String SpotLightShader =
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

				SINGLETON_DEFINITION(DeferredRendering)

				DeferredRendering::DeferredRendering(void) :
					m_RenderTarget(nullptr)
				{
				}

				void DeferredRendering::Initialize(void)
				{
					DeviceInterface *device = RenderingManager::GetInstance()->GetActiveDevice();

					device->AddListener(this);

					OnDeviceInterfaceResized(device);

					m_AmbientLightProgram = ProgramHandle(device->CreateProgram(AmbientLightShader));
					m_DirectionalLightProgram = ProgramHandle(device->CreateProgram(DirectionalLightShader));
					m_PointLightProgram = ProgramHandle(device->CreateProgram(PointLightShader));
					m_SpotLightProgram = ProgramHandle(device->CreateProgram(SpotLightShader));
				}

				void DeferredRendering::OnDeviceInterfaceResized(DeviceInterface * DeviceInterface)
				{
					if (m_RenderTarget != nullptr)
						DeviceInterface->DestroyRenderTarget(m_RenderTarget);

					Window *window = DeviceInterface->GetWindow();
					auto &size = window->GetClientSize();

					RenderTargetInfo gbuffer;

					RenderTextureInfo tex0;
					tex0.Format = Texture::Formats::RGB32F;
					tex0.Point = RenderTarget::AttachmentPoints::Color0;
					tex0.Width = size.X;
					tex0.Height = size.Y;
					gbuffer.Textures.Add(tex0);

					RenderTextureInfo tex1;
					tex1.Format = Texture::Formats::RGB16F;
					tex1.Point = RenderTarget::AttachmentPoints::Color1;
					tex1.Width = size.X;
					tex1.Height = size.Y;
					gbuffer.Textures.Add(tex1);

					RenderTextureInfo tex2;
					tex2.Format = Texture::Formats::RGBA8;
					tex2.Point = RenderTarget::AttachmentPoints::Color2;
					tex2.Width = size.X;
					tex2.Height = size.Y;
					gbuffer.Textures.Add(tex2);

					RenderTextureInfo depthTex;
					depthTex.Format = Texture::Formats::Depth16;
					depthTex.Point = RenderTarget::AttachmentPoints::Depth;
					depthTex.Width = size.X;
					depthTex.Height = size.Y;
					gbuffer.Textures.Add(depthTex);

					m_RenderTarget = DeviceInterface->CreateRenderTarget(&gbuffer);
					m_PositionTexture = TextureHandle((*m_RenderTarget)[0]);
					m_NormalTexture = TextureHandle((*m_RenderTarget)[1]);
					m_AlbedoSpecularTexture = TextureHandle((*m_RenderTarget)[2]);
				}
			}
		}
	}
}
