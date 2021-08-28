// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <JSON\JSONArray.h>
#include <JSON\JSONObject.h>

namespace Engine
{
	namespace JSON
	{
		JSONArray::JSONArray(AllocatorBase* Allocator) :
			Vector<JSONData>(Allocator)
		{
		}

		String JSONArray::ToString(void) const
		{
			String json;

			json += '[';

			bool isFirstOne = true;
			for (auto& item : *this)
			{
				if (!isFirstOne)
					json += ',';

				json += JSONBasic::ToString(&item);

				isFirstOne = false;
			}

			json += ']';

			return json;
		}
	}
}