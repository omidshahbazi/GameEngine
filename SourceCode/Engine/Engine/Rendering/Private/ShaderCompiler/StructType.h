// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef STRUCT_TYPE_H
#define STRUCT_TYPE_H

#include <Containers\Strings.h>
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
				class StructType
				{
				public:
					StructType(void)
					{
					}

					void SetName(const String &Name)
					{
						m_Name = Name;
					}

					const String &GetName(void) const
					{
						return m_Name;
					}

					void AddVariable(VariableType *Variable)
					{
						m_Variables.Add(Variable);
					}

					const VariableList &GetVariables(void) const
					{
						return m_Variables;
					}

				private:
					String m_Name;
					VariableList m_Variables;
				};
			}
		}
	}
}

#endif