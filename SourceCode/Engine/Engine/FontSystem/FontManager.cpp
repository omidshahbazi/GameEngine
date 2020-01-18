// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <FontSystem\FontManager.h>
#include <FontSystem\Private\FontSystemAllocators.h>
#include <FontSystem\Font.h>
#include <Utility\AssetParser\InternalModelParser.h>
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
			Font *font = Allocate<Font>();
			Construct(font);

			auto &initialChars = font->m_InitialCharacters;

			uint64 index = 0;

#define READ_VALUE(Type) \
			Data.ReadValue<Type>(index);\
			index += sizeof(Type);

			while (index < Data.GetSize())
			{
				uint64 charCode = READ_VALUE(uint64);

				Vector2F size;
				size.X = READ_VALUE(float32);
				size.Y = READ_VALUE(float32);

				Vector2F bearing;
				bearing.X = READ_VALUE(float32);
				bearing.Y = READ_VALUE(float32);

				Vector2F advance;
				advance.X = READ_VALUE(float32);
				advance.Y = READ_VALUE(float32);

				uint64 meshDataSize = READ_VALUE(uint64);

				MeshInfo *meshInfo = nullptr;
				if (meshDataSize != 0)
				{
					const byte* meshData = Data.ReadValue(index, meshDataSize);
					index += meshDataSize;

					meshInfo = Allocate<MeshInfo>();
					Construct(meshInfo, &FontSystemAllocators::FontSystemAllocator);

					InternalModelParser parser;
					parser.Parse(ByteBuffer(ConstCast(byte*, meshData), meshDataSize), *meshInfo);
				}

				initialChars.Add(charCode, { meshInfo, size, bearing, advance });
			}

#undef READ_VALUE

			return font;
		}

		void FontManager::DestroyFont(Font * Font)
		{
		}
	}
}