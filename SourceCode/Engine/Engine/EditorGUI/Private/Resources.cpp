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

			const String TEXTURE_TEXT_SHADER_NAME = "TextureText.shader";
			const String TEXTURE_TEXT_SHADER_SOURCE =
				"float3 pos : POSITION;"
				"float2 uv : UV;"
				"const texture2D _FontTexture;"
				"const float4 _FontTextureUV;"
				"const float4 color;"
				"const matrix4 _MVP;"
				"float4 VertexMain()"
				"{"
				"return _MVP * float4(pos, 1);"
				"}"
				"float4 FragmentMain()"
				"{"
				"return color * texture(_FontTexture, uv).r;"
				"}";

			ResourceHolder* Resources::m_ResourceHolder = nullptr;
			Mesh* Resources::m_QuadMesh = nullptr;
			Font* Resources::m_Font = nullptr;
			Material Resources::m_SpriteRendererMaterial;
			Material Resources::m_TextRendererMaterial;
			Resources::SpriteMap Resources::m_Sprites;

			ShaderResource CreateShader(const String& Name, const String& Source)
			{
				ShaderInfo shaderInfo;
				shaderInfo.Value = Source;

				Shader* shader = RenderingManager::GetInstance()->GetActiveDevice()->CreateShader(&shaderInfo);

				return ResourceManager::GetInstance()->LoadFromMemory(Name, shader);
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

				m_QuadMesh = ResourceManager::GetInstance()->GetPrimitiveMesh(ResourceManager::PrimitiveMeshTypes::Quad).GetData()->GetData();

				m_Font = m_ResourceHolder->Load<Font>("Roboto-Light.ttf").GetData()->GetData();

				ShaderResource spriteRendererShader = ResourceManager::GetInstance()->GetSpriteRendererShader();
				{
					Pass pass(*spriteRendererShader);
					pass.GetRenderState().BlendFunctionDestinationFactor = IDevice::BlendFunctions::OneMinusSourceAlpha;
					pass.GetRenderState().BlendFunctionSourceFactor = IDevice::BlendFunctions::SourceAlpha;
					pass.GetRenderState().DepthTestFunction = IDevice::TestFunctions::Never;

					m_SpriteRendererMaterial.AddPass(pass);
				}

				ShaderResource textRendererShader = CreateShader(TEXTURE_TEXT_SHADER_NAME, TEXTURE_TEXT_SHADER_SOURCE);
				{
					Pass pass(*textRendererShader);
					pass.GetRenderState().BlendFunctionDestinationFactor = IDevice::BlendFunctions::OneMinusSourceAlpha;
					pass.GetRenderState().BlendFunctionSourceFactor = IDevice::BlendFunctions::SourceAlpha;
					pass.GetRenderState().DepthTestFunction = IDevice::TestFunctions::Never;

					m_TextRendererMaterial.AddPass(pass);
				}
			}
		}
	}
}