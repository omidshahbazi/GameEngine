// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef OBJ_PARSER_H
#define OBJ_PARSER_H

#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace ResourceSystem
	{
		namespace Private
		{
			namespace Parser
			{
				class RESOURCESYSTEM_API OBJParser
				{
				public:
					void Parse(const WString &Path);
				};
			}
		}
	}
}

#endif