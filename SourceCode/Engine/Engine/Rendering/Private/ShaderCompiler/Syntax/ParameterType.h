// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PARAMETER_TYPE_H
#define PARAMETER_TYPE_H

#include <Rendering\Private\ShaderCompiler\Syntax\VariableType.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
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