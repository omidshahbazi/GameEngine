//// Copyright 2016-2020 ?????????????. All Rights Reserved.
//#pragma once
//
//#ifndef VIEW_H
//#define VIEW_H
//
//#include <EntityComponentSystem\Private\ComponentCache.h>
//
//namespace Engine
//{
//	namespace EntityComponentSystem
//	{
//		template <typename... RestComponentTypes>
//		class View;
//
//		template<>
//		class View<>
//		{
//		};
//
//		template<typename ThisComponentType, typename... RestComponentTypes>
//		class View<ThisComponentType, RestComponentTypes...> : private View<RestComponentTypes...>
//		{
//		public:
//			View(Private::ComponentCache<RestComponentTypes>&... Caches) :
//				m_Caches(&Caches)...
//			{
//
//			}
//
//		private:
//			Private::ComponentCache<RestComponentTypes> m_Caches...;
//		};
//	}
//}
//
//#endif