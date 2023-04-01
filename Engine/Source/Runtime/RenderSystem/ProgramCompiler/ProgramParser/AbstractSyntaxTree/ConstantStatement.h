// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONSTANT_STATEMENT_H
#define CONSTANT_STATEMENT_H

#include <ProgramParser\AbstractSyntaxTree\Statement.h>
#include <RenderCommon\Enums.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Containers;
	using namespace RenderCommon;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API ConstantStatement : public Statement
			{
			public:
				ConstantStatement(void) :
					m_Type(ProgramDataTypes::Unknown),
					m_Float32(0)
				{
				}

				ProgramDataTypes GetType(void) const
				{
					return m_Type;
				}

				bool GetBool(void) const
				{
					return m_Bool;
				}

				void SetBool(bool Value)
				{
					m_Type = ProgramDataTypes::Bool;
					m_Bool = Value;
				}

				int32 GetInt32(void) const
				{
					return m_Int32;
				}

				void SetInt32(int32 Value)
				{
					m_Type = ProgramDataTypes::Integer;
					m_Int32 = Value;
				}

				float32 GetFloat32(void) const
				{
					return m_Float32;
				}

				void SetFloat32(float32 Value)
				{
					m_Type = ProgramDataTypes::Float;
					m_Float32 = Value;
				}

				virtual String ToString(void) const override
				{
					if (m_Type == ProgramDataTypes::Bool)
						return (m_Bool ? "true" : "false");

					if (m_Type == ProgramDataTypes::Integer)
						return StringUtility::ToString<char8>(m_Int32);

					return StringUtility::ToString<char8>(m_Float32);
				}

			private:
				ProgramDataTypes m_Type;

				union
				{
					bool m_Bool;
					int32 m_Int32;
					float32 m_Float32;
				};
			};
		}
	}
}

#endif