// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PRIMITVE_TYPE_ATTRIBUTE_TYPE_H
#define PRIMITVE_TYPE_ATTRIBUTE_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\BaseAttributeType.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API PrimitiveTypeAttributeType : public BaseAttributeType
			{
			public:
				enum class Types
				{
					Point = 0,
					Line,
					Triangle,
					LineAdjacency,
					TriangleAdjacency
				};

			public:
				void SetType(Types Type)
				{
					m_Type = Type;
				}

				Types GetType(void) const
				{
					return m_Type;
				}

				virtual String ToString(void) const override
				{
					String result = "[PrimitiveType";

					result += "(";

					switch (m_Type)
					{
					case Types::Point:
						result += "Point";
						break;

					case Types::Line:
						result += "Line";
						break;

					case Types::Triangle:
						result += "Triangle";
						break;

					case Types::LineAdjacency:
						result += "LineAdjacency";
						break;

					case Types::TriangleAdjacency:
						result += "TriangleAdjacency";
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