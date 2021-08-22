// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\Runtime.h>
#include <Reflection\Private\RuntimeImplementation.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Debugging;

	namespace Reflection
	{
		using namespace Private;

		void* Runtime::CreateInstance(const String& FullQualifiedTypeName)
		{
			const DataStructureType const* type = RuntimeImplementation::GetDataStructureType(FullQualifiedTypeName);

			THROW_IF_EXCEPTION(Categories::Reflection, type != nullptr, "Type doesn't exists");

			AnyDataType data = type->CreateInstance();

			if (data.GetValueType() == ValueTypes::VoidPointer)
				return data.GetAsVoid();

			return nullptr;
		}


		void* Runtime::CreateInstance(const String& FullQualifiedTypeName, const AnyDataType& Argument)
		{
			const DataStructureType const* type = RuntimeImplementation::GetDataStructureType(FullQualifiedTypeName);

			THROW_IF_EXCEPTION(Categories::Reflection, type != nullptr, " doesn't exists");

			AnyDataType data = type->CreateInstance(Argument);

			if (data.GetValueType() == ValueTypes::VoidPointer)
				return data.GetAsVoid();

			return nullptr;
		}


		void* Runtime::CreateInstance(const String& FullQualifiedTypeName, const ArgumentsList& Arguments)
		{
			const DataStructureType const* type = RuntimeImplementation::GetDataStructureType(FullQualifiedTypeName);

			THROW_IF_EXCEPTION(Categories::Reflection, type != nullptr, " doesn't exists");

			AnyDataType data = type->CreateInstance(Arguments);

			if (data.GetValueType() == ValueTypes::VoidPointer)
				return data.GetAsVoid();

			return nullptr;
		}
	}
}