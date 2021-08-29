// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef SPECIFIERS_H
#define SPECIFIERS_H

#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace ReflectionTool
	{
		class REFLECTIONTOOL_API Specifiers
		{
		public:
			void AddSpecifier(const String &Value)
			{
				m_Specifiers.Add(Value);
			}

			const StringList &GetSpecifiers(void) const
			{
				return m_Specifiers;
			}

		private:
			StringList m_Specifiers;

		};
	}
}

#endif
