// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROMISE_H
#define PROMISE_H

#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformThread.h>
#include <Allocators\AllocatorBase.h>
#include <functional>
#include <atomic>
#include <memory>

namespace Engine
{
	using namespace Common;
	using namespace Platform;
	using namespace Allocators;

	namespace Containers
	{
		class PromiseBlockBase
		{
		public:
			typedef std::function<void(void)> CallbackType;

		public:
			PromiseBlockBase(AllocatorBase* Allocator = nullptr, int16 MustDoneCount = 1) :
				m_ReferenceCount(0),
				m_Allocator(Allocator),
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
				if (--m_ReferenceCount != 0)
					return;

				if (m_Allocator == nullptr)
					return;

				DeallocateMemory(m_Allocator, this);
			}

			void Reset(void)
			{
				m_DoneCount = 0;
			}

			void IncreaseDoneCount(void)
			{
				if (++m_DoneCount < m_MustDoneCount)
					return;

				if (m_Callback != nullptr)
					(*m_Callback)();
			}

			int16 GetMustDoneCount(void) const
			{
				return m_MustDoneCount;
			}

			int16 GetDoneCount(void) const
			{
				return m_DoneCount;
			}

			bool GetIsDone(void) const
			{
				return (m_DoneCount.load(std::memory_order::memory_order_acquire) == m_MustDoneCount);
			}

			void SetThen(const CallbackType& Callback)
			{
				if (GetIsDone())
				{
					if (Callback != nullptr)
						Callback();

					return;
				}

				m_Callback = std::make_shared<CallbackType>(Callback);
			}

		public:
			std::atomic_int16_t m_ReferenceCount;
			AllocatorBase* m_Allocator;
			int16 m_MustDoneCount;
			AtomicUInt32 m_DoneCount;
			std::shared_ptr<CallbackType> m_Callback;
		};

		template<typename T>
		class PromiseBlock : public PromiseBlockBase
		{
		public:
			PromiseBlock(AllocatorBase* Allocator = nullptr, int16 MustDoneCount = 1) :
				PromiseBlockBase(Allocator, MustDoneCount)
			{
			}

			void SetValue(const T& Value)
			{
				m_Value = Value;
			}

			T& GetValue(void)
			{
				return m_Value;
			}

			const T& GetValue(void) const
			{
				return m_Value;
			}

		private:
			T m_Value;
		};

		template<>
		class PromiseBlock<void> : public PromiseBlockBase
		{
		public:
			PromiseBlock(AllocatorBase* Allocator = nullptr, int16 MustDoneCount = 1) :
				PromiseBlockBase(Allocator, MustDoneCount)
			{
			}
		};

		template<typename T>
		class PromiseBase
		{
		public:
			PromiseBase(PromiseBlock<T>* Block) :
				m_Block(Block)
			{
				if (m_Block != nullptr)
					m_Block->Grab();
			}

			PromiseBase(const PromiseBase<T>& Other) :
				m_Block(Other.m_Block)
			{
				if (m_Block != nullptr)
					m_Block->Grab();
			}

			virtual ~PromiseBase(void)
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

			void Then(const PromiseBlockBase::CallbackType& Callback)
			{
				if (m_Block != nullptr)
					m_Block->SetThen(Callback);
			}

		public:
			PromiseBlock<T>* m_Block;
		};

		template<typename T>
		class Promise : public PromiseBase<T>
		{
		public:
			Promise(PromiseBlock<T>* Block) :
				PromiseBase<T>(Block)
			{
			}

			Promise(const Promise<T>& Other) :
				PromiseBase<T>(Other)
			{
			}

			virtual ~Promise(void)
			{
			}

			T& GetValue(void)
			{
				if (PromiseBase<T>::m_Block == nullptr)
				{
					static T t;
					return t;
				}

				return PromiseBase<T>::m_Block->GetValue();
			}

			const T& GetValue(void) const
			{
				if (PromiseBase<T>::m_Block == nullptr)
				{
					static T t;
					return t;
				}

				return PromiseBase<T>::m_Block->GetValue();
			}

			T& Wait(void)
			{
				while (!PromiseBase<T>::GetIsDone())
					PlatformThread::YieldThread();

				return GetValue();
			}

			const T& Wait(void) const
			{
				while (!PromiseBase<T>::GetIsDone())
				{
					PlatformThread::Sleep(1);
					PlatformThread::YieldThread();
				}

				return GetValue();
			}
		};

		template<>
		class Promise<void> : public PromiseBase<void>
		{
		public:
			Promise(PromiseBlock<void>* Block) :
				PromiseBase(Block)
			{
			}

			Promise(const Promise<void>& Other) :
				PromiseBase(Other)
			{
			}

			virtual ~Promise(void)
			{
			}

			void Wait(void)
			{
				while (!GetIsDone())
				{
					PlatformThread::Sleep(1);
					PlatformThread::YieldThread();
				}
			}
		};

		template<typename T>
		PromiseBlock<T>* AllocatePromiseBlock(AllocatorBase* Allocator, int16 MustDoneCount = 1)
		{
			PromiseBlock<T>* promiseBlock = ReinterpretCast(PromiseBlock<T>*, AllocateMemory(Allocator, sizeof(PromiseBlock<T>)));

			Construct(promiseBlock, Allocator, MustDoneCount);

			return promiseBlock;
		}
	}
}

#endif