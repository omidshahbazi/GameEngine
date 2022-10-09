// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef MAX_VERTEX_COUNT_ATTRIBUTE_TYPE_H
#define MAX_VERTEX_COUNT_ATTRIBUTE_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\BaseAttributeType.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API MaxVertexCountAttributeType : public BaseAttributeType
			{
			public:
				void SetCount(uint16 Value)
				{
					m_Count = Value;
				}

				uint16 GetCount(void) const
				{
					return m_Count;
				}

				virtual String ToString(void) const override
				{
					String result = "[MaxVertexCount";

					result += "(";

					result += StringUtility::ToString<char8>(m_Count);

					result += ")]";

					return result;
				}

			private:
				uint16 m_Count;
			};
		}
	}
}

#endif