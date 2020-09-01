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
			SINGLETON_DEFINITION(Resources);

			cwstr ASSETS_DIRECTORY_PATH(L"../Contents/Editor");
			cwstr LIBRARY_DIRECTORY_PATH(L"../Contents/Editor/Library");

			cstr TEXTURE_TEXT_SHADER_NAME = "TextureText.shader";

			cstr TEXTURE_TEXT_SHADER_SOURCE =
				"#include <ShaderIncludes.shader>"
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
				"float2 finalUV;"
				"finalUV.x = Map(uv.x, 0, 1, _FontTextureUV.x, _FontTextureUV.x + _FontTextureUV.z, 1);"
				"finalUV.y = 1 - Map(uv.y, 0, 1, _FontTextureUV.y, _FontTextureUV.y + _FontTextureUV.w, 1);"
				"return color * texture(_FontTexture, finalUV).r;"
				"}";

			Resources::Resources(void) :
				m_Allocator("EditorGUI Allocator", RootAllocator::GetInstance(), MegaByte)
			{
				m_ResourceHolder = m_Allocator_Allocate<ResourceHolder>();
				Construct(m_ResourceHolder, Path::Combine(FileSystem::GetWorkingPath(), WString(ASSETS_DIRECTORY_PATH)), Path::Combine(FileSystem::GetWorkingPath(), WString(LIBRARY_DIRECTORY_PATH)));
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

			Resources::~Resources(void)
			{
				m_Allocator_Deallocate(m_ResourceHolder);
			}

			SpriteHandle* Resources::GetSprite(const String& Name)
			{
				if (m_Sprites.Contains(Name))
					return m_Sprites[Name];

				auto sprite = m_ResourceHolder->Load<Sprite>(Name).GetData();

				m_Sprites[Name] = sprite;

				return sprite;
			}

			ShaderResource Resources::CreateShader(const String& Name, const String& Source)
			{
				ShaderInfo shaderInfo;
				shaderInfo.Source = Source;

				Shader* shader = RenderingManager::GetInstance()->GetActiveDevice()->CreateShader(&shaderInfo);

				return m_ResourceHolder->LoadFromMemory(Name, shader);
			}
		}
	}
}