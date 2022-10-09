// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BASE_ATTRIBUTE_TYPE_H
#define BASE_ATTRIBUTE_TYPE_H

#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API BaseAttributeType
			{
			public:
				BaseAttributeType(void)
				{
				}

				virtual String ToString(void) const = 0;
			};

			typedef Vector<BaseAttributeType*> AttributeList;
		}
	}
}

#endif