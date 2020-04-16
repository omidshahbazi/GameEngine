// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef TYPE_H
#define TYPE_H
#include <Containers\AnyDataType.h>
#include <Containers\List.h>

namespace Engine
{
	using namespace Containers;

	namespace Reflection
	{
		class REFLECTION_API Type
		{
		public:
			enum class Types
			{
				DataStructure = 0,
				Enum,
				Property,
				Constructor,
				Function
			};

		public:
			Type(const String &Name);
			Type(Type *TopNest);
			Type(const String &Name, Type *TopNest);
			virtual ~Type(void)
			{
			}

			INLINE uint32 GetTypeID(void) const
			{
				return m_TypeID;
			}

			virtual Types GetType(void) const = 0;

			INLINE const String &GetName(void) const
			{
				return m_Name;
			}

			INLINE String GetFullQualifiedName(void) const
			{
				return (m_TopNest == nullptr ? "" : m_TopNest->GetFullQualifiedName() + "::") + m_Name;
			}

			INLINE Type *GetTopNest(void)
			{
				return m_TopNest;
			}

		protected:
			uint32 m_TypeID;
			String m_Name;
			Type *m_TopNest;
		};

		typedef Vector<Type*> TypeList;
		typedef List<AnyDataType> ArgumentsList;
	}
}
#endif