// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <JSON\JSONObject.h>

namespace Engine
{
	namespace JSON
	{
		JSONObject::JSONObject(AllocatorBase* Allocator) :
			Map<String, JSONData>(Allocator)
		{
		}

		String JSONObject::ToString(void) const
		{
			String json;

			json += '{';

			bool isFirstOne = true;
			for (auto& item : *this)
			{
				if (!isFirstOne)
					json += ',';

				json += '"';

				json += item.GetFirst();

				json += "\":";

				json += JSONBasic::ToString(&item.GetSecond());

				isFirstOne = false;
			}

			json += '}';

			return json;
		}
	}
}