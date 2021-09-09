// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef PROGRAM_COMPILER_EXCEPTION_H
#define PROGRAM_COMPILER_EXCEPTION_H

#include <Containers\Exception.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramCompilerCommon
	{
		class ProgramCompilerException : public Exception
		{
		public:
			ProgramCompilerException(const String& What, const String& Info) :
				Exception(Categories::ProgramCompiler, What, Info, String::Empty, 0, String::Empty)
			{
			}

			virtual String ToString(void) const override
			{
				return GetWhat() + (GetInfo() == String::Empty ? String::Empty : " {" + GetInfo() + "}");
			}

		};

#define THROW_PROGRAM_COMPILER_EXCEPTION(What, Info) throw ProgramCompilerException(What, Info)
	}
}
#endif
