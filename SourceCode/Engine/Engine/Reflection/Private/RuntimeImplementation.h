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
				class IMetaObject
				{
				public:
					virtual void Initialize(void) = 0;
				};

			private:
				typedef Map<String, Type*> TypeMap;

			public:
				static const DataStructureType* const GetDataStructureType(const String& FullQualifiedTypeName);
				static const DataStructureType* const FindDataStructureType(const String& TypeName);
				static const EnumType* const GetEnumType(const String& TypeName);

				static void RegisterMeta(IMetaObject* Meta);
				static void InitializeMeta(void);

				static void RegisterTypeInfo(Type* Type);
				static void UnregisterTypeInfo(Type* Type);

			private:
				static IMetaObject* m_Metas[2048];
				static uint16 m_MetaIndex;
				static TypeMap m_DataStructureTypes;
				static TypeMap m_EnumTypes;
			};
		}
	}
}
#endif