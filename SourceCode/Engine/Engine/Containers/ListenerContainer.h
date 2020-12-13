// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef LISTENER_CONTAINER_H
#define LISTENER_CONTAINER_H

#include <Containers\Vector.h>

#define LISTENER_DECLARATION(ListenerType) \
		protected: \
			Vector<ListenerType*> m_##ListenerType##_List; \
		public: \
			INLINE void AddListener(ListenerType *Listener) \
			{ \
				if (!m_##ListenerType##_List.Contains(Listener)) \
					m_##ListenerType##_List.Add(Listener); \
			} \
			INLINE void RemoveListener(ListenerType *Listener) \
			{ \
				if (m_##ListenerType##_List.Contains(Listener)) \
					m_##ListenerType##_List.Remove(Listener); \
			}

#define CALL_CALLBACK(ListenerType, Name, ...) \
for (auto listener : m_##ListenerType##_List) \
	listener->Name(__VA_ARGS__);

#endif