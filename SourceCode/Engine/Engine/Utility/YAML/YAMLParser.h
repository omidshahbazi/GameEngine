// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef YAML_PARSER_H
#define YAML_PARSER_H

#include <YAML\YAMLObject.h>

namespace Engine
{
	using namespace Containers;

	namespace YAML
	{
		class YAML_API YAMLParser
		{
		public:
			void Parse(const String& Value, YAMLObject& Object);
		};
	}
}

#endif