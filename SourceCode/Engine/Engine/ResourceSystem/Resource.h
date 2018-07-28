// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef RESOURCE_H
#define RESOURCE_H

namespace Engine
{
	namespace Containers
	{
		class Buffer;
	}

	using namespace Containers;

	namespace ResourceSystem
	{
		class RESOURCESYSTEM_API Resource
		{
		public:
			enum class Types
			{
				Texture = 0
			};

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