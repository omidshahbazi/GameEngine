// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GLOBAL_VARIABLE_TYPE_H
#define GLOBAL_VARIABLE_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\VariableType.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API GlobalVariableType : public VariableType
			{
			public:
				GlobalVariableType(AllocatorBase* Allocator) :
					VariableType(Allocator)
				{
				}

				virtual String ToString(void) const override
				{
					String result;

					result += GetDataType()->ToString() + " " + GetName();

					return result;
				}
			};

			typedef Vector<GlobalVariableType*> GlobalVariableList;
		}
	}
}

#endif