// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <EditorGUI\Private\Resources.h>
#include <Rendering\RenderingManager.h>
#include <ResourceSystem\ResourceManager.h>

namespace Engine
{
	namespace EditorGUI
	{
		namespace Private
		{
			MeshResource Resources::m_QuadMesh = nullptr;
			Material Resources::m_TitleBarMaterial;

			void Resources::Initialize(void)
			{
				static bool isInitialized = false;
				if (isInitialized)
					return;
				isInitialized = true;

				ResourceManager* resMan = ResourceManager::GetInstance();

				m_QuadMesh = resMan->LoadPrimitiveMesh(PrimitiveMeshTypes::Quad);

				ProgramResource program = resMan->LoadProgram("TitlebarProgram", "float3 pos : POSITION;const matrix4 _MVP;float4 VertexMain(){return _MVP * float4(pos, 1);}float4 FragmentMain(){return float4(1, 0, 1, 1);}");
				Pass pass(*program);
				m_TitleBarMaterial.AddPass(pass);
			}
		}
	}
}