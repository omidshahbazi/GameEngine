// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <Containers\Strings.h>
#include <Common\PrimitiveTypes.h>
#include <Common\Categories.h>

namespace Engine
{
	using namespace Common;

	namespace Containers
	{
		class Exception
		{
		public:
			Exception(Categories CategoryFlags, const String& Message, const String& File, uint32 LineNumber, const String& Function) :
				m_CategoryFlags(CategoryFlags),
				m_Message(Message),
				m_File(File),
				m_LineNumber(LineNumber),
				m_Function(Function)
			{
			}

			Exception(Categories CategoryFlags, const String& Message, const String& Info, const String& File, uint32 LineNumber, const String& Function) :
				m_CategoryFlags(CategoryFlags),
				m_Message(Message),
				m_Info(Info),
				m_File(File),
				m_LineNumber(LineNumber),
				m_Function(Function)
			{
			}

			Categories GetCategoryFlags(void) const
			{
				return m_CategoryFlags;
			}

			const String& GetMessage(void) const
			{
				return m_Message;
			}

			const String& GetInfo(void) const
			{
				return m_Info;
			}

			const String& GetFile(void) const
			{
				return m_File;
			}

			uint32 GetLineNumber(void) const
			{
				return m_LineNumber;
			}

			const String& GetFunction(void) const
			{
				return m_Function;
			}

		private:
			Categories m_CategoryFlags;
			String m_Message;
			String m_Info;
			String m_File;
			uint32 m_LineNumber;
			String m_Function;
		};

#define THROW_EXCEPTION(CategoryFlags, Message) throw Exception(CategoryFlags, Message, DEBUG_ARGUMENTS)
#define THROW_FULL_EXCEPTION(CategoryFlags, Message, Info) throw Exception(CategoryFlags, Message, Info, DEBUG_ARGUMENTS)

#define THROW_IF_EXCEPTION(CategoryFlags, Condition, Message) \
		if (!(Condition)) \
			THROW_EXCEPTION(CategoryFlags, Message, #Condition);
	}
}

#endif