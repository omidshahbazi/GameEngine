// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef ENUM_TYPE_H
#define ENUM_TYPE_H
#include <Reflection\Type.h>

namespace Engine
{
	namespace Reflection
	{
		class REFLECTION_API EnumType : public Type
		{
		public:
			struct Item
			{
			public:
				Item(void) :
					m_Value(0)
				{
				}

				Item(const String& Name, int32 Value) :
					m_Name(Name),
					m_Value(Value)
				{
				}

			public:
				const String& GetName(void) const
				{
					return m_Name;
				}

				int32 GetValue(void) const
				{
					return m_Value;
				}

			private:
				String m_Name;
				int32 m_Value;
			};

			typedef Vector<Item> ItemsList;

		protected:
			EnumType(ObjectType* TopNest);

		public:
			virtual ~EnumType(void)
			{
			}

			INLINE const ItemsList& GetItems(void) const
			{
				return m_Items;
			}

		protected:
			ItemsList m_Items;
		};
	}
}
#endif
