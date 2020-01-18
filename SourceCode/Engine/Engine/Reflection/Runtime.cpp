// Copyright 2012-2015 ?????????????. All Rights Reserved.
#include <Reflection\Runtime.h>
#include <Private\RuntimeImplementation.h>

namespace Engine
{
	namespace Reflection
	{
		using namespace Private;

		void *Runtime::CreateInstance(const String &TypeName)
		{
			const DataStructureType const *type = RuntimeImplementation::GetDataStructureType(TypeName);

			Assert(type != nullptr, "Type doesn't exists");

			AnyDataType data = type->CreateInstance();

			if (data.GetValueTypes() == ValueTypes::VoidPointer)
				return data.GetAsVoid();

			return nullptr;
		}


		void *Runtime::CreateInstance(const String &TypeName, const AnyDataType &Argument)
		{
			const DataStructureType const *type = RuntimeImplementation::GetDataStructureType(TypeName);

			Assert(type != nullptr, " doesn't exists");

			AnyDataType data = type->CreateInstance(Argument);

			if (data.GetValueTypes() == ValueTypes::VoidPointer)
				return data.GetAsVoid();

			return nullptr;
		}


		void *Runtime::CreateInstance(const String &TypeName, const ArgumentsList &Arguments)
		{
			const DataStructureType const *type = RuntimeImplementation::GetDataStructureType(TypeName);

			Assert(type != nullptr, " doesn't exists");

			AnyDataType data = type->CreateInstance(Arguments);

			if (data.GetValueTypes() == ValueTypes::VoidPointer)
				return data.GetAsVoid();

			return nullptr;
		}
	}
}