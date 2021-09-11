// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef SYNTAX_ITEM_HOLDER_H
#define SYNTAX_ITEM_HOLDER_H

#include <Containers\Vector.h>

namespace Engine
{
	using namespace Containers;

	namespace ProgramParser
	{
		namespace AbstractSyntaxTree
		{
			template<typename T>
			class SyntaxItemHolder
			{
			public:
				SyntaxItemHolder(AllocatorBase* Allocator) :
					m_Items(Allocator)
				{
				}

				virtual ~SyntaxItemHolder(void)
				{
					for (auto item : m_Items)
						Destruct(item);
				}

				const Vector<T*>& GetItems(void) const
				{
					return m_Items;
				}

				void AddItem(T* Item)
				{
					m_Items.Add(Item);
				}

			private:
				Vector<T*> m_Items;
			};
		}
	}
}

#endif