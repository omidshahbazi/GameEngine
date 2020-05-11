// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceSystem\ResourceManager.h>
#include <ResourceSystem\Private\BuiltInAssets.h>
#include <ResourceAssetParser\OBJParser.h>
#include <Rendering\RenderingManager.h>
#include <Rendering\PixelBuffer.h>
#include <Rendering\Sprite.h>
#include <Utility\FileSystem.h>
#include <Utility\Path.h>

namespace Engine
{
	using namespace Utility;
	using namespace ResourceAssetParser;

	namespace ResourceSystem
	{
		using namespace Private;

		const WString ASSETS_DIRECTORY_NAME(L"Assets");
		const WString LIBRARY_DIRECTORY_NAME(L"Library");

		Shader* CreateShader(const ShaderInfo* ShaderInfo)
		{
			return RenderingManager::GetInstance()->GetActiveDevice()->CreateShader(ShaderInfo);
		}

		Mesh* CreateMesh(const SubMeshInfo* SubMeshInfo)
		{
			MeshInfo info(&ResourceSystemAllocators::ResourceAllocator);
			info.SubMeshes.Add(ConstCast(Rendering::SubMeshInfo*, SubMeshInfo));

			return RenderingManager::GetInstance()->GetActiveDevice()->CreateMesh(&info, GPUBuffer::Usages::StaticDraw);
		}

		SINGLETON_DEFINITION(ResourceManager)

			ResourceManager::ResourceManager(void) :
			ResourceHolder(Path::Combine(FileSystem::GetWorkingPath(), ASSETS_DIRECTORY_NAME), Path::Combine(FileSystem::GetWorkingPath(), LIBRARY_DIRECTORY_NAME))
		{
			CreateDefaultResources();
		}

		ResourceManager::~ResourceManager(void)
		{
		}

		TextureResource ResourceManager::GetWhiteTexture(void)
		{
			return ReinterpretCast(TextureHandle*, GetFromLoaded(BuiltInAssets::WHITE_TEXTURE_NAME));
		}

		SpriteResource ResourceManager::GetWhiteSprite(void)
		{
			return ReinterpretCast(SpriteHandle*, GetFromLoaded(BuiltInAssets::WHITE_TEXTURE_NAME));
		}

		ShaderResource ResourceManager::GetDefaultShader(void)
		{
			return Load<Shader>(BuiltInAssets::DEFAULT_SHADER_NAME);
		}

		ShaderResource ResourceManager::GetSpriteRendererShader(void)
		{
			return Load<Shader>(BuiltInAssets::SPRITE_RENDERER_SHADER_NAME);
		}

		MeshResource ResourceManager::LoadPrimitiveMesh(PrimitiveMeshTypes Type)
		{
			WString name;
			switch (Type)
			{
			case ResourceManager::PrimitiveMeshTypes::Quad:
				name = BuiltInAssets::QUAD_MESH_NAME;
				break;

			case ResourceManager::PrimitiveMeshTypes::Cube:
				name = BuiltInAssets::CUBE_MESH_NAME;
				break;

			case ResourceManager::PrimitiveMeshTypes::Sphere:
				name = BuiltInAssets::SPHERE_MESH_NAME;
				break;

			case ResourceManager::PrimitiveMeshTypes::Cone:
				name = BuiltInAssets::CONE_MESH_NAME;
				break;

			}

			return Load<Mesh>(name);
		}

		void ResourceManager::CreateDefaultResources(void)
		{
			// White Texture
			{
				TextureInfo info;
				info.Dimension = Vector2I::One;
				info.Format = Texture::Formats::RGBA8;
				Texture* tex = RenderingManager::GetInstance()->GetActiveDevice()->CreateTexture(&info);
				auto* buf = tex->GetBuffer();
				buf->Lock(GPUBuffer::Access::WriteOnly);
				buf->GetColorUI8Pixel() = ColorUI8::White;
				buf->Unlock();
				AddToLoaded(BuiltInAssets::WHITE_TEXTURE_NAME, AllocateResourceHandle(tex));
			}

			// Default Shader
			{
				ShaderInfo info;
				info.Value = BuiltInAssets::DEFAULT_SHADER_SOURCE;
				AddToLoaded(BuiltInAssets::DEFAULT_SHADER_NAME, AllocateResourceHandle(CreateShader(&info)));
			}

			// Sprite Shader
			{
				ShaderInfo info;
				info.Value = BuiltInAssets::SPRITE_RENDERER_SHADER_SOURCE;
				AddToLoaded(BuiltInAssets::SPRITE_RENDERER_SHADER_NAME, AllocateResourceHandle(CreateShader(&info)));
			}

			// Quad Mesh
			{
				SubMeshInfo* info = ReinterpretCast(SubMeshInfo*, AllocateMemory(&ResourceSystemAllocators::ResourceAllocator, sizeof(SubMeshInfo)));
				Construct(info, &ResourceSystemAllocators::ResourceAllocator);

				info->Vertices.Clear();
				info->Vertices.Add({ Vector3F(-0.5F, 0.5F, 0), Vector2F(0, 1) });
				info->Vertices.Add({ Vector3F(-0.5F, -0.5F, 0), Vector2F(0, 0) });
				info->Vertices.Add({ Vector3F(0.5F, 0.5F, 0), Vector2F(1, 1) });
				info->Vertices.Add({ Vector3F(0.5F, -0.5F, 0), Vector2F(1, 0) });
				info->Indices.Add(0);
				info->Indices.Add(1);
				info->Indices.Add(2);
				info->Indices.Add(2);
				info->Indices.Add(1);
				info->Indices.Add(3);
				info->Type = SubMesh::PolygonTypes::Triangles;
				info->Layout = SubMesh::VertexLayouts::Position | SubMesh::VertexLayouts::UV;

				AddToLoaded(BuiltInAssets::QUAD_MESH_NAME, AllocateResourceHandle(CreateMesh(info)));
			}

			// Cube Mesh
			{
				SubMeshInfo* info = ReinterpretCast(SubMeshInfo*, AllocateMemory(&ResourceSystemAllocators::ResourceAllocator, sizeof(SubMeshInfo)));
				Construct(info, &ResourceSystemAllocators::ResourceAllocator);

				OBJParser::Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::CUBE_MESH_DATA.GetValue())), BuiltInAssets::CUBE_MESH_DATA.GetLength()), *info);
				AddToLoaded(BuiltInAssets::CUBE_MESH_NAME, AllocateResourceHandle(CreateMesh(info)));
			}

			// Sphere Mesh
			{
				SubMeshInfo* info = ReinterpretCast(SubMeshInfo*, AllocateMemory(&ResourceSystemAllocators::ResourceAllocator, sizeof(SubMeshInfo)));
				Construct(info, &ResourceSystemAllocators::ResourceAllocator);

				OBJParser::Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::SPHERE_MESH_DATA.GetValue())), BuiltInAssets::SPHERE_MESH_DATA.GetLength()), *info);
				AddToLoaded(BuiltInAssets::SPHERE_MESH_NAME, AllocateResourceHandle(CreateMesh(info)));
			}

			// Cone Mesh
			{
				SubMeshInfo* info = ReinterpretCast(SubMeshInfo*, AllocateMemory(&ResourceSystemAllocators::ResourceAllocator, sizeof(SubMeshInfo)));
				Construct(info, &ResourceSystemAllocators::ResourceAllocator);

				OBJParser::Parse(ByteBuffer(ReinterpretCast(byte*, ConstCast(char8*, BuiltInAssets::CONE_MESH_DATA.GetValue())), BuiltInAssets::CONE_MESH_DATA.GetLength()), *info);
				AddToLoaded(BuiltInAssets::CONE_MESH_NAME, AllocateResourceHandle(CreateMesh(info)));
			}
		}
	}
}