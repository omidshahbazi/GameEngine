// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef VIEW_H
#define VIEW_H

#include <EntityComponentSystem\Entity.h>
#include <Allocators\AllocatorBase.h>
#include <EntityComponentSystem\Private\Iterator.h>

namespace Engine
{
	using namespace Allocators;

	namespace EntityComponentSystem
	{
		class Registry;

		template<typename... ComponentTypes>
		class View
		{
			friend class Registry;

		public:
			typedef Vector<Entity> ListType;

		private:
			View(AllocatorBase* Allocator) :
				m_Entities(Allocator)
			{
			}

			View(View&& Other) :
				m_Entities(std::move(Other.m_Entities))
			{
			}

			void Add(const Entity& Entity)
			{
				m_Entities.Add(Entity);
			}

		public:
			ListType::Iterator begin(void)
			{
				return m_Entities.GetBegin();
			}

			ListType::Iterator end(void)
			{
				return m_Entities.GetEnd();
			}

		private:
			ListType m_Entities;
		};
	}
}

#endif