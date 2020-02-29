// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_FARCTORY_H
#define RESOURCE_FARCTORY_H

#include <Containers\Strings.h>
#include <MemoryManagement\Singleton.h>
#include <Containers\Buffer.h>
#include <ResourceSystem\Text.h>
#include <ResourceSystem\Private\ResourceSystemAllocators.h>
#include <ResourceSystem\Enumerators.h>

namespace Engine
{
	namespace Rendering
	{
		class Texture;
		class Program;
		class Mesh;
	}

	namespace FontSystem
	{
		class Font;
	}

	using namespace Containers;
	using namespace Rendering;
	using namespace FontSystem;

	namespace ResourceSystem
	{
		class ResourceManager;

		template<typename T>
		class Resource;

		namespace Private
		{
			class RESOURCESYSTEM_API ResourceFactory
			{
				SINGLETON_DECLARATION(ResourceFactory)

					friend class ResourceManager;

			private:
				enum class FileTypes
				{
					TXT = 0,
					PNG = 1,
					JPG = 2,
					SHADER = 3,
					OBJ = 4,
					FONT = 5,
					Unknown
				};

				enum class ResourceTypes
				{
					Text = 0,
					Texture = 1,
					Shader = 2,
					Model = 3,
					Font = 4,
					Unknown
				};

			private:
				ResourceFactory(void);
				~ResourceFactory(void);

			private:
				bool Compile(const WString& Extension, ByteBuffer& OutBuffer, const ByteBuffer& InBuffer, ResourceTypes& Type);

				void CompileImageFile(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer);
				void CompileOBJFile(ByteBuffer& OutBuffer, const ByteBuffer& InBuffer);

				Text* CreateText(const ByteBuffer& Buffer);
				void DestroyText(Text* Text);

				Texture* CreateTexture(const ByteBuffer& Buffer);
				void DestroyTexture(Texture* Texture);

				Program* CreateProgram(const ByteBuffer& Buffer);
				void DestroyProgram(Program* Program);

				Mesh* CreateModel(const ByteBuffer& Buffer);
				void DestroyMesh(Mesh* Mesh);

				Font* CreateFont(const ByteBuffer& Buffer);
				void DestroyFont(Font* Font);

				template<typename T>
				T* Create(const ByteBuffer& Buffer)
				{
					ResourceTypes resType = (ResourceTypes)Buffer.ReadValue<int32>(0);
					//uint64 size = Buffer.ReadValue<uint64>(4);
					uint64 size = Buffer.GetSize() - 12;

					auto data = ConstCast(byte*, Buffer.ReadValue(12, size));

					ByteBuffer buffer(data, size);

					T* ptr = nullptr;

					switch (resType)
					{
					case ResourceTypes::Text:
						ptr = ReinterpretCast(T*, CreateText(buffer));
						break;

					case ResourceTypes::Texture:
						ptr = ReinterpretCast(T*, CreateTexture(buffer));
						break;

					case ResourceTypes::Shader:
						ptr = ReinterpretCast(T*, CreateProgram(buffer));
						break;

					case ResourceTypes::Model:
						ptr = ReinterpretCast(T*, CreateModel(buffer));
						break;

					case ResourceTypes::Font:
						ptr = ReinterpretCast(T*, CreateFont(buffer));
						break;
					}

					return ptr;
				}

				Mesh* CreatePrimitiveMesh(PrimitiveMeshTypes Type);

				static FileTypes GetFileTypeByExtension(const WString& Extension);
				static ResourceTypes GetResourceTypeByFileType(FileTypes FileType);
			};
		}
	}
}

#endif