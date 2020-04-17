// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Private\ImplementDataStructureType.h>

namespace Engine
{
	namespace Reflection
	{
		namespace Private
		{
			ImplementDataStructureType::ImplementDataStructureType(DataStructureType* TopNest) :
				DataStructureType(TopNest)
			{
			}

			void ImplementDataStructureType::AddParentName(const String& Value, AccessSpecifiers Access)
			{
				m_ParentsName.Add(Value);//TODO:, Access
			}

			void ImplementDataStructureType::AddNestedType(Type* Value, AccessSpecifiers Access)
			{
				if (Access == AccessSpecifiers::None || Access == AccessSpecifiers::Private)
					m_NonPublicNestedTypes.Add(Value);
				else
					m_PublicNestedTypes.Add(Value);
			}

			void ImplementDataStructureType::AddFunction(Type* Value, AccessSpecifiers Access)
			{
				if (Access == AccessSpecifiers::None || Access == AccessSpecifiers::Private)
					m_NonPublicFunctions.Add(Value);
				else
					m_PublicFunctions.Add(Value);
			}

			void ImplementDataStructureType::AddProperty(Type* Value, AccessSpecifiers Access)
			{
				if (Access == AccessSpecifiers::None || Access == AccessSpecifiers::Private)
					m_NonPublicProperties.Add(Value);
				else
					m_PublicProperties.Add(Value);
			}
		}
	}
}