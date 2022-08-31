// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef REFLECTION_TOOL_EXCEPTION_H
#define REFLECTION_TOOL_EXCEPTION_H

#include <Containers\Exception.h>

namespace Engine
{
	using namespace Containers;

	namespace ReflectionGenerator
	{
		class ReflectionGeneratorException : public Exception
		{
		public:
			ReflectionGeneratorException(const String& What, uint32 LineNumber, uint32 ColumnNumber) :
				Exception(Categories::Utilities, What, "", LineNumber, ""),
				m_ColumnNumber(ColumnNumber)
			{
			}

			uint32 GetColumnNumber(void) const
			{
				return m_ColumnNumber;
			}

			virtual String ToString(void) const override
			{
				return GetWhat() + " at (" + StringUtility::ToString<char8>(GetLineNumber()) + ", " + StringUtility::ToString<char8>(m_ColumnNumber) + ")";
			}

		private:
			uint32 m_ColumnNumber;
		};

#define THROW_REFLECTION_TOOL_EXCEPTION(What) throw ReflectionGeneratorException(What, GetCurrentLineIndex(), GetCurrentColumnIndex())
	}
}
#endif
