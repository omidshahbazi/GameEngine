// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Private\Resources.h>
#include <Rendering\RenderingManager.h>
#include <ResourceSystem\Private\ResourceHolder.h>
#include <Utility\FileSystem.h>
#include <Utility\Path.h>
#include <Containers\Strings.h>




#include <Rendering\PixelBuffer.h>

namespace Engine
{
	using namespace Containers;
	using namespace Utility;
	using namespace ResourceSystem::Private;

	namespace EditorGUI
	{
		namespace Private
		{
			const WString ASSETS_DIRECTORY_PATH(L"../Contents");
			const WString LIBRARY_DIRECTORY_PATH(L"../Contents/Library");

			Mesh* Resources::m_QuadMesh = nullptr;
			Material Resources::m_TitleBarMaterial;
			Material Resources::m_NineSliceMaterial;
			Material Resources::m_SimpleMaterial;
			TextureHandle* Resources::m_ButtonTexture = nullptr;

			void Resources::Initialize(void)
			{
				static bool isInitialized = false;
				if (isInitialized)
					return;
				isInitialized = true;

				WString executingPath = Path::GetDirectoryName(FileSystem::GetExecutablePath());
				ResourceHolder resHolder(Path::Combine(executingPath, ASSETS_DIRECTORY_PATH), Path::Combine(executingPath, LIBRARY_DIRECTORY_PATH));
				resHolder.CheckResources();

				m_QuadMesh = resHolder.LoadPrimitiveMesh(PrimitiveMeshTypes::Quad).GetData()->GetData();

				ProgramResource program = resHolder.LoadProgram("TitlebarProgram", "float3 pos : POSITION;const matrix4 _MVP;float4 VertexMain(){return _MVP * float4(pos, 1);}float4 FragmentMain(){return float4(1, 0, 1, 1);}");
				{
					Pass pass(*program);
					m_TitleBarMaterial.AddPass(pass);
				}

				program = resHolder.Load<Program>("9Slice.shader");
				{
					Pass pass(*program);

					pass.GetRenderState().BlendFunctionDestinationFactor = IDevice::BlendFunctions::OneMinusSourceAlpha;
					pass.GetRenderState().BlendFunctionSourceFactor = IDevice::BlendFunctions::SourceAlpha;

					m_NineSliceMaterial.AddPass(pass);
				}

				program = resHolder.Load<Program>("Simple.shader");
				{
					Pass pass(*program);

					pass.GetRenderState().BlendFunctionDestinationFactor = IDevice::BlendFunctions::OneMinusSourceAlpha;
					pass.GetRenderState().BlendFunctionSourceFactor = IDevice::BlendFunctions::SourceAlpha;

					m_SimpleMaterial.AddPass(pass);
				}

				m_ButtonTexture = resHolder.Load<Texture>("Block1.png").GetData();

				PixelBuffer* buff = m_ButtonTexture->GetData()->GetBuffer();

				buff->Lock(IDevice::BufferAccess::WriteOnly);


				//buff->Move(10);

				auto& col = buff->GetColorUI8Pixel();

				col = ColorUI8::Red;

				buff->Unlock();
			}
		}
	}
}