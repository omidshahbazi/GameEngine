// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef RUNTIME_H
#define RUNTIME_H

#include <Reflection\Type.h>
#include <Reflection\ObjectType.h>

namespace Engine
{
	namespace Reflection
	{
		class REFLECTION_API Runtime
		{
		public:
			template<class T> static T* CreateInstance(const String& FullQualifiedTypeName)
			{
				return (T*)CreateInstance(FullQualifiedTypeName);
			}
			template<class T> static T* CreateInstance(const String& FullQualifiedTypeName, const AnyDataType& Argument)
			{
				return (T*)CreateInstance(FullQualifiedTypeName, Argument);
			}
			template<class T> static T* CreateInstance(const String& FullQualifiedTypeName, const ArgumentsList& Arguments)
			{
				return (T*)CreateInstance(FullQualifiedTypeName, Arguments);
			}

			static void* CreateInstance(const String& FullQualifiedTypeName);
			static void* CreateInstance(const String& FullQualifiedTypeName, const AnyDataType& Argument);
			static void* CreateInstance(const String& FullQualifiedTypeName, const ArgumentsList& Arguments);

			static void* CreateInstance(const ObjectType* Type);
			static void* CreateInstance(const ObjectType* Type, const AnyDataType& Argument);
			static void* CreateInstance(const ObjectType* Type, const ArgumentsList& Arguments);
		};
	}
}
#endif
