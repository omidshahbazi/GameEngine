// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef LISTENER_CONTAINER_H
#define LISTENER_CONTAINER_H

#include <Containers\Vector.h>

#define LISTENER_DECLARATION(ListenerType) \
		private: \
			typedef Vector<ListenerType*> ListernerList; \
			ListernerList m_Listeners; \
		public: \
			INLINE void AddListener(ListenerType *Listener) \
			{ \
				if (m_Listeners.Contains(Listener)) \
					return; \
				m_Listeners.Add(Listener); \
			} \
			INLINE void RemoveListener(ListenerType *Listener) \
			{ \
				if (m_Listeners.Contains(Listener)) \
					m_Listeners.Remove(Listener); \
			}

#define CALL_CALLBACK(Name, ...) \
for each (auto listener in m_Listeners) \
	listener->Name(__VA_ARGS__);

#endif