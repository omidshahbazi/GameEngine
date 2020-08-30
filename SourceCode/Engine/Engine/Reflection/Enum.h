// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef ENUM_H
#define ENUM_H

#include <Reflection\EnumType.h>

namespace Engine
{
	namespace Reflection
	{
		class REFLECTION_API Enum
		{
		public:
			static const EnumType *const GetType(const String &TypeName);

			static bool IsDefined(const String &TypeName, const String &Value);
			static bool IsDefined(const EnumType const *Type, const String &Value);

			template<class T> static T Parse(const String &TypeName, const String &Value)
			{
				return (T)Parse(TypeName, Value);
			}
			template<class T> static T Parse(const EnumType const *Type, const String &Value)
			{
				return (T)Parse(Type, Value);
			}
			static int32 Parse(const String &TypeName, const String &Value);
			static int32 Parse(const EnumType const *Type, const String &Value);

			template<class T> static String ToString(const String &TypeName, T Value)
			{
				return ToString(TypeName, (T)Value);
			}
			template<class T> static String ToString(const EnumType const *Type, T Value)
			{
				return ToString(Type, (T)Value);
			}
			static String ToString(const String &TypeName, int32 Value);
			static String ToString(const EnumType const *Type, int32 Value);

			//TODO: Impl. these functions
			//static String GetName(Type enumType, object value);
			//static StringsList GetNames(Type enumType);
			//static Array GetValues(Type enumType);
		};
	}
}
#endif
