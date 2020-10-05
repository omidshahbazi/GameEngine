// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROMISE_H
#define PROMISE_H

#include <Common\PrimitiveTypes.h>
#include <functional>

namespace Engine
{
	using namespace Common;

	namespace Containers
	{
		class PromiseBlockBase
		{
		public:
			typedef std::function<void(PromiseBlockBase*)> DeallocateProcedure;

		public:
			PromiseBlockBase(DeallocateProcedure DeallocateProcedure) :
				m_ReferenceCount(0),
				m_DeallocateProcedure(DeallocateProcedure),
				m_IsDone(false)
			{
			}

			void Grab(void)
			{
				++m_ReferenceCount;
			}

			void Drop(void)
			{
				if (--m_ReferenceCount == 0)
					m_DeallocateProcedure(this);
			}

			void SetAsDone(void)
			{
				m_IsDone = true;
			}

			bool GetIsDone(void) const
			{
				return m_IsDone;
			}

		private:
			uint32 m_ReferenceCount;
			DeallocateProcedure m_DeallocateProcedure;
			AtomicBool m_IsDone;
		};

		template<typename T>
		class PromiseBlock : public PromiseBlockBase
		{
		public:
			PromiseBlock(DeallocateProcedure DeallocateProcedure) :
				PromiseBlockBase(DeallocateProcedure),
				m_Value(nullptr)
			{
			}

			void SetValue(T* Value)
			{
				m_Value = Value;
			}

			T* GetValue(void) const
			{
				return m_Value;
			}

		private:
			T* m_Value;
		};

		template<>
		class PromiseBlock<void> : public PromiseBlockBase
		{
		public:
			PromiseBlock(DeallocateProcedure DeallocateProcedure) :
				PromiseBlockBase(DeallocateProcedure)
			{
			}
		};

		template<typename T>
		class Promise
		{
		public:
			Promise(PromiseBlock<T>* Block) :
				m_Block(Block)
			{
				if (m_Block != nullptr)
					m_Block->Grab();
			}

			Promise(const Promise<T>& Other) :
				m_Block(Other.m_Block)
			{
				if (m_Block != nullptr)
					m_Block->Grab();
			}

			~Promise(void)
			{
				if (m_Block != nullptr)
					m_Block->Drop();
			}

			bool GetIsInvalid(void) const
			{
				return (m_Block == nullptr);
			}

			bool GetIsDone(void) const
			{
				if (m_Block == nullptr)
					return true;

				return m_Block->GetIsDone();
			}

			T* GetValue(void) const
			{
				if (m_Block == nullptr)
					return nullptr;

				return m_Block->GetValue();
			}

			void Wait(void)
			{
				while (!GetIsDone())
					PlatformThread::Sleep(1);
			}

		private:
			PromiseBlock<T>* m_Block;
		};

		template<typename T>
		PromiseBlock<T>* AllocatePromiseBlock(AllocatorBase* Allocator, PromiseBlockBase::DeallocateProcedure DeallocateProcedure)
		{
			PromiseBlock<void>* promiseBlock = ReinterpretCast(PromiseBlock<T>*, AllocateMemory(Allocator, sizeof(PromiseBlock<void>)));

			Construct(promiseBlock, DeallocateProcedure);

			promiseBlock->Grab();

			return promiseBlock;
		}
	}
}

#endif