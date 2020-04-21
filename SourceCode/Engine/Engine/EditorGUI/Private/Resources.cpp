// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Private\Resources.h>
#include <Rendering\RenderingManager.h>
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
			Material Resources::m_BackgroundMaterial;
			Material Resources::m_TitleBarMaterial;
			Material Resources::m_SpriteRendererMaterial;
			SpriteHandle* Resources::m_ButtonTexture = nullptr;

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

				m_QuadMesh = m_ResourceHolder->LoadPrimitiveMesh(PrimitiveMeshTypes::Quad).GetData()->GetData();

				ShaderResource program = m_ResourceHolder->LoadShader("Background.shader", "float3 pos : POSITION;const matrix4 _MVP;float4 VertexMain(){return _MVP * float4(pos, 1);}float4 FragmentMain(){return float4(0.12, 0.05, 0.05, 1);}");
				{
					Pass pass(*program);
					m_BackgroundMaterial.AddPass(pass);
				}

				program = m_ResourceHolder->LoadShader("Titlebar.shader", "float3 pos : POSITION;const matrix4 _MVP;float4 VertexMain(){return _MVP * float4(pos, 1);}float4 FragmentMain(){return float4(1, 0, 1, 1);}");
				{
					Pass pass(*program);
					m_TitleBarMaterial.AddPass(pass);
				}

				program = m_ResourceHolder->Load<Shader>("SpriteRenderer.shader");
				{
					Pass pass(*program);

					pass.SetColor("color", ColorUI8::White);

					pass.GetRenderState().BlendFunctionDestinationFactor = IDevice::BlendFunctions::OneMinusSourceAlpha;
					pass.GetRenderState().BlendFunctionSourceFactor = IDevice::BlendFunctions::SourceAlpha;

					m_SpriteRendererMaterial.AddPass(pass);
				}

				m_ButtonTexture = m_ResourceHolder->Load<Sprite>("Button.png").GetData();
			}
		}
	}
}