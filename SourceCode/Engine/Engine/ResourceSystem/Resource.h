// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_H
#define RESOURCE_H

#include <ResourceSystem\ResourceHandle.h>

namespace Engine
{
	namespace Rendering
	{
		class Texture;
		class Sprite;
		class Shader;
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

		enum class ResourceTypes
		{
			Text = 0,
			Texture = 1,
			Sprite = 2,
			Shader = 3,
			Mesh = 4,
			Font = 5,
			Unknown
		};

		template<typename T>
		class Resource
		{
		public:
			Resource(void) :
				m_Resource(nullptr)
			{
			}

			Resource(ResourceHandle<T>* Resource) :
				m_Resource(Resource)
			{
				if (m_Resource != nullptr)
					m_Resource->Grab();
			}

			Resource(const Resource<T>& Other) :
				m_Resource(Other.m_Resource)
			{
				if (m_Resource != nullptr)
					m_Resource->Grab();
			}

			~Resource(void)
			{
				if (m_Resource == nullptr || m_Resource->GetReferenceCount() == 0)
					return;

				m_Resource->Drop();

				m_Resource == nullptr;
			}

			ResourceHandle<T>* GetData(void)
			{
				return m_Resource;
			}

			Resource<T>& operator =(ResourceHandle<T>* Resource)
			{
				if (m_Resource != nullptr)
					m_Resource->Drop();

				m_Resource = Resource;

				if (m_Resource != nullptr)
					m_Resource->Grab();

				return *this;
			}

			Resource<T>& operator =(const Resource<T>& Other)
			{
				if (m_Resource != nullptr)
					m_Resource->Drop();

				m_Resource = Other.m_Resource;

				m_Resource->Grab();

				return *this;
			}

			ResourceHandle<T>* operator *(void)
			{
				if (m_Resource == nullptr)
					return nullptr;

				return m_Resource;
			}

			T* operator ->(void)
			{
				return m_Resource->GetData();
			}

		private:
			ResourceHandle<T>* m_Resource;
		};

		typedef Resource<Rendering::Texture> TextureResource;
		typedef Resource<Rendering::Sprite> SpriteResource;
		typedef Resource<Rendering::Shader> ShaderResource;
		typedef Resource<Rendering::Material> MaterialResource;
		typedef Resource<Rendering::Mesh> MeshResource;
		typedef Resource<Text> TextResource;
		typedef Resource<FontSystem::Font> FontResource;

		template<class Type>
		struct ResourceTypeSpecifier
		{
		public:
			static constexpr ResourceTypes Type = ResourceTypes::Unknown;
		};

		template<>
		struct ResourceTypeSpecifier<Text>
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
		struct ResourceTypeSpecifier<Rendering::Shader>
		{
		public:
			static constexpr ResourceTypes Type = ResourceTypes::Shader;
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