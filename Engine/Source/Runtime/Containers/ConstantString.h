// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once

#ifndef CONSTANT_STRING_H
#define CONSTANT_STRING_H

#include <Common\CharacterUtility.h>
#include <Platform\PlatformMemory.h>
#include <MemoryManagement\ReferenceCounted.h>
#include <Containers\DynamicString.h>
#include <Containers\Private\ContainersAllocators.h>

namespace Engine
{
	using namespace Platform;

	namespace Containers
	{
		namespace Private
		{
			template<typename T>
			class SharedBlock
			{
			public:
				T* m_String;
				uint32 m_Length;

				REFERENCE_COUNTED_DEFINITION()
			};
		}

		using namespace Private;

		template<typename T>
		class ConstantString
		{
		public:
			typedef T CharType;

		public:
			ConstantString(const T Value) :
				m_Block(nullptr)
			{
				SetValue(&Value, 1);
			}

			ConstantString(const T* Value) :
				m_Block(nullptr)
			{
				SetValue(Value);
			}

			template<typename T>
			ConstantString(const ConstantString<T>& Value) :
				m_Block(nullptr)
			{
				SetValue(Value);
			}

			template<typename T>
			ConstantString(ConstantString<T>&& Value) :
				m_Block(nullptr)
			{
				Move(std::forward(Value));
			}

			template<typename T>
			ConstantString(const DynamicString<T>& Value) :
				m_Block(nullptr)
			{
				SetValue(Value.GetValue(), Value.GetLength());
			}

			~ConstantString(void)
			{
				Deallocate();
			}

			INLINE ConstantString<T>& operator = (const ConstantString<T>& Value) const
			{
				SetValue(Value);

				return *this;
			}

			INLINE ConstantString<T>& operator = (const DynamicString<T>& Value) const
			{
				SetValue(Value.GetValue(), Value.GetLength());

				return *this;
			}

			INLINE bool operator == (const T* Value) const
			{
				if (m_Block->m_String == Value)
					return true;

				uint32 length = CharacterUtility::GetLength(Value);

				if (m_Block->m_Length != length)
					return false;

				return CharacterUtility::AreEquals(m_Block->m_String, Value);
			}

			INLINE bool operator == (const ConstantString<T>& Value) const
			{
				if (m_Block == Value.m_Block)
					return true;

				if (m_Block->m_Length != Value.m_Block->m_Length)
					return false;

				return CharacterUtility::AreEquals(m_Block->m_String, Value.m_Block->m_String);
			}

			INLINE bool operator != (const T* Value) const
			{
				return !(*this == Value);
			}

			INLINE bool operator != (const ConstantString<T>& Value) const
			{
				return !(*this == Value);
			}

			INLINE const T* GetValue(void) const
			{
				return m_Block == nullptr ? nullptr : m_Block->m_String;
			}

			INLINE uint32 GetLength(void) const
			{
				return m_Block == nullptr ? 0 : m_Block->m_Length;
			}

		private:
			INLINE void SetValue(const T* Value)
			{
				SetValue(Value, CharacterUtility::GetLength(Value));
			}

			INLINE void SetValue(const T* Value, uint32 Length)
			{
				Drop();

				m_Block = Allocate(Length);

				PlatformMemory::Copy((byte*)Value, (byte*)m_Block->m_String, sizeof(T) * (Length + 1));
			}

			INLINE void SetValue(const ConstantString<T>& Value)
			{
				Drop();

				m_Block = Value.m_Block;
				m_Block->Grab();
			}

			INLINE void Move(ConstantString<T>& Value)
			{
				m_Block = Value.m_Block;

				Value.m_Block = nullptr;
			}

			INLINE void Deallocate(void)
			{
				Drop();
			}

			INLINE SharedBlock<T>* Allocate(uint32 Length)
			{
				SharedBlock<T>* block = ReinterpretCast(SharedBlock<T>*, ContainersAllocators::ConstStringAllocator_Allocate<byte>(sizeof(SharedBlock<T>) + (sizeof(T) * (Length + 1))));
				new (block) SharedBlock<T>();
				block->Grab();
				block->m_String = ReinterpretCast(T*, ReinterpretCast(byte, block) + sizeof(SharedBlock<T>));
				block->m_Length = Length;
				return block;
			}

			INLINE void Drop(void)
			{
				m_Block->Drop();

				if (m_Block->GetReferenceCount() == 0)
					if (m_Block != nullptr)
						ContainersAllocators::ConstStringAllocator_Deallocate(m_Block);
			}

		private:
			SharedBlock<T>* m_Block;
		};
	}
}

#endif