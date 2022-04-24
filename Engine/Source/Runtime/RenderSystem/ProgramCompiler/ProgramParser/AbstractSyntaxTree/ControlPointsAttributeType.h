// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONTROL_POINTS_ATTRIBUTE_TYPE_H
#define CONTROL_POINTS_ATTRIBUTE_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\BaseAttributeType.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API ControlPointsAttributeType : public BaseAttributeType
			{
			public:
				void SetNumber(uint16 Value)
				{
					m_Number = Value;
				}

				uint16 GetNumber(void)
				{
					return m_Number;
				}

				String ToString(void) const override
				{
					String result = "[ControlPoints";

					result += "(";

					result += StringUtility::ToString<char8>(m_Number);

					result += ")]";

					return result;
				}

			private:
				uint16 m_Number;
			};
		}
	}
}

#endif