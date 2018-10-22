// Copyright 2016-2017 ?????????????. All Rights Reserved.
#include <ResourceSystem\Private\ResourceFactory.h>
#include <ResourceSystem\Resource.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <Rendering\RenderingManager.h>

#include <ResourceSystem\Private\Parser\OBJParser.h>

namespace Engine
{
	using namespace Containers;
	using namespace Rendering;

	namespace ResourceSystem
	{
		namespace Private
		{
			SINGLETON_DECLARATION(ResourceFactory)

				ResourceFactory::ResourceFactory(void)
			{
			}

			ResourceFactory::~ResourceFactory(void)
			{
			}

			ByteBuffer *ResourceFactory::Compile(const WString &Extension, ByteBuffer *Buffer)
			{
				//Parser::OBJParser parser;
				//parser.Parse(L"G:/GameEngine/sourcecode/engine/Binaries/Assets/box.obj");

				ResourceTypes type = GetTypeByExtension(Extension);

				if (type == ResourceTypes::Unknown)
					return nullptr;

				ByteBuffer *buffer = ResourceSystemAllocators::Allocate<ByteBuffer>(1);
				new (buffer) ByteBuffer(&ResourceSystemAllocators::ResourceAllocator);

				int32 typeInt = (int32)type;
				buffer->AppendBuffer(ReinterpretCast(byte*, &typeInt), 0, sizeof(int32));

				uint64 sizeInt = Buffer->GetSize();
				buffer->AppendBuffer(ReinterpretCast(byte*, &sizeInt), 0, sizeof(uint64));

				switch (type)
				{
				case ResourceTypes::Text:
				case ResourceTypes::Texture:
				case ResourceTypes::Shader:
					buffer->AppendBuffer(*Buffer);
					break;
				}

				return buffer;
			}

			Text *ResourceFactory::CreateText(ResourceTypes Type, uint64 Size, const byte *const Data)
			{
				wstr data = ResourceSystemAllocators::Allocate<char16>(Size + 1);
				CharacterUtility::ChangeType(Data, data);

				Text *text = ResourceSystemAllocators::Allocate<Text>(1);
				Construct(text, data);

				ResourceSystemAllocators::Deallocate(data);

				return text;
			}

			Texture *ResourceFactory::CreateTexture(ResourceTypes Type, uint64 Size, const byte *const Data)
			{
				return RenderingManager::GetInstance()->GetActiveDevice()->CreateTexture2D(Data, 10, 10);
			}

			Program *ResourceFactory::CreateShader(ResourceTypes Type, uint64 Size, const byte *const Data)
			{
				auto data = ReinterpretCast(cstr, Data);

				return RenderingManager::GetInstance()->GetActiveDevice()->CreateProgram(data);
			}

			ResourceFactory::ResourceTypes ResourceFactory::GetTypeByExtension(const WString &Extension)
			{
				if (Extension == L".txt")
					return ResourceTypes::Text;

				if (Extension == L".png")
					return ResourceTypes::Texture;

				if (Extension == L".shader")
					return ResourceTypes::Shader;

				return ResourceTypes::Unknown;
			}
		}
	}
}