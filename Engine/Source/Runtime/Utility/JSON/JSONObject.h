// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef JSON_OBJECT_H
#define JSON_OBJECT_H

#include <JSON\JSONBasic.h>
#include <Containers\Map.h>

namespace Engine
{
	using namespace Containers;

	namespace JSON
	{
		class JSON_API JSONObject : public JSONBasic, public Map<String, JSONData>
		{
		public:
			JSONObject(AllocatorBase* Allocator);

			String ToString(void) const override;
		};
	}
}

#endif