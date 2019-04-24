// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <FontSystem\FontManager.h>
#include <FontSystem\Private\FontSystemAllocators.h>
#include <FontSystem\Font.h>
#include <Utility\AssetParser\InternalModelParser.h>
#include <Rendering\RenderingManager.h>
#include <MemoryManagement\Allocator\RootAllocator.h>

namespace Engine
{
	using namespace Containers;
	using namespace Utility::AssetParser;
	using namespace Rendering;

	namespace FontSystem
	{
		using namespace Private;

		template<typename BaseType>
		BaseType *Allocate(void)
		{
			return ReinterpretCast(BaseType*, AllocateMemory(&FontSystemAllocators::FontSystemAllocator, sizeof(BaseType)));
		}

		template<typename BaseType>
		void Deallocate(BaseType *Ptr)
		{
			DeallocateMemory(&FontSystemAllocators::FontSystemAllocator, Ptr);
		}

		SINGLETON_DEFINITION(FontManager)

			FontManager::FontManager(void)
		{
		}

		FontManager::~FontManager(void)
		{
		}

		Font *FontManager::LoadFont(const ByteBuffer &Data)
		{
			DeviceInterface *device = RenderingManager::GetInstance()->GetActiveDevice();

			FrameAllocator meshhAllocator("Mesh Generator Allocator", &FontSystemAllocators::FontSystemAllocator, 50 * MegaByte);

			Font *font = Allocate<Font>();
			Construct(font);

			auto &meshes = font->m_Meshes;

			uint64 index = 0;
			while (index < Data.GetSize())
			{
				uint64 charCode = Data.ReadValue<uint64>(index);
				index += sizeof(uint64);

				uint64 meshDataSize = Data.ReadValue<uint64>(index);
				index += sizeof(uint64);

				const byte* meshData = Data.ReadValue(index, meshDataSize);
				index += meshDataSize;

				MeshInfo meshInfo(&meshhAllocator);
				SubMeshInfo subMeshInfo(&meshhAllocator);

				InternalModelParser parser;
				//parser.Parse(meshData, meshDataSize, subMeshInfo);

				meshInfo.SubMeshes.Add(subMeshInfo);

				Mesh *mesh = device->CreateMesh(&meshInfo, IDevice::BufferUsages::StaticDraw);

				meshes.Add(charCode, mesh);

				meshhAllocator.Reset();
			}

			return font;
		}

		void FontManager::DestroyFont(Font * Font)
		{
		}
	}
}