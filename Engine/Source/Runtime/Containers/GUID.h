// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GUID_H
#define GUID_H

#include <Containers\Strings.h>
#include <Platform\PlatformOS.h>

namespace Engine
{
	using namespace Platform;

	namespace Containers
	{
		class CONTAINERS_API GUID
		{
		public:
			GUID(void);
			GUID(const PlatformOS::GUIDData& Data);
			GUID(const String& Value);

			GUID& operator=(const String& Value);

			bool operator==(const GUID& Other) const;
			bool operator!=(const GUID& Other) const;

			String ToString(void) const;

			static GUID Create(void);

			static const GUID Invalid;

		private:
			PlatformOS::GUIDData m_Data;
		};
	}
}

#endif