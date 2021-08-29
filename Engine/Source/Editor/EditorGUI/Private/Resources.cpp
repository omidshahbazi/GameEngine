// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Private\Resources.h>
#include <EditorGUI\Private\EditorGUIAllocators.h>
#include <Rendering\RenderingManager.h>
#include <ResourceSystem\ResourceManager.h>
#include <ResourceSystem\Private\ResourceHolder.h>
#include <FileUtility\FileSystem.h>
#include <FileUtility\Path.h>
#include <Containers\Strings.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	using namespace Containers;
	using namespace FileUtility;
	using namespace ResourceSystem;
	using namespace ResourceSystem::Private;
	using namespace MemoryManagement::Allocator;

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
					Pass pass(spriteRendererProgram);
					pass.GetRenderState().BlendFunctionDestinationFactor = IDevice::BlendFunctions::OneMinusSourceAlpha;
					pass.GetRenderState().BlendFunctionSourceFactor = IDevice::BlendFunctions::SourceAlpha;
					pass.GetRenderState().DepthTestFunction = IDevice::TestFunctions::Never;

					m_SpriteRendererMaterial.AddPass(pass);
				}

				ProgramResource* textRendererProgram = CreateProgram(TEXTURE_TEXT_PROGRAM_SOURCE);
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
				//if (m_Sprites.Contains(Name))
				//	return m_Sprites[Name];

				//auto sprite = m_ResourceHolder->Load<Sprite>(Name);

				//m_Sprites[Name] = sprite;

				//return sprite;


				return m_ResourceHolder->Load<Sprite>(Name);
			}

			ProgramResource* Resources::CreateProgram(const String& Source)
			{
				ProgramInfo programInfo;
				programInfo.Source = Source;

				Program* program = RenderingManager::GetInstance()->GetActiveDevice()->CreateProgram(&programInfo);

				return m_ResourceHolder->AddFromMemory(GUID::Create(), program);
			}
		}
	}
}