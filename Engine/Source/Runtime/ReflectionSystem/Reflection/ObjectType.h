// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef OBJECT_TYPE_H
#define OBJECT_TYPE_H
#include <Reflection\Type.h>
#include <Reflection\Definitions.h>
#include <Reflection\FunctionType.h>
#include <Reflection\PropertyType.h>
#include <Common\BitwiseUtils.h>
#include <Containers\Map.h>

namespace Engine
{
	using namespace Common;

	namespace Reflection
	{
		class Runtime;
		class ObjectType;

		typedef Vector<ObjectType*> ObjectTypeList;

		class REFLECTION_API ObjectType : public Type
		{
			friend class Runtime;

		private:
			typedef Map<AccessSpecifiers, StringList> ParentsMap;
			typedef Map<AccessSpecifiers, TypeList> NestedTypesMap;
			typedef Map<AccessSpecifiers, FunctionTypeList> FunctionTypesMap;
			typedef Map<AccessSpecifiers, PropertyTypeList> PropertyTypesMap;

		protected:
			ObjectType(AllocatorBase* Allocator, ObjectType* TopNest);

		public:
			virtual ~ObjectType(void);

			virtual String GetFullQualifiedName(void) const override;

			INLINE const String& GetNamespace(void) const
			{
				return m_Namespace;
			}

			INLINE bool GetIsStruct(void) const
			{
				return m_IsStruct;
			}

			INLINE bool GetIsAbstract(void) const
			{
				return m_IsAbstract;
			}

			INLINE bool GetIsStatic(void) const
			{
				return m_IsStatic;
			}

			void GetParents(AccessSpecifiers AccessFlags, ObjectTypeList& List) const;

			void GetNestedTypes(AccessSpecifiers AccessFlags, TypeList& List) const;

			void GetFunctions(AccessSpecifiers AccessFlags, FunctionTypeList& List) const;

			void GetProperties(AccessSpecifiers AccessFlags, PropertyTypeList& List) const;

		protected:
			virtual void CreateInstanceInternal(const ArgumentsList* Arguments, AnyDataType& ReturnValue) const = 0;

		protected:
			String m_Namespace;

			bool m_IsStruct;
			bool m_IsAbstract;
			bool m_IsStatic;

			ParentsMap m_ParentNames;
			NestedTypesMap m_NestedTypes;
			FunctionTypesMap m_Functions;
			PropertyTypesMap m_Properties;
		};
	}
}
#endif
