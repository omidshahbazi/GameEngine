// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef RUNTIME_IMPLEMENTATION_H
#define RUNTIME_IMPLEMENTATION_H

#include <Containers\Map.h>
#include <Reflection\ObjectType.h>
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
				friend class Destructor;

			public:
				class IMetaType
				{
				public:
					virtual ~IMetaType(void)
					{

					}

					virtual void Initialize(void) = 0;
				};

			private:
				typedef Map<String, Type*> TypeMap;

			public:
				static const ObjectType* GetObjectType(const String& FullQualifiedTypeName);
				static const ObjectType* FindObjectType(const String& TypeName);
				static const EnumType* GetEnumType(const String& TypeName);

				static void RegisterMeta(IMetaType* Meta);
				static void InitializeMeta(void);

				static void RegisterType(Type* Type);
				static void UnregisterType(Type* Type);

			private:
				static SpinLock m_Lock;
				static IMetaType* m_MetaTypes[2048];
				static uint16 m_MetaObjectCount;
				static TypeMap* m_ObjectTypes;
				static TypeMap* m_EnumTypes;
			};
		}
	}
}
#endif