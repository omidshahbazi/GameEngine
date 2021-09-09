// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DELEGATE_H
#define DELEGATE_H

#include <Common\TypeTraits.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Common;

	namespace Containers
	{
		template<typename... Params>
		class Delegate
		{
		public:
			typedef std::function<void(Params...)> FunctionType;
			typedef std::shared_ptr<FunctionType> SharedFunctionType;

		public:
			void operator += (SharedFunctionType Handler)
			{
				m_Handlers.Add(Handler);
			}
			void operator -= (SharedFunctionType Handler)
			{
				m_Handlers.Remove(Handler);
			}

			Delegate& operator =(const Delegate&) = delete;

			void operator =(SharedFunctionType Handler)
			{
				m_Handlers.Clear();

				m_Handlers.Add(Handler);
			}

			void operator =(void*)
			{
				m_Handlers.Clear();
			}

			void operator()(Params... Arguments) const
			{
				for (auto handler : m_Handlers)
					(*handler)(Arguments...);
			}

			const Vector<SharedFunctionType>& GetHandlers(void) const
			{
				return m_Handlers;
			}

		private:
			Vector<SharedFunctionType> m_Handlers;
		};

#define DECLARE_MEMBER_EVENT_LISTENER(DeclaringType, Listener) std::shared_ptr<ExtractFunctionType<decltype(&DeclaringType::Listener)>::Type> EventListener_##Listener = std::make_shared<ExtractFunctionType<decltype(&DeclaringType::Listener)>::Type>(Attach(&DeclaringType::Listener, this))
#define DECLARE_GLOBAL_EVENT_LISTENER(Listener) auto EventListener_##Listener = std::make_shared<ExtractFunctionType<decltype(&Listener)>::Type>(Listener)
#define CREATE_LAMBDA_EVENT_LISTENER(DelegateType, Lambda) std::make_shared<DelegateType::FunctionType>(Lambda)
	}
}

#endif