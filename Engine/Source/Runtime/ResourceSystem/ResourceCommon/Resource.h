// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_H
#define RESOURCE_H

#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformThread.h>
#include <Containers\GUID.h>
#include <Containers\Delegate.h>

namespace Engine
{
	namespace RenderSystem
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
		class ResourceHolder;
	}

	using namespace Common;
	using namespace Containers;
	using namespace Platform;
	using namespace ResourceSystem;

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
		public:
			typedef Delegate<Resource<T>*> OnUpdatedEventHandler;

		public:
			Resource(T* Resource = nullptr)
			{
				*this = Resource;
			}

			Resource(const Resource<T>& Other)
			{
				*this = Other;
			}

			T* GetPointer(void)
			{
				return m_Resource;
			}

			const T* GetPointer(void) const
			{
				return m_Resource;
			}

			bool IsNull(void) const
			{
				return (m_Resource == nullptr);
			}

			void Set(T* Resource)
			{
				m_Resource = Resource;
				m_Set = true;

				OnUpdated(this);
			}

			void Wait(void) const
			{
				while (!m_Set)
					PlatformThread::Sleep(1);
			}

			T* operator *(void)
			{
				return m_Resource;
			}

			const T* operator *(void) const
			{
				return m_Resource;
			}

			T* operator ->(void)
			{
				return m_Resource;
			}

			const T* operator ->(void) const
			{
				return m_Resource;
			}

			Resource& operator=(T* Other)
			{
				m_Resource = Other;
				m_Set = (m_Resource != nullptr);

				return *this;
			}

			Resource& operator=(const Resource<T>& Other)
			{
				m_Resource = Other.m_Resource;
				m_Set = (m_Resource != nullptr);

				return *this;
			}

		private:
			T* m_Resource;
			std::atomic_bool m_Set;

		public:
			OnUpdatedEventHandler OnUpdated;
		};

		typedef Resource<RenderSystem::Texture> TextureResource;
		typedef Resource<RenderSystem::Sprite> SpriteResource;
		typedef Resource<RenderSystem::Program> ProgramResource;
		typedef Resource<RenderSystem::Material> MaterialResource;
		typedef Resource<RenderSystem::Mesh> MeshResource;
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
		struct ResourceTypeSpecifier<RenderSystem::Texture>
		{
		public:
			static constexpr ResourceTypes Type = ResourceTypes::Texture;
		};

		template<>
		struct ResourceTypeSpecifier<RenderSystem::Program>
		{
		public:
			static constexpr ResourceTypes Type = ResourceTypes::Program;
		};

		template<>
		struct ResourceTypeSpecifier<RenderSystem::Mesh>
		{
		public:
			static constexpr ResourceTypes Type = ResourceTypes::Mesh;
		};

		template<>
		struct ResourceTypeSpecifier<RenderSystem::Sprite>
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