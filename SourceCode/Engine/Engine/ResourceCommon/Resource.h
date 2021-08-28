// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_H
#define RESOURCE_H

#include <Common\PrimitiveTypes.h>
#include <Containers\GUID.h>

namespace Engine
{
	namespace Rendering
	{
		class Texture;
		class Sprite;
		class Program;
		class Material;
		class Mesh;
	}

	namespace FontSystem
	{
		class Font;
	}

	namespace ResourceSystem
	{
		class Text;

		namespace Private
		{
			class ResourceHolder;
		}
	}

	using namespace Common;
	using namespace Containers;
	using namespace Platform;
	using namespace ResourceSystem::Private;

	namespace ResourceCommon
	{
		enum class ResourceTypes
		{
			Text = 0,
			Texture = 1,
			Sprite = 2,
			Program = 3,
			Mesh = 4,
			Font = 5,
			Unknown
		};

		class ResourceBase
		{
			friend class ResourceHolder;

		public:
			virtual ~ResourceBase(void)
			{
			}

			const GUID& GetID(void) const
			{
				return m_ID;
			}

		private:
			void SetID(const GUID& ID)
			{
				m_ID = ID;
			}

		private:
			GUID m_ID;
		};

		template<typename T>
		class Resource : public ResourceBase
		{
		public:
			Resource(T* Resource = nullptr) :
				m_Resource(Resource)
			{
			}

			T* GetPointer(void)
			{
				return m_Resource;
			}

			bool IsNull(void) const
			{
				return (m_Resource == nullptr);
			}

			void Swap(T* Resource)
			{
				m_Resource = Resource;
			}

			T* operator *(void)
			{
				return m_Resource;
			}

			T* operator ->(void)
			{
				return m_Resource;
			}

		private:
			T* m_Resource;
		};

		typedef Resource<Rendering::Texture> TextureResource;
		typedef Resource<Rendering::Sprite> SpriteResource;
		typedef Resource<Rendering::Program> ProgramResource;
		typedef Resource<Rendering::Material> MaterialResource;
		typedef Resource<Rendering::Mesh> MeshResource;
		typedef Resource<ResourceSystem::Text> TextResource;
		typedef Resource<FontSystem::Font> FontResource;

		template<class Type>
		struct ResourceTypeSpecifier
		{
		public:
			static constexpr ResourceTypes Type = ResourceTypes::Unknown;
		};

		template<>
		struct ResourceTypeSpecifier<ResourceSystem::Text>
		{
		public:
			static constexpr ResourceTypes Type = ResourceTypes::Text;
		};

		template<>
		struct ResourceTypeSpecifier<Rendering::Texture>
		{
		public:
			static constexpr ResourceTypes Type = ResourceTypes::Texture;
		};

		template<>
		struct ResourceTypeSpecifier<Rendering::Program>
		{
		public:
			static constexpr ResourceTypes Type = ResourceTypes::Program;
		};

		template<>
		struct ResourceTypeSpecifier<Rendering::Mesh>
		{
		public:
			static constexpr ResourceTypes Type = ResourceTypes::Mesh;
		};

		template<>
		struct ResourceTypeSpecifier<Rendering::Sprite>
		{
		public:
			static constexpr ResourceTypes Type = ResourceTypes::Sprite;
		};

		template<>
		struct ResourceTypeSpecifier<FontSystem::Font>
		{
		public:
			static constexpr ResourceTypes Type = ResourceTypes::Font;
		};
	}
}

#endif