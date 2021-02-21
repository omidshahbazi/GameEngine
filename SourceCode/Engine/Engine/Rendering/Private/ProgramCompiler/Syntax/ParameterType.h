// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PARAMETER_TYPE_H
#define PARAMETER_TYPE_H

#include <Rendering\Private\ProgramCompiler\Syntax\VariableType.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				namespace Syntax
				{
					class ParameterType : public VariableType
					{
					};

					typedef Vector<ParameterType*> ParameterList;
				}
			}
		}
	}
}

#endif