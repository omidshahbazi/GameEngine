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
			virtual ~IDynamicModule(void)
			{
			}

			virtual void* Load(void) = 0;
			virtual bool Unload(void) = 0;
		};

#define DEFINE_DYNAMIC_MODULE_ENTRY_POINT(DynamicModuleType) \
		extern "C" \
		{ \
			EXPORT_API IDynamicModule* Create(AllocatorBase* Allocator) \
			{ \
				DynamicModuleType* ptr = ReinterpretCast(DynamicModuleType*, AllocateMemory(Allocator, sizeof(DynamicModuleType))); \
				ConstructMacro(DynamicModuleType, ptr); \
				return ptr; \
			} \
		}
	}
}

#endif