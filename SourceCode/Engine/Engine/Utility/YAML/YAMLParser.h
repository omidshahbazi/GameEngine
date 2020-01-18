// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef YAML_PARSER_H
#define YAML_PARSER_H

#include <Utility\YAML\YAMLObject.h>

namespace Engine
{
	using namespace Containers;

	namespace Utility
	{
		namespace YAML
		{
			class UTILITY_API YAMLParser
			{
			public:
				void Parse(const String &Value, YAMLObject &Object);
			};
		}
	}
}

#endif