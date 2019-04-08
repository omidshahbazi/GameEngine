// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\Pipeline\DeferredRendering.h>
#include <Rendering\RenderingManager.h>

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
					"		float3 toLight = normalize(worldPos - gPos);"
					"		float3 normal = texture(NormalTex, uv).rgb;"
					"		float3 diffuse = max(dot(normal, toLight), 0.0) * color.rgb * gDiffuse;"
					""
					"		float3 viewDir = normalize(viewPos - gPos);"
					"		float3 halfwayDir = normalize(toLight + viewDir);"
					"		float3 specular = pow(max(dot(normal, halfwayDir), 0.0), 16) * color.rgb * gSpecular;"
					""
					"		float attenuation = 1 / (constantAttenuation + (linearAttenuation * distance) + (quadraticAttenuation * distance * distance));"
					""
					"		diffuse *= attenuation;"
					//"		specular *= attenuation;"
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

					const int width = 1024;
					const int height = 768;

					RenderTargetInfo gbuffer;

					RenderTextureInfo tex0;
					tex0.Format = Texture::Formats::RGB32F;
					tex0.Point = RenderTarget::AttachmentPoints::Color0;
					tex0.Width = width;
					tex0.Height = height;
					gbuffer.Textures.Add(tex0);

					RenderTextureInfo tex1;
					tex1.Format = Texture::Formats::RGB16F;
					tex1.Point = RenderTarget::AttachmentPoints::Color1;
					tex1.Width = width;
					tex1.Height = height;
					gbuffer.Textures.Add(tex1);

					RenderTextureInfo tex2;
					tex2.Format = Texture::Formats::RGBA8;
					tex2.Point = RenderTarget::AttachmentPoints::Color2;
					tex2.Width = width;
					tex2.Height = height;
					gbuffer.Textures.Add(tex2);

					RenderTextureInfo depthTex;
					depthTex.Format = Texture::Formats::Depth16;
					depthTex.Point = RenderTarget::AttachmentPoints::Depth;
					depthTex.Width = width;
					depthTex.Height = height;
					gbuffer.Textures.Add(depthTex);

					m_RenderTarget = device->CreateRenderTarget(&gbuffer);
					m_PositionTexture = TextureHandle((*m_RenderTarget)[0]);
					m_NormalTexture = TextureHandle((*m_RenderTarget)[1]);
					m_AlbedoSpecularTexture = TextureHandle((*m_RenderTarget)[2]);

					m_AmbientLightProgram = ProgramHandle(device->CreateProgram(AmbientLightShader));
					m_DirectionalLightProgram = ProgramHandle(device->CreateProgram(DirectionalLightShader));
					m_PointLightProgram = ProgramHandle(device->CreateProgram(PointLightShader));
				}
			}
		}
	}
}
