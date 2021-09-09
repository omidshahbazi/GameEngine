// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PARAMETER_TYPE_H
#define PARAMETER_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\VariableType.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API ParameterType : public VariableType
			{
			public:
				ParameterType(AllocatorBase* Allocator) :
					VariableType(Allocator)
				{
				}
			};

			typedef Vector<ParameterType*> ParameterList;
		}
	}
}

#endif