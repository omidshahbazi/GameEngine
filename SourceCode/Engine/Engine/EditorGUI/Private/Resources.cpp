// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Private\Resources.h>
#include <EditorGUI\Private\EditorGUIAllocators.h>
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

			cstr TEXTURE_TEXT_PROGRAM_NAME = "TextureText.program";
			cstr TEXTURE_TEXT_PROGRAM_SOURCE =
				"#include <ShaderIncludes.program>"
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

			Resources::Resources(void)
			{
				m_ResourceHolder = EditorGUIAllocators::TypesAllocator_Allocate<ResourceHolder>();
				Construct(m_ResourceHolder, Path::Combine(FileSystem::GetWorkingPath(), WString(ASSETS_DIRECTORY_PATH)), Path::Combine(FileSystem::GetWorkingPath(), WString(LIBRARY_DIRECTORY_PATH)));
				m_ResourceHolder->GetCompiler()->CompileResources();

				m_QuadMesh = ResourceManager::GetInstance()->GetPrimitiveMesh(ResourceManager::PrimitiveMeshTypes::Quad)->GetPointer();
				m_QuadMesh->SetName(L"QuadTESSSST");

				m_Font = m_ResourceHolder->Load<Font>("Roboto-Light.ttf");

				ProgramResource* spriteRendererProgram = ResourceManager::GetInstance()->GetSpriteRendererShader();
				{
					Pass pass(spriteRendererProgram);
					pass.GetRenderState().BlendFunctionDestinationFactor = IDevice::BlendFunctions::OneMinusSourceAlpha;
					pass.GetRenderState().BlendFunctionSourceFactor = IDevice::BlendFunctions::SourceAlpha;
					pass.GetRenderState().DepthTestFunction = IDevice::TestFunctions::Never;

					m_SpriteRendererMaterial.AddPass(pass);
				}

				ProgramResource* textRendererProgram = CreateProgram(TEXTURE_TEXT_PROGRAM_NAME, TEXTURE_TEXT_PROGRAM_SOURCE);
				{
					Pass pass(textRendererProgram);
					pass.GetRenderState().BlendFunctionDestinationFactor = IDevice::BlendFunctions::OneMinusSourceAlpha;
					pass.GetRenderState().BlendFunctionSourceFactor = IDevice::BlendFunctions::SourceAlpha;
					pass.GetRenderState().DepthTestFunction = IDevice::TestFunctions::Never;

					m_TextRendererMaterial.AddPass(pass);
				}
			}

			Resources::~Resources(void)
			{
				EditorGUIAllocators::TypesAllocator_Deallocate(m_ResourceHolder);
			}

			SpriteResource* Resources::GetSprite(const String& Name)
			{
				if (m_Sprites.Contains(Name))
					return m_Sprites[Name];

				auto sprite = m_ResourceHolder->Load<Sprite>(Name);

				m_Sprites[Name] = sprite;

				return sprite;
			}

			ProgramResource* Resources::CreateProgram(const String& Name, const String& Source)
			{
				ProgramInfo programInfo;
				programInfo.Source = Source;

				Program* program = RenderingManager::GetInstance()->GetActiveDevice()->CreateProgram(&programInfo);

				return m_ResourceHolder->AddFromMemory(Name, program);
			}
		}
	}
}