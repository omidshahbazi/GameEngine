// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Private\Resources.h>
#include <EditorGUI\Private\EditorGUIAllocators.h>
#include <RenderSystem\RenderManager.h>
#include <RenderSystem\Program.h>
#include <ResourceManagement\ResourceManager.h>
#include <ResourceSystem\ResourceHolder.h>
#include <FileUtility\FileSystem.h>
#include <FileUtility\Path.h>
#include <Containers\Strings.h>
#include <Allocators\RootAllocator.h>

namespace Engine
{
	using namespace Containers;
	using namespace FileUtility;
	using namespace ResourceManagement;
	using namespace ResourceSystem;
	using namespace Allocators;

	namespace EditorGUI
	{
		namespace Private
		{
			SINGLETON_DEFINITION(Resources);

			cwstr ASSETS_DIRECTORY_PATH(L"../../Contents/Editor");
			cwstr LIBRARY_DIRECTORY_PATH(L"../../Contents/Editor/Library");

			Resources::Resources(void)
			{
				m_ResourceHolder = EditorGUIAllocators::TypesAllocator_Allocate<ResourceHolder>();
				Construct(m_ResourceHolder, Path::Combine(FileSystem::GetWorkingPath(), WString(ASSETS_DIRECTORY_PATH)), Path::Combine(FileSystem::GetWorkingPath(), WString(LIBRARY_DIRECTORY_PATH)));
				m_ResourceHolder->GetCompiler()->CompileResources();

				m_QuadMesh = ResourceManager::GetInstance()->GetPrimitiveMesh(ResourceManager::PrimitiveMeshTypes::Quad)->GetPointer();

				m_Font = m_ResourceHolder->Load<Font>("Fonts/Roboto-Light.ttf");

				ProgramResource* spriteRendererProgram = ResourceManager::GetInstance()->GetSpriteRendererShader();
				{
					spriteRendererProgram->Wait();

					m_SpriteRendererMaterial.SetProgram(spriteRendererProgram);
					m_SpriteRendererMaterial.GetRenderState().BlendFunctionDestinationFactor = BlendFunctions::OneMinusSourceAlpha;
					m_SpriteRendererMaterial.GetRenderState().BlendFunctionSourceFactor = BlendFunctions::SourceAlpha;
					m_SpriteRendererMaterial.GetRenderState().DepthTestFunction = TestFunctions::Never;
				}

				ProgramResource* textRendererProgram = m_ResourceHolder->Load<Program>("Programs/TextureText.program");
				{
					textRendererProgram->Wait();

					m_TextRendererMaterial.SetProgram(textRendererProgram);
					m_TextRendererMaterial.GetRenderState().BlendFunctionDestinationFactor = BlendFunctions::OneMinusSourceAlpha;
					m_TextRendererMaterial.GetRenderState().BlendFunctionSourceFactor = BlendFunctions::SourceAlpha;
					m_TextRendererMaterial.GetRenderState().DepthTestFunction = TestFunctions::Never;
				}
			}

			Resources::~Resources(void)
			{
				EditorGUIAllocators::TypesAllocator_Deallocate(m_ResourceHolder);
			}

			SpriteResource* Resources::GetSprite(const String& Name)
			{
				return m_ResourceHolder->Load<Sprite>("Sprites/" + Name);
			}
		}
	}
}