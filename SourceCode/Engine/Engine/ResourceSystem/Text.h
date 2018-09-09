// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXT_H
#define TEXT_H

#include <Containers\Strings.h>

namespace Engine
{
	namespace Containers
	{
		template<typename T>
		class Buffer;
	}

	using namespace Containers;

	namespace ResourceSystem
	{
		class RESOURCESYSTEM_API Text
		{
		public:
			Text(void) = delete;

			Text(cwstr Data) :
				m_Data(Data)
			{
			}

			const WString &GetData(void) const
			{
				return m_Data;
			}

		private:
			WString m_Data;
		};
	}
}

#endif