// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef TYPE_H
#define TYPE_H
#include <Containers\AnyDataType.h>
#include <Containers\Vector.h>

namespace Engine
{
	using namespace Containers;

	namespace Reflection
	{
		class ObjectType;

		class REFLECTION_API Type
		{
		public:
			Type(ObjectType* TopNest);
			virtual ~Type(void)
			{
			}

			INLINE uint32 GetTypeID(void) const
			{
				return m_TypeID;
			}

			INLINE const String& GetName(void) const
			{
				return m_Name;
			}

			INLINE ObjectType* GetTopNest(void) const
			{
				return m_TopNest;
			}

			virtual String GetFullQualifiedName(void) const;

			bool operator==(const Type& Other) const
			{
				return (m_TypeID == Other.m_TypeID);
			}

			bool operator!=(const Type& Other) const
			{
				return (m_TypeID != Other.m_TypeID);
			}

		protected:
			uint32 m_TypeID;
			String m_Name;
			ObjectType* m_TopNest;
		};

		typedef Vector<Type*> TypeList;
	}
}
#endif