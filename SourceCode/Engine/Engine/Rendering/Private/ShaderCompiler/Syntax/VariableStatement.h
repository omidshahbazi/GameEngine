// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef VARIABLE_STATEMENT_H
#define VARIABLE_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Syntax\Statement.h>
#include <Containers\Strings.h>

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
					class VariableStatement : public Statement
					{
					public:
						VariableStatement(void) :
							m_InitialStatement(nullptr)
						{
						}

						virtual ~VariableStatement(void)
						{
							if (m_InitialStatement != nullptr)
								Destruct(m_InitialStatement);
						}

						const ShaderDataType& GetDataType(void) const
						{
							return m_DataType;
						}

						void SetDataType(const ShaderDataType& DataType)
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

					private:
						ShaderDataType m_DataType;
						String m_Name;
						Statement* m_InitialStatement;
					};
				}
			}
		}
	}
}

#endif