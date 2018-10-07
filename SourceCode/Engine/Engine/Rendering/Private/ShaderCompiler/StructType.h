// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef STRUCT_TYPE_H
#define STRUCT_TYPE_H

#include <Rendering\Private\ShaderCompiler\VariableType.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class StructType : public Type
				{
				public:
					StructType(void)
					{
					}

					void AddVariable(VariableType *Variable)
					{
						m_Variables.Add(Variable);
					}

					const VariableList &GetVariables(void) const
					{
						return m_Variables;
					}

					String ToString(void) const override
					{
						String result;

						result += "struct " + GetName() + "\n{";

						for each (auto var in m_Variables)
							result += "\t" + var->ToString() + "\n";

						result += "};";

						return result;
					}

				private:
					VariableList m_Variables;
				};
			}
		}
	}
}

#endif