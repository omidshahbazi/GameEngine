// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef I_DYNAMIC_MODULE_H
#define I_DYNAMIC_MODULE_H

#include <Containers\Strings.h>
#include <MemoryManagement\Allocator\AllocatorBase.h>

namespace Engine
{
	using namespace Containers;
	using namespace MemoryManagement::Allocator;

	namespace DynamicModuleDefinition
	{
		class IDynamicModule
		{
		public:
			virtual bool Load(void) = 0;
			virtual bool Unload(void) = 0;
		};

#define DEFINE_DYNAMIC_MODULE_ENTRY_POINT(DynamicModuleType) \
		extern "C" \
		{ \
			IDynamicModule* Create(AllocatorBase* Allocator) \
			{ \
				return ReinterpretCast(DynamicModuleType*, AllocateMemory(Allocator, sizeof(DynamicModuleType))); \
			} \
		}
	}
}

#endif