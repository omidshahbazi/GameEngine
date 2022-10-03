// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <Containers\Strings.h>
#include <Containers\StringUtility.h>
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
			Exception(Categories CategoryFlags, const String& What, const String& File, uint32 LineNumber, const String& Function) :
				m_CategoryFlags(CategoryFlags),
				m_What(What),
				m_File(File),
				m_LineNumber(LineNumber),
				m_Function(Function)
			{
			}

			Exception(Categories CategoryFlags, const String& What, const String& Info, const String& File, uint32 LineNumber, const String& Function) :
				m_CategoryFlags(CategoryFlags),
				m_What(What),
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

			const String& GetWhat(void) const
			{
				return m_What;
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

			virtual String ToString(void) const
			{
				return m_What + "{" + m_Info + "} in " + m_File + ":Ln" + StringUtility::ToString<char8>(m_LineNumber);
			}

		protected:
			void SetCategoryFlags(Categories Value)
			{
				m_CategoryFlags = Value;
			}

			void SetWhat(const String& Value)
			{
				m_What = Value;
			}

			void SetInfo(const String& Value)
			{
				m_Info = Value;
			}

			void SetFile(const String& Value)
			{
				m_File = Value;
			}

			void SetLineNumber(uint32 Value)
			{
				m_LineNumber = Value;
			}

			void SetFunction(const String& Value)
			{
				m_Function = Value;
			}

		private:
			Categories m_CategoryFlags;
			String m_What;
			String m_Info;
			String m_File;
			uint32 m_LineNumber;
			String m_Function;
		};

		class NotImplementedException : public Exception
		{
		public:
			NotImplementedException(Categories CategoryFlags, const String& File, uint32 LineNumber, const String& Function) :
				Exception(CategoryFlags, "Not Implemented", File, LineNumber, Function)
			{
			}
		};

#define THROW_EXCEPTION(CategoryFlags, Message) throw Engine::Containers::Exception(CategoryFlags, Message, DEBUG_ARGUMENTS)
#define THROW_FULL_EXCEPTION(CategoryFlags, Message, Info) throw Engine::Containers::Exception(CategoryFlags, Message, Info, DEBUG_ARGUMENTS)

#define THROW_IF_EXCEPTION(CategoryFlags, Condition, Message) \
		if (Condition) \
			THROW_FULL_EXCEPTION(CategoryFlags, Message, #Condition);

#define THROW_NOT_IMPLEMENTED_EXCEPTION(CategoryFlags) throw Engine::Containers::NotImplementedException(CategoryFlags, DEBUG_ARGUMENTS)
	}
}

#endif