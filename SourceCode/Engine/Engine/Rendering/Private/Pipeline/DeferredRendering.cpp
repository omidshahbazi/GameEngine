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
				const String GBUfferShader =
					"float3 pos : POSITION;"
					"const matrix4 _MVP;"
					"float2 uv : UV;"
					"const texture2D PosTex;"
					"const texture2D NormTex;"
					"const texture2D AlbedoSpec;"
					""
					"float4 VertexMain()"
					"{"
					"	return _MVP * float4(pos, 1);"
					"}"
					""
					"float4 FragmentMain()"
					"{"
					"	return float4(1, 1, 1, 1) * texture(PosTex, uv);"
					"}";

				SINGLETON_DEFINITION(DeferredRendering)

					DeferredRendering::DeferredRendering(void) :
					m_RenderTarget(nullptr),
					m_PositionTexture(nullptr),
					m_NormalTexture(nullptr),
					m_AlbedoSpecularTexture(nullptr),
					m_QuadMesh(nullptr),
					m_Program(nullptr)
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

					MeshInfo mesh;
					SubMeshInfo subMesh;
					subMesh.Vertices.Add({ Vector3F(-1, 1, 0), Vector2F(0, 1) });
					subMesh.Vertices.Add({ Vector3F(-1, -1, 0), Vector2F(0, 0) });
					subMesh.Vertices.Add({ Vector3F(1, 1, 0), Vector2F(1, 1) });
					subMesh.Vertices.Add({ Vector3F(1, -1, 0), Vector2F(1, 0) });
					subMesh.Indices.Add(0);
					subMesh.Indices.Add(1);
					subMesh.Indices.Add(2);
					subMesh.Indices.Add(2);
					subMesh.Indices.Add(1);
					subMesh.Indices.Add(3);
					subMesh.Layout = Mesh::SubMesh::VertexLayouts::Position | Mesh::SubMesh::VertexLayouts::UV;
					mesh.SubMeshes.Add(subMesh);

					m_QuadMesh = device->CreateMesh(&mesh, IDevice::BufferUsages::StaticDraw);

					m_Program = ProgramHandle(device->CreateProgram(GBUfferShader));
					Pass pass(&m_Program);
					pass.SetTexture("PosTex", &m_PositionTexture);
					pass.SetTexture("NormTex", &m_NormalTexture);
					pass.SetTexture("AlbedoSpec", &m_AlbedoSpecularTexture);
					m_Material.AddPass(pass);
				}

				void DeferredRendering::BindRenderTarget(void)
				{
					DeviceInterface *device = RenderingManager::GetInstance()->GetActiveDevice();

					device->SetRenderTarget(m_RenderTarget);
					device->Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer, Color(0, 0, 0, 255));
				}

				void DeferredRendering::Render(void)
				{
					DeviceInterface *device = RenderingManager::GetInstance()->GetActiveDevice();

					device->SetRenderTarget(nullptr);
					device->Clear(IDevice::ClearFlags::ColorBuffer, Color(0, 0, 0, 255));

					static Matrix4F quadMat;
					quadMat.MakeIdentity();

					device->DrawMesh(m_QuadMesh, quadMat, &m_Material);
				}
			}
		}
	}
}
