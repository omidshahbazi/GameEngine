// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef CONSTANT_ENTRYPOINT_ATTRIBUTE_TYPE_H
#define CONSTANT_ENTRYPOINT_ATTRIBUTE_TYPE_H

#include <ProgramParser\AbstractSyntaxTree\BaseAttributeType.h>
#include <Containers\StringUtility.h>
#include <ConstantEntrypointAttributeType.Reflection.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			REFLECTION_OBJECT(REFLECTION_ABSTRACT);
			class PROGRAMPARSER_API ConstantEntrypointAttributeType : public BaseAttributeType
			{
				CONSTANTENTRYPOINTATTRIBUTETYPE_OBJECT();

			public:
				void SetEntrypoint(const String& Value)
				{
					m_Entrypoint = Value;
				}

				const String& GetEntrypoint(void)
				{
					return m_Entrypoint;
				}

				virtual String ToString(void) const override
				{
					String result = "[ConstantEntrypoint";

					result += "(";

					result += m_Entrypoint;

					result += ")]";

					return result;
				}

			private:
				String m_Entrypoint;
			};
		}
	}
}

#endif