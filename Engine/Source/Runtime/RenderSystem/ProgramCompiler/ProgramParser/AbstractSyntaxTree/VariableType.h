// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef AST_VARIABLE_TYPE_H
#define AST_VARIABLE_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\Type.h>
#include <ProgramParser\AbstractSyntaxTree\DataTypeStatement.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API VariableType : public Type
			{
			public:
				VariableType(AllocatorBase* Allocator) :
					Type(Allocator),
					m_DataType(nullptr)
				{
				}

				~VariableType(void)
				{
					Destruct(m_DataType);
				}

				DataTypeStatement* GetDataType(void) const
				{
					return m_DataType;
				}

				void SetDataType(DataTypeStatement* DataType)
				{
					m_DataType = DataType;
				}

			private:
				DataTypeStatement* m_DataType;
			};

			typedef Vector<VariableType*> VariableList;
		}
	}
}

#endif