// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_H
#define RESOURCE_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

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

		class ResourceBase
		{
		public:
			virtual ~ResourceBase(void)
			{
			}
		};

		template<typename T>
		class Resource : public ResourceBase
		{
		public:
			Resource(void) :
				m_Resource(nullptr),
				m_IsReady(false)
			{
			}

			Resource(T* Resource) :
				m_Resource(Resource),
				m_IsReady(false)
			{
			}

			Resource(const Resource<T>& Other)
			{
				*this = Other;
			}

			virtual ~Resource(void)
			{
			}

			void Swap(T* Resource)
			{
				m_Resource = Resource;
			}

			T* GetPointer(void)
			{
				return m_Resource;
			}

			T* operator *(void)
			{
				return m_Resource;
			}

			T* operator ->(void)
			{
				return m_Resource;
			}

			//void Lock(void)
			//{
			//	bool expected = false;
			//	m_IsReady.compare_exchange_weak(expected, true);
			//}

			//void Free(void)
			//{
			//	bool expected = true;
			//	m_IsReady.compare_exchange_weak(expected, false);
			//}

			bool IsReady(void) const
			{
				return m_IsReady.load();
			}

			bool IsNull(void) const
			{
				return (m_Resource == nullptr);
			}

			Resource<T>& operator = (const Resource<T>& Other)
			{
				m_Resource = Other.m_Resource;
				m_IsReady.store(Other.m_IsReady.load());

				return *this;
			}

		private:
			T* m_Resource;
			AtomicBool m_IsReady;
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