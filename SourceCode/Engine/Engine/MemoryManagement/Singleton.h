// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef SINGLETON_H
#define SINGLETON_H

#include <Common\PrimitiveTypes.h>
#include <MemoryManagement\Allocator\AllocatorBase.h>
#include <new>

namespace Engine
{
	namespace MemoryManagement
	{
#define SINGLETON_DEFINITION(Type) \
		public: \
			INLINE static Type *Create(MemoryManagement::Allocator::AllocatorBase *Allocator) \
			{ \
				if (m_Instance == nullptr) \
				{ \
					m_Allocator = Allocator; \
					m_Instance = ReinterpretCast(Type*, AllocateMemory(m_Allocator, sizeof(Type))); \
					new (m_Instance) Type(); \
				} \
				return m_Instance; \
			} \
			INLINE static void Destroy(void) \
			{ \
				m_Instance->~Type(); \
				DeallocateMemory(m_Allocator, m_Instance); \
			} \
			INLINE static Type *GetInstance(void) \
			{ \
				return m_Instance; \
			} \
		private: \
			static MemoryManagement::Allocator::AllocatorBase *m_Allocator; \
			static Type *m_Instance; \

#define SINGLETON_DECLARATION(Type) \
		MemoryManagement::Allocator::AllocatorBase *Type::m_Allocator = nullptr; \
		Type *Type::m_Instance = nullptr;
	}
}

#endif