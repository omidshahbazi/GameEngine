// Copyright 2016-2020 ?????????????. All Rights Reserved.
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
#define CREATOR_DECLARATION(Type) \
		public: \
			INLINE static void Create(void) \
			{ \
				if (m_Instance != nullptr) \
					return; \
				m_Instance = new Type(); \
			} \
			private: \
				static Type *m_Instance;
		//m_Instance = ReinterpretCast(Type*, AllocateMemory(DefaultAllocator::GetInstance(), sizeof(Type))); \
				//new (m_Instance) Type(); \*

#define CREATOR_DEFINITION(Type) \
		Type *Type::m_Instance = nullptr;

#define SINGLETON_DECLARATION(Type) \
		public: \
			INLINE static Type *Create(MemoryManagement::Allocator::AllocatorBase *Allocator) \
			{ \
				if (__m_Instance == nullptr) \
				{ \
					__m_Allocator = Allocator; \
					__m_Instance = ReinterpretCast(Type*, AllocateMemory(__m_Allocator, sizeof(Type))); \
					new (__m_Instance) Type(); \
				} \
				return __m_Instance; \
			} \
			INLINE static void Destroy(void) \
			{ \
				DestructMacro(Type, __m_Instance); \
				DeallocateMemory(__m_Allocator, __m_Instance); \
				__m_Instance = nullptr; \
			} \
			INLINE static Type *GetInstance(void) \
			{ \
				Assert(__m_Instance != nullptr, #Type ## " doesn't created"); \
				return __m_Instance; \
			} \
		private: \
			static MemoryManagement::Allocator::AllocatorBase *__m_Allocator; \
			static Type *__m_Instance;

#define SINGLETON_DEFINITION(Type) \
		MemoryManagement::Allocator::AllocatorBase *Type::__m_Allocator = nullptr; \
		Type *Type::__m_Instance = nullptr;
	}
}

#endif