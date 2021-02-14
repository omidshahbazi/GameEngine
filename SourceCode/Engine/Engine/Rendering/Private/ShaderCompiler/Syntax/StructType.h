// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef STRUCT_TYPE_H
#define STRUCT_TYPE_H

#include <Rendering\Private\ShaderCompiler\Syntax\VariableType.h>
#include <Rendering\Private\ShaderCompiler\Syntax\SyntaxItemHolder.h>

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
					class StructType : public Type, public SyntaxItemHolder<VariableType>
					{
					public:
						StructType(AllocatorBase* Allocator) :
							SyntaxItemHolder<VariableType>(Allocator)
						{
						}

						String ToString(void) const override
						{
							String result;

							result += "struct " + GetName();
							result += "\n{";

							for (auto& variable : GetItems())
								result += "\n" + variable->ToString();

							result += "\n};";

							return result;
						}
					};

					typedef Vector<StructType*> StructList;
				}
			}
		}
	}
}

#endif