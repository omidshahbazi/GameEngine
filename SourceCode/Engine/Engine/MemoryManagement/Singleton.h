// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#include <Common\PrimitiveTypes.h>
#include <Allocator\AllocatorBase.h>
#include <new>

#ifndef SINGLETON_H
#define SINGLETON_H

namespace Engine
{
	namespace MemoryManagement
	{
#define SINGLETON_DEFINITION(Type) \
		public: \
			INLINE static Type *Create(Allocator::AllocatorBase *Allocator) \
			{ \
				if (m_Instance == nullptr) \
				{ \
					m_Allocator = Allocator; \
					m_Instance = reinterpret_cast<Type*>(AllocateMemory(m_Allocator, sizeof(Type))); \
					new (m_Instance) Type(); \
				} \
				return m_Instance; \
			} \
			INLINE static void Destroy(void) \
			{ \
				m_Allocator->Deallocate(reinterpret_cast<Common::byte*>(m_Instance)); \
			} \
			INLINE static Type *GetInstance(void) \
			{ \
				return m_Instance; \
			} \
		private: \
			static Allocator::AllocatorBase *m_Allocator; \
			static Type *m_Instance; \

#define SINGLETON_DECLARATION(Type) \
		Allocator::AllocatorBase *Type::m_Allocator = nullptr; \
		Type *Type::m_Instance = nullptr;
	}
}

#endif