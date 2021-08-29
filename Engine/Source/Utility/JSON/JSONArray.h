// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef JSON_ARRAY_H
#define JSON_ARRAY_H

#include <JSON\JSONBasic.h>
#include <Containers\Vector.h>

namespace Engine
{
	namespace JSON
	{
		class JSON_API JSONArray : public JSONBasic, public Vector<JSONData>
		{
		public:
			JSONArray(AllocatorBase* Allocator);

			String ToString(void) const override;
		};
	}
}

#endif