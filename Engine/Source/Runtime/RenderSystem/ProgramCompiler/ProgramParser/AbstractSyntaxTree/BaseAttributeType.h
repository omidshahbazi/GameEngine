// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef BASE_ATTRIBUTE_TYPE_H
#define BASE_ATTRIBUTE_TYPE_H

#include <Containers\Strings.h>
#include <BaseAttributeType.Reflection.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			REFLECTION_OBJECT(REFLECTION_ABSTRACT);
			class PROGRAMPARSER_API BaseAttributeType
			{
				BASEATTRIBUTETYPE_OBJECT();

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