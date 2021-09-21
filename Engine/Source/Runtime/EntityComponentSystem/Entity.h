// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef ENTITY_H
#define ENTITY_H

#include <Common\PrimitiveTypes.h>

namespace Engine
{
	using namespace Common;

	namespace EntityComponentSystem
	{
		namespace Private
		{
			class EntityCache;
		}

		class ENTITYCOMPONENTSYSTEM_API Entity
		{
			friend class Private::EntityCache;

		public:
			Entity(void) :
				Value(0)
			{
			}

			Entity(const Entity& Other) :
				Value(Other.Value)
			{
			}

		private:
			Entity(uint32 Value) :
				Value(Value)
			{
			}

		public:
			Entity& operator=(const Entity& Other)
			{
				Value = Other.Value;
				return *this;
			}

			bool operator==(const Entity& Other) const
			{
				return (Value == Other.Value);
			}

			bool operator!=(const Entity& Other) const
			{
				return (Value != Other.Value);
			}

		public:
			static const Entity Null;

		private:
			uint32 Value;
		};
	}
}

#endif