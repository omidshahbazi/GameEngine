// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef TYPE_H
#define TYPE_H

#include <Containers\Strings.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				namespace Syntax
				{
					class Type
					{
					public:
						Type(void)
						{
						}

						virtual ~Type(void)
						{
						}

						void SetName(const String& Name)
						{
							m_Name = Name;
						}

						const String& GetName(void) const
						{
							return m_Name;
						}

						virtual String ToString(void) const = 0;

					private:
						String m_Name;
					};
				}
			}
		}
	}
}

#endif