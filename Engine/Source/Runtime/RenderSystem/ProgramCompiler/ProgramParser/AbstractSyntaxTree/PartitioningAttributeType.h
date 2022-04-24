// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PARTITIONING_ATTRIBUTE_TYPE_H
#define PARTITIONING_ATTRIBUTE_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\BaseAttributeType.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API PartitioningAttributeType : public BaseAttributeType
			{
			public:
				enum class Types
				{
					Integer = 0,
					FractionalEven,
					FractionalOdd,
					PowerOfTwo
				};

			public:
				void SetType(Types Type)
				{
					m_Type = Type;
				}

				Types GetType(void)
				{
					return m_Type;
				}

				String ToString(void) const override
				{
					String result = "[Partitioning";

					result += "(";

					switch (m_Type)
					{
					case Types::Integer:
						result += "Integer";
						break;

					case Types::FractionalEven:
						result += "FractionalEven";
						break;

					case Types::FractionalOdd:
						result += "FractionalOdd";
						break;

					case Types::PowerOfTwo:
						result += "PowerOfTwo";
						break;
					}

					result += ")]";

					return result;
				}

			private:
				Types m_Type;
			};
		}
	}
}

#endif