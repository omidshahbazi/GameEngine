// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Private\Resources.h>
#include <Rendering\RenderingManager.h>
#include <ResourceSystem\ResourceManager.h>
#include <ResourceSystem\Private\ResourceHolder.h>
#include <Utility\FileSystem.h>
#include <Utility\Path.h>
#include <Containers\Strings.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	using namespace Containers;
	using namespace Utility;
	using namespace ResourceSystem;
	using namespace ResourceSystem::Private;
	using namespace MemoryManagement::Allocator;

	namespace EditorGUI
	{
		namespace Private
		{
			DynamicSizeAllocator Allocator("EditorGUI Allocator", RootAllocator::GetInstance(), MegaByte);

			const WString ASSETS_DIRECTORY_PATH(L"../Contents");
			const WString LIBRARY_DIRECTORY_PATH(L"../Contents/Library");

			ResourceHolder* Resources::m_ResourceHolder = nullptr;
			Mesh* Resources::m_QuadMesh = nullptr;
			Font* Resources::m_Font = nullptr;
			Material Resources::m_SpriteRendererMaterial;
			Material Resources::m_TextRendererMaterial;
			Resources::SpriteMap Resources::m_Sprites;

			Mesh* CreateQuadMesh(void)
			{
				SubMeshInfo subMeshInfo;
				subMeshInfo.Vertices.Add(Vertex(Vector3F(-0.5F, -0.5F, 0), Vector2F(0, 1)));
				subMeshInfo.Vertices.Add(Vertex(Vector3F(-0.5F, 0.5F, 0), Vector2F(0, 0)));
				subMeshInfo.Vertices.Add(Vertex(Vector3F(0.5F, 0.5F, 0), Vector2F(1, 0)));
				subMeshInfo.Vertices.Add(Vertex(Vector3F(0.5F, -0.5F, 0), Vector2F(1, 1)));
				subMeshInfo.Indices.Add(0);
				subMeshInfo.Indices.Add(1);
				subMeshInfo.Indices.Add(2);
				subMeshInfo.Indices.Add(1);
				subMeshInfo.Indices.Add(3);
				subMeshInfo.Indices.Add(0);
				subMeshInfo.Layout = SubMesh::VertexLayouts::Position | SubMesh::VertexLayouts::UV;

				MeshInfo meshInfo;
				meshInfo.SubMeshes.Add(&subMeshInfo);

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateMesh(&meshInfo, GPUBuffer::Usages::StaticDraw);
			}

			SpriteHandle* Resources::GetGetSprite(const String& Name)
			{
				Initialize();

				if (m_Sprites.Contains(Name))
					return m_Sprites[Name];

				auto sprite = m_ResourceHolder->Load<Sprite>(Name).GetData();

				m_Sprites[Name] = sprite;

				return sprite;
			}

			void Resources::Initialize(void)
			{
				static bool isInitialized = false;
				if (isInitialized)
					return;
				isInitialized = true;

				WString executingPath = Path::GetDirectoryName(FileSystem::GetExecutablePath());

				m_ResourceHolder = ReinterpretCast(ResourceHolder*, AllocateMemory(&Allocator, sizeof(ResourceHolder)));
				Construct(m_ResourceHolder, Path::Combine(executingPath, ASSETS_DIRECTORY_PATH), Path::Combine(executingPath, LIBRARY_DIRECTORY_PATH));
				m_ResourceHolder->CheckResources();

				m_QuadMesh = CreateQuadMesh();

				m_Font = m_ResourceHolder->Load<Font>("Roboto-Light.font").GetData()->GetData();

				ShaderResource spriteRendererShader = ResourceManager::GetInstance()->GetSpriteRendererShader();
				{
					Pass pass(*spriteRendererShader);
					pass.GetRenderState().BlendFunctionDestinationFactor = IDevice::BlendFunctions::OneMinusSourceAlpha;
					pass.GetRenderState().BlendFunctionSourceFactor = IDevice::BlendFunctions::SourceAlpha;
					pass.GetRenderState().DepthTestFunction = IDevice::TestFunctions::Never;

					m_SpriteRendererMaterial.AddPass(pass);
				}

				ShaderResource textRendererShader = ResourceManager::GetInstance()->GetDefaultShader();
				{
					Pass pass(*textRendererShader);
					pass.GetRenderState().DepthTestFunction = IDevice::TestFunctions::Never;

					m_TextRendererMaterial.AddPass(pass);
				}
			}
		}
	}
}