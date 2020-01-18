// Copyright 2012-2015 ?????????????. All Rights Reserved.
#ifndef RUNTIME_H
#define RUNTIME_H

#include <Reflection\Type.h>

namespace Engine
{
	namespace Reflection
	{
		class REFLECTION_API Runtime
		{
		public:
			template<class T> static T *CreateInstance(const String &TypeName)
			{
				return (T*)CreateInstance(TypeName);
			}
			template<class T> static T *CreateInstance(const String &TypeName, const AnyDataType &Argument)
			{
				return (T*)CreateInstance(TypeName, Argument);
			}
			template<class T> static T *CreateInstance(const String &TypeName, const ArgumentsList &Arguments)
			{
				return (T*)CreateInstance(TypeName, Arguments);
			}

			static void *CreateInstance(const String &TypeName);
			static void *CreateInstance(const String &TypeName, const AnyDataType &Argument);
			static void *CreateInstance(const String &TypeName, const ArgumentsList &Arguments);
		};
	}
}
#endif
