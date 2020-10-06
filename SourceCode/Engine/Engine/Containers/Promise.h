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
			PromiseBlockBase(DeallocateProcedure DeallocateProcedure, uint32 MustDoneCount = 1) :
				m_ReferenceCount(0),
				m_DeallocateProcedure(DeallocateProcedure),
				m_MustDoneCount(MustDoneCount),
				m_DoneCount(0)
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

			void IncreaseDoneCount(void)
			{
				++m_DoneCount;
			}

			uint32 GetMustDoneCount(void) const
			{
				return m_MustDoneCount;
			}

			uint32 GetDoneCount(void) const
			{
				return m_DoneCount;
			}

			bool GetIsDone(void) const
			{
				return (m_DoneCount == m_MustDoneCount);
			}

		private:
			uint32 m_ReferenceCount;
			DeallocateProcedure m_DeallocateProcedure;
			uint32 m_MustDoneCount;
			AtomicUInt32 m_DoneCount;
		};

		template<typename T>
		class PromiseBlock : public PromiseBlockBase
		{
		public:
			PromiseBlock(DeallocateProcedure DeallocateProcedure, uint32 MustDoneCount = 1) :
				PromiseBlockBase(DeallocateProcedure, MustDoneCount),
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
			PromiseBlock(DeallocateProcedure DeallocateProcedure, uint32 MustDoneCount = 1) :
				PromiseBlockBase(DeallocateProcedure, MustDoneCount)
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
		PromiseBlock<T>* AllocatePromiseBlock(AllocatorBase* Allocator, PromiseBlockBase::DeallocateProcedure DeallocateProcedure, uint32 MustDoneCount = 1)
		{
			PromiseBlock<void>* promiseBlock = ReinterpretCast(PromiseBlock<T>*, AllocateMemory(Allocator, sizeof(PromiseBlock<void>)));

			Construct(promiseBlock, DeallocateProcedure, MustDoneCount);

			promiseBlock->Grab();

			return promiseBlock;
		}
	}
}

#endif