// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\Enum.h>
#include <Reflection\Private\RuntimeImplementation.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Debugging;

	namespace Reflection
	{
		using namespace Private;

		const EnumType* const Enum::GetType(const String& TypeName)
		{
			return RuntimeImplementation::GetEnumType(TypeName);
		}


		bool Enum::IsDefined(const String& TypeName, const String& Value)
		{
			const EnumType* const type = GetType(TypeName);

			THROW_IF_EXCEPTION(Categories::Reflection, type == nullptr, "Type doesn't exists");

			return IsDefined(type, Value);
		}


		bool Enum::IsDefined(const EnumType const* Type, const String& Value)
		{
			const EnumType::ItemsList& items = Type->GetItems();

			for (auto& item : items)
				if (item.GetName() == Value)
					return true;

			return false;
		}


		int32 Enum::Parse(const String& TypeName, const String& Value)
		{
			const EnumType* const type = GetType(TypeName);

			THROW_IF_EXCEPTION(Categories::Reflection, type == nullptr, "Type doesn't exists");

			return Parse(type, Value);
		}


		int32 Enum::Parse(const EnumType const* Type, const String& Value)
		{
			const EnumType::ItemsList& items = Type->GetItems();

			for (auto& item : items)
			{
				if (item.GetName() == Value)
					return item.GetValue();
			}

			return -1;
		}


		String Enum::ToString(const String& TypeName, int32 Value)
		{
			const EnumType* const type = GetType(TypeName);

			THROW_IF_EXCEPTION(Categories::Reflection, type == nullptr, "Type doesn't exists");

			return ToString(type, Value);
		}


		String Enum::ToString(const EnumType const* Type, int32 Value)
		{
			const EnumType::ItemsList& items = Type->GetItems();

			for (auto& item : items)
			{
				if (item.GetValue() == Value)
					return item.GetName();
			}

			return "";
		}
	}
}