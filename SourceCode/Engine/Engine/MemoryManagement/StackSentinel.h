// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef STACK_SENTINEL_H
#define STACK_SENTINEL_H

#include <Common\PrimitiveTypes.h>
#include <Debugging\Debug.h>

namespace Engine
{
	using namespace Common;
	using namespace Debugging;

	namespace MemoryManagement
	{
		class StackSentinel
		{
		public:
			StackSentinel(cstr File, uint32 LineNumber, cstr Function) :
				m_File(File),
				m_LineNumber(LineNumber),
				m_Function(Function)
			{
				for (uint8 i = 0; i < BUFFER_SIZE; ++i)
					m_Buffer[i] = i;
			}

			~StackSentinel(void)
			{
				for (uint8 i = 0;  i < BUFFER_SIZE; ++i)
					if (m_Buffer[i] != i)
						Debug::Assertion(false, "", "Stack Corrupted", m_File, m_LineNumber, m_Function);
			}

		private:
			static const uint8 BUFFER_SIZE = 511;
			byte m_Buffer[BUFFER_SIZE];
			cstr m_File;
			uint32 m_LineNumber;
			cstr m_Function;
		};

#ifdef DEBUG_MODE

#define STACK_BOGUS_CHECKER(Name) StackSentinel Name(DEBUG_ARGUMENTS)
#else
#define STACK_BOGUS_CHECKER(Name)
#endif
	}
}

#endif