// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef RUNTIME_IMPLEMENTATION_H
#define RUNTIME_IMPLEMENTATION_H

#include <Containers\Map.h>
#include <Reflection\DataStructureType.h>
#include <Reflection\EnumType.h>

namespace Engine
{
	using namespace Containers;

	namespace Reflection
	{
		namespace Private
		{
			class REFLECTION_API RuntimeImplementation
			{
			public:
				static const DataStructureType* const GetDataStructureType(const String& FullQualifiedTypeName);
				static const DataStructureType *const FindDataStructureType(const String & TypeName);
				static const EnumType *const GetEnumType(const String &TypeName);

				static void RegisterTypeInfo(Type *Type);
				static void UnregisterTypeInfo(Type *Type);

			private:
				typedef Map<String, Type*> TypesMap;

			private:
				static TypesMap m_DataStructureTypes;
				static TypesMap m_EnumTypes;
			};
		}
	}
}
#endif