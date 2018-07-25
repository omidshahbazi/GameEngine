// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_H
#define RESOURCE_H

namespace Engine
{
	namespace ResourceSystem
	{
		class Buffer;

		class RESOURCESYSTEM_API Resource
		{
		public:
			Resource(Buffer *Buffer) :
				m_Buffer(Buffer)
			{
			}

		private:
			Buffer *m_Buffer;
		};
	}
}

#endif