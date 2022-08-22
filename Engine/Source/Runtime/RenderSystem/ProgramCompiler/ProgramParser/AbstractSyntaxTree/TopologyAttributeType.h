// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TOPOLOGY_ATTRIBUTE_TYPE_H
#define TOPOLOGY_ATTRIBUTE_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\BaseAttributeType.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API TopologyAttributeType : public BaseAttributeType
			{
			public:
				enum class Types
				{
					Point = 0,
					Line,
					TriangleClockwise,
					TriangleCounterClockwise
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

				virtual String ToString(void) const override
				{
					String result = "[Topology";

					result += "(";

					switch (m_Type)
					{
					case Types::Point:
						result += "Point";
						break;

					case Types::Line:
						result += "Line";
						break;

					case Types::TriangleClockwise:
						result += "TriangleClockwise";
						break;

					case Types::TriangleCounterClockwise:
						result += "TriangleCounterClockwise";
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