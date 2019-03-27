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
					"const texture2D AlbedoSpec;"
					"const matrix4 _MVP;"
					"const float4 color;"
					"const float strength;"
					""
					"float4 VertexMain()"
					"{"
					"	return _MVP * float4(pos, 1);"
					"}"
					""
					"float4 FragmentMain()"
					"{"
					"	return float4(texture(AlbedoSpec, uv).rgb * color.rgb * strength, 1);"
					"}";

				const String DirectionalLightShader =
					"float3 pos : POSITION;"
					"float2 uv : UV;"
					"const texture2D AlbedoSpec;"
					"const texture2D NormTex;"
					"const matrix4 _MVP;"
					"const float4 color;"
					"const float strength;"
					"const float3 direction;"
					""
					"float4 VertexMain()"
					"{"
					"	return _MVP * float4(pos, 1);"
					"}"
					""
					"float4 FragmentMain()"
					"{"
					"	float3 dir = normalize(-direction);"
					"	float diff = max(dot(texture(NormTex, uv).rgb, dir), 0.0);"
					"	return float4(texture(AlbedoSpec, uv).rgb * color.rgb * strength * diff, 1);"
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
					tex0.Format = Texture::Formats::RGB16F;
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
					depthTex.Format = Texture::Formats::Depth24;
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
				}
			}
		}
	}
}
