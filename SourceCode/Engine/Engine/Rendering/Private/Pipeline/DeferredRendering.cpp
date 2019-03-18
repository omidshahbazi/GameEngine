// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <Rendering\Private\Pipeline\DeferredRendering.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\Material.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace Pipeline
			{
				SINGLETON_DEFINITION(DeferredRendering)

					DeferredRendering::DeferredRendering(void) :
					m_RenderTarget(nullptr),
					m_QuadMesh(nullptr)
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
				}

				void DeferredRendering::BindRenderTarget(void)
				{
					DeviceInterface *device = RenderingManager::GetInstance()->GetActiveDevice();

					device->SetRenderTarget(m_RenderTarget);
					device->Clear(IDevice::ClearFlags::ColorBuffer | IDevice::ClearFlags::DepthBuffer, Color(0, 0, 0, 255));
				}

				void DeferredRendering::Render(void)
				{
					static Matrix4F quadMat;
					quadMat.MakeIdentity();

					Material mat;
	/*				Pass pass(*shader1);
					pass.SetTexture("tex0", tex0);
					pass.SetTexture("tex1", tex1);
					pass.SetTexture("tex2", tex2);
					mat.AddPass(pass);*/

					//RenderingManager::GetInstance()->GetActiveDevice()->DrawMesh(m_QuadMesh, quadMat, &mat);
				}
			}
		}
	}
}
