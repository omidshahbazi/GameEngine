// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef DOMAIN_ATTRIBUTE_TYPE_H
#define DOMAIN_ATTRIBUTE_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\BaseAttributeType.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			class PROGRAMPARSER_API DomainAttributeType : public BaseAttributeType
			{
			public:
				enum class Types
				{
					Triangle = 0,
					Quad,
					Isoline
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
					String result = "[Domain";

					result += "(";

					switch (m_Type)
					{
					case Types::Triangle:
						result += "Triangle";
						break;

					case Types::Quad:
						result += "Triangle";
						break;

					case Types::Isoline:
						result += "Isoline";
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