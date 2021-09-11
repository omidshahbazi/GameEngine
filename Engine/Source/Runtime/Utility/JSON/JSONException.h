// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef JSON_EXCEPTION_H
#define JSON_EXCEPTION_H

#include <Containers\Exception.h>

namespace Engine
{
	using namespace Containers;

	namespace JSON
	{
		class JSONException : public Exception
		{
		public:
			JSONException(const String& What, uint32 LineNumber) :
				Exception(Categories::Utilities, What, "", LineNumber, "")
			{
			}
		};

#define THROW_JSON_EXCEPTION(What, Line) throw JSONException(What, Line)
	}
}
#endif
