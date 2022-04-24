// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef THREAD_COUNT_ATTRIBUTE_TYPE_H
#define THREAD_COUNT_ATTRIBUTE_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\BaseAttributeType.h>
#include <Containers\Strings.h>
#include <Containers\StringUtility.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API ThreadCountAttributeType : public BaseAttributeType
			{
			public:
				void SetXCount(uint16 Value)
				{
					m_XCount = Value;
				}

				uint16 GetXCount(void)
				{
					return m_XCount;
				}

				void SetYCount(uint16 Value)
				{
					m_YCount = Value;
				}

				uint16 GetYCount(void)
				{
					return m_YCount;
				}

				void SetZCount(uint16 Value)
				{
					m_ZCount = Value;
				}

				uint16 GetZCount(void)
				{
					return m_ZCount;
				}

				String ToString(void) const override
				{
					String result = "[ThreadCount";

					result += "(";

					result += StringUtility::ToString<char8>(m_XCount);
					result += ", ";
					result += StringUtility::ToString<char8>(m_YCount);
					result += ", ";
					result += StringUtility::ToString<char8>(m_ZCount);

					result += ")]";

					return result;
				}

			private:
				uint16 m_XCount;
				uint16 m_YCount;
				uint16 m_ZCount;
			};
		}
	}
}

#endif