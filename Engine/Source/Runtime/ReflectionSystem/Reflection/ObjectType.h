// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef OBJECT_TYPE_H
#define OBJECT_TYPE_H
#include <Reflection\Type.h>
#include <Reflection\Definitions.h>
#include <Reflection\FunctionType.h>
#include <Reflection\PropertyType.h>
#include <Common\BitwiseUtils.h>

namespace Engine
{
	using namespace Common;

	namespace Reflection
	{
		class ObjectType;

		typedef Vector<ObjectType*> ObjectTypeList;

		class REFLECTION_API ObjectType : public Type
		{
		protected:
			ObjectType(AllocatorBase* Allocator, ObjectType* TopNest);

		public:
			virtual ~ObjectType(void);

			INLINE bool GetIsStruct(void) const
			{
				return m_IsStruct;
			}

			void GetParents(AccessSpecifiers AccessFlags, ObjectTypeList& List) const;

			void GetNestedTypes(AccessSpecifiers AccessFlags, TypeList& List) const;

			void GetFunctions(AccessSpecifiers AccessFlags, FunctionTypeList& List) const;

			const FunctionType* const GetFunction(const String& Name, AccessSpecifiers Access) const;

			void GetProperties(AccessSpecifiers AccessFlags, PropertyTypeList& List) const;

			AnyDataType CreateInstance(void) const;
			AnyDataType CreateInstance(const AnyDataType& Argument) const;
			AnyDataType CreateInstance(const ArgumentsList& Arguments) const;

		protected:
			virtual void CreateInstanceInternal(AnyDataType& ReturnValue, const ArgumentsList* Arguments) const = 0;

		protected:
			bool m_IsStruct;

			StringList m_PublicParentsName;
			StringList m_NonPublicParentsName;

			TypeList m_PublicNestedTypes;
			TypeList m_NonPublicNestedTypes;

			FunctionTypeList m_PublicFunctions;
			FunctionTypeList m_NonPublicFunctions;

			PropertyTypeList m_PublicProperties;
			PropertyTypeList m_NonPublicProperties;
		};
	}
}
#endif
