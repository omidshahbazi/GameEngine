// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\Runtime.h>
#include <Private\RuntimeImplementation.h>

namespace Engine
{
	namespace Reflection
	{
		using namespace Private;

		void* Runtime::CreateInstance(const String& FullQualifiedTypeName)
		{
			const DataStructureType const* type = RuntimeImplementation::GetDataStructureType(FullQualifiedTypeName);

			Assert(type != nullptr, "Type doesn't exists");

			AnyDataType data = type->CreateInstance();

			if (data.GetValueTypes() == ValueTypes::VoidPointer)
				return data.GetAsVoid();

			return nullptr;
		}


		void* Runtime::CreateInstance(const String& FullQualifiedTypeName, const AnyDataType& Argument)
		{
			const DataStructureType const* type = RuntimeImplementation::GetDataStructureType(FullQualifiedTypeName);

			Assert(type != nullptr, " doesn't exists");

			AnyDataType data = type->CreateInstance(Argument);

			if (data.GetValueTypes() == ValueTypes::VoidPointer)
				return data.GetAsVoid();

			return nullptr;
		}


		void* Runtime::CreateInstance(const String& FullQualifiedTypeName, const ArgumentsList& Arguments)
		{
			const DataStructureType const* type = RuntimeImplementation::GetDataStructureType(FullQualifiedTypeName);

			Assert(type != nullptr, " doesn't exists");

			AnyDataType data = type->CreateInstance(Arguments);

			if (data.GetValueTypes() == ValueTypes::VoidPointer)
				return data.GetAsVoid();

			return nullptr;
		}
	}
}