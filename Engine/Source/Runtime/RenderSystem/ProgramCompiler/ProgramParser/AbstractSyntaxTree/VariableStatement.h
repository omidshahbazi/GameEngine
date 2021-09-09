// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef VARIABLE_STATEMENT_H
#define VARIABLE_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\DataTypeStatement.h>
#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API VariableStatement : public Statement
			{
			public:
				VariableStatement(AllocatorBase* Allocator) :
					m_DataType(nullptr),
					m_Name(Allocator),
					m_InitialStatement(nullptr)
				{
				}

				virtual ~VariableStatement(void)
				{
					Destruct(m_DataType);

					if (m_InitialStatement != nullptr)
						Destruct(m_InitialStatement);
				}

				DataTypeStatement* GetDataType(void) const
				{
					return m_DataType;
				}

				void SetDataType(DataTypeStatement* DataType)
				{
					m_DataType = DataType;
				}

				const String& GetName(void) const
				{
					return m_Name;
				}

				void SetName(const String& Name)
				{
					m_Name = Name;
				}

				Statement* GetInitialStatement(void) const
				{
					return m_InitialStatement;
				}

				void SetInitialStatement(Statement* Value)
				{
					m_InitialStatement = Value;
				}

				virtual String ToString(void) const override
				{
					String result;

					result += m_DataType->ToString();

					result += " ";

					result += m_Name;

					if (m_InitialStatement != nullptr)
					{
						result += "=";
						result += m_InitialStatement->ToString();
					}

					return result;
				}

			private:
				DataTypeStatement* m_DataType;
				String m_Name;
				Statement* m_InitialStatement;
			};
		}
	}
}

#endif