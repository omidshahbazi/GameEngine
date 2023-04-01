// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef AST_PARAMETER_TYPE_H
#define AST_PARAMETER_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\VariableType.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class FunctionType;

			class PROGRAMPARSER_API ParameterType : public VariableType
			{
			public:
				ParameterType(FunctionType* Parent, AllocatorBase* Allocator) :
					VariableType(Allocator),
					m_Parent(Parent)
				{
				}

				const FunctionType* GetParent(void) const
				{
					return m_Parent;
				}

				virtual String ToString(void) const override
				{
					String result;

					result += GetDataType()->ToString();
					result += ' ';
					result += GetName();
					result += GetDataType()->PostElementCountToString();

					return result;
				}

			private:
				FunctionType* m_Parent;
			};

			typedef Vector<ParameterType*> ParameterList;
		}
	}
}

#endif