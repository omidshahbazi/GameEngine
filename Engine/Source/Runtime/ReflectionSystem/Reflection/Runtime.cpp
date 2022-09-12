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
			auto type = RuntimeImplementation::GetObjectType(FullQualifiedTypeName);
			THROW_IF_EXCEPTION(Categories::Reflection, type == nullptr, "Couldn't find " + FullQualifiedTypeName);

			return CreateInstance(type);
		}

		void* Runtime::CreateInstance(const String& FullQualifiedTypeName, const AnyDataType& Argument)
		{
			auto type = RuntimeImplementation::GetObjectType(FullQualifiedTypeName);
			THROW_IF_EXCEPTION(Categories::Reflection, type == nullptr, "Couldn't find " + FullQualifiedTypeName);

			return CreateInstance(type, Argument);
		}


		void* Runtime::CreateInstance(const String& FullQualifiedTypeName, const ArgumentsList& Arguments)
		{
			auto type = RuntimeImplementation::GetObjectType(FullQualifiedTypeName);
			THROW_IF_EXCEPTION(Categories::Reflection, type == nullptr, "Couldn't find " + FullQualifiedTypeName);

			return CreateInstance(type, Arguments);
		}

		void* Runtime::CreateInstance(const ObjectType* Type)
		{
			THROW_IF_EXCEPTION(Categories::Reflection, Type == nullptr, "Type is null");

			AnyDataType data = Type->CreateInstance();

			if (data.GetValueType() == ValueTypes::VoidPointer)
				return data.GetAsVoid();

			return nullptr;
		}

		void* Runtime::CreateInstance(const ObjectType* Type, const AnyDataType& Argument)
		{
			THROW_IF_EXCEPTION(Categories::Reflection, Type == nullptr, "Type is null");

			AnyDataType data = Type->CreateInstance(Argument);

			if (data.GetValueType() == ValueTypes::VoidPointer)
				return data.GetAsVoid();

			return nullptr;
		}

		void* Runtime::CreateInstance(const ObjectType* Type, const ArgumentsList& Arguments)
		{
			THROW_IF_EXCEPTION(Categories::Reflection, Type == nullptr, "Type is null");

			AnyDataType data = Type->CreateInstance(Arguments);

			if (data.GetValueType() == ValueTypes::VoidPointer)
				return data.GetAsVoid();

			return nullptr;
		}
	}
}