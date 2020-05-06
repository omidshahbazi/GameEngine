// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TEXT_H
#define TEXT_H

#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace ResourceSystem
	{
		namespace Private
		{
			class ResourceFactory;
		}

		using namespace Private;

		class RESOURCESYSTEM_API Text
		{
			friend class ResourceFactory;

		private:
			Text(const WString& Data) :
				m_Data(Data)
			{
			}

		public:
			const WString& GetData(void) const
			{
				return m_Data;
			}

		private:
			WString m_Data;
		};
	}
}

#endif