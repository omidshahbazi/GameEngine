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

			cstr TEXTURE_TEXT_PROGRAM_SOURCE =
				"#include <ShaderIncludes.program>"
				"struct InputData { float3 Position : POSITION; float2 UV : TEXCOORD; };"
				"struct TransformData { matrix4 Model; matrix4 View; matrix4 Projection; matrix4 MVP; };"
				"struct Data { float4 FontTextureBound; float4 Color; };"
				"texture2D FontTexture;"
				"TransformData _TransformData;"
				"Data data;"
				"float4 VertexMain(InputData inputData)"
				"{"
				"return _TransformData.MVP * float4(inputData.Position, 1);"
				"}"
				"float4 FragmentMain(InputData inputData)"
				"{"
				"float2 finalUV;"
				"finalUV.x = Map(inputData.UV.x, 0, 1, data.FontTextureBound.x, data.FontTextureBound.x + data.FontTextureBound.z, 1);"
				"finalUV.y = 1 - Map(inputData.UV.y, 0, 1, data.FontTextureBound.y, data.FontTextureBound.y + data.FontTextureBound.w, 1);"
				"return data.Color * Sample(FontTexture, finalUV).r;"
				"}";

			Resources::Resources(void)
			{
				m_ResourceHolder = EditorGUIAllocators::TypesAllocator_Allocate<ResourceHolder>();
				Construct(m_ResourceHolder, Path::Combine(FileSystem::GetWorkingPath(), WString(ASSETS_DIRECTORY_PATH)), Path::Combine(FileSystem::GetWorkingPath(), WString(LIBRARY_DIRECTORY_PATH)));
				m_ResourceHolder->GetCompiler()->CompileResources();

				m_QuadMesh = ResourceManager::GetInstance()->GetPrimitiveMesh(ResourceManager::PrimitiveMeshTypes::Quad)->GetPointer();

				m_Font = m_ResourceHolder->Load<Font>("Roboto-Light.ttf");

				ProgramResource* spriteRendererProgram = ResourceManager::GetInstance()->GetSpriteRendererShader();
				{
					m_SpriteRendererMaterial.SetProgram(spriteRendererProgram);
					m_SpriteRendererMaterial.GetRenderState().BlendFunctionDestinationFactor = BlendFunctions::OneMinusSourceAlpha;
					m_SpriteRendererMaterial.GetRenderState().BlendFunctionSourceFactor = BlendFunctions::SourceAlpha;
					m_SpriteRendererMaterial.GetRenderState().DepthTestFunction = TestFunctions::Never;
				}

				ProgramResource* textRendererProgram = CreateProgram(TEXTURE_TEXT_PROGRAM_SOURCE);
				{
					textRendererProgram->GetPointer()->SetName("TextProgram");

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
				return m_ResourceHolder->Load<Sprite>(Name);
			}

			ProgramResource* Resources::CreateProgram(const String& Source)
			{
				ProgramInfo programInfo;
				programInfo.Source = Source;

				Program* program = RenderManager::GetInstance()->GetDevice()->CreateProgram(&programInfo);

				return m_ResourceHolder->AddFromMemory(GUID::Create(), program);
			}
		}
	}
}