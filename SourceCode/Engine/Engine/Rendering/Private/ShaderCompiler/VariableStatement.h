// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef VARIABLE_STATEMENT_H
#define VARIABLE_STATEMENT_H

#include <Rendering\Private\ShaderCompiler\Statement.h>
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
				class VariableStatement : public Statement
				{
				public:
					VariableStatement(void) :
						m_InitialStatement(nullptr)
					{
					}

					Types GetType(void) const override
					{
						return Types::Variable;
					}

					DataTypes GetDataType(void) const
					{
						return m_DataType;
					}

					void SetDataType(DataTypes DataType)
					{
						m_DataType = DataType;
					}

					const String &GetName(void) const
					{
						return m_Name;
					}

					void SetName(const String &Name)
					{
						m_Name = Name;
					}

					Statement *GetInitialStatement(void) const
					{
						return m_InitialStatement;
					}

					void SetInitialStatement(Statement *InitialStatement)
					{
						m_InitialStatement = InitialStatement;
					}

				private:
					DataTypes m_DataType;
					String m_Name;
					Statement *m_InitialStatement;
				};
			}
		}
	}
}

#endif