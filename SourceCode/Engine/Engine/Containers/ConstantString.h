// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#ifndef CONSTANT_STRING_H
#define CONSTANT_STRING_H

#include <Common\StringUtils.h>
#include <MemoryManagement\Allocator\DynamicSizeAllocator.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Platform\PlatformMemory.h>
#include <MemoryManagement\ReferenceCountedInfo.h>

namespace Engine
{
	using namespace MemoryManagement::Allocator;
	using namespace Platform;

	namespace Containers
	{
		template<typename T>
		class ConstantString
		{
		private:
			class CONTAINERS_API SharedBlock
			{
			public:
				T *m_String;
				uint32 m_Length;

				REFERENCE_COUNTED_INFO(SharedBlock)
			};

		public:
			typedef T CharType;

			ConstantString(void) :
				m_Allocator(nullptr),
				m_Block(nullptr)
			{
			}

			ConstantString(AllocatorBase *Allocator) :
				m_Allocator(Allocator),
				m_Block(nullptr)
			{
			}

			ConstantString(const T Value) :
				m_Allocator(nullptr),
				m_Block(nullptr)
			{
				SetValue(&Value, 1);
			}

			ConstantString(AllocatorBase *Allocator, const T Value) :
				m_Allocator(Allocator),
				m_Block(nullptr)
			{
				SetValue(&Value, 1);
			}

			ConstantString(const T *Value) :
				m_Allocator(nullptr),
				m_Block(nullptr)
			{
				SetValue(Value);
			}

			ConstantString(AllocatorBase *Allocator, const T *Value) :
				m_Allocator(Allocator),
				m_Block(nullptr)
			{
				SetValue(Value);
			}

			ConstantString(const ConstantString<T> &Value) :
				m_Allocator(Value.m_Allocator),
				m_Block(nullptr)
			{
				SetValue(Value);
			}

			ConstantString(AllocatorBase *Allocator, const ConstantString<T> &Value) :
				m_Allocator(Allocator),
				m_Block(nullptr)
			{
				SetValue(Value);
			}

			ConstantString(ConstantString<T> &&Value) :
				m_Allocator(Value.m_Allocator),
				m_Block(nullptr)
			{
				Move(Value);
			}

			ConstantString(AllocatorBase *Allocator, ConstantString<T> &&Value) :
				m_Allocator(Allocator),
				m_Block(nullptr)
			{
				Move(Value);
			}

			~ConstantString(void)
			{
				Deallocate();
			}

			bool operator == (const T *Value) const
			{
				if (m_Block->m_String == Value)
					return true;

				uint32 length = StringUtils::GetLength(Value);

				if (m_Block->m_Length != length)
					return false;

				return StringUtils::AreEquals(m_Block->m_String, Value);
			}

			bool operator == (const ConstantString<T> &Value) const
			{
				if (m_Block == Value.m_Block)
					return true;

				if (m_Block->m_Length != Value.m_Block->m_Length)
					return false;

				return StringUtils::AreEquals(m_Block->m_String, Value.m_Block->m_String);
			}

			bool operator != (const T *Value) const
			{
				return !(*this == Value);
			}

			bool operator != (const ConstantString<T> &Value) const
			{
				return !(*this == Value);
			}

			const T *GetValue(void) const
			{
				return m_Block == nullptr ? nullptr : m_Block->m_String;
			}

			uint32 GetLength(void) const
			{
				return m_Block == nullptr ? 0 : m_Block->m_Length;
			}

		private:
			void SetValue(const T *Value)
			{
				SetValue(Value, StringUtils::GetLength(Value));
			}

			void SetValue(const T *Value, uint32 Length)
			{
				if (Length == 0)
				{
					m_Block = nullptr;

					return;
				}

				m_Block = Allocate(Length);

				PlatformMemory::Copy((byte*)Value, (byte*)m_Block->m_String, sizeof(T) * (Length + 1));
			}

			void SetValue(const ConstantString<T> &Value)
			{
				if (Value.m_Block->m_Length == 0)
				{
					m_Block = nullptr;

					return;
				}

				if (m_Allocator == Value.m_Allocator)
				{
					m_Block = Value.m_Block;
					m_Block->Grab();
				}
				else
				{
					m_Block = Allocate(Value.m_Block->m_Length);

					PlatformMemory::Copy((byte*)Value.m_Block->m_String, (byte*)m_Block->m_String, sizeof(T) * (m_Block->m_Length + 1));
				}
			}

			void Move(ConstantString<T> &Value)
			{
				if (m_Allocator != Value.m_Allocator)
				{
					SetValue(Value);

					return;
				}

				m_Block = Value.m_Block;

				Value.m_Block = nullptr;
			}

			void Deallocate(void)
			{
				Drop();
			}

			SharedBlock *Allocate(uint32 Length)
			{
				if (m_Allocator == nullptr)
				{
					static DynamicSizeAllocator allocator("Default ConstantString Allocator", RootAllocator::GetInstance(), 1024 * 1024);

					m_Allocator = &allocator;
				}

				SharedBlock *block = reinterpret_cast<SharedBlock*>(AllocateMemory(m_Allocator, sizeof(SharedBlock) + (sizeof(T) * (Length + 1))));
				new (block) SharedBlock();
				block->m_String = reinterpret_cast<T*>(reinterpret_cast<byte*>(block) + sizeof(SharedBlock));
				block->m_Length = Length;
				return block;
			}

			void Drop(void)
			{
				m_Block->Drop();

				if (m_Block->GetCount() == 0)
					if (m_Block != nullptr)
						m_Allocator->Deallocate((byte*)m_Block);
			}

		private:
			SharedBlock *m_Block;
			AllocatorBase *m_Allocator;
		};
	}
}

#endif