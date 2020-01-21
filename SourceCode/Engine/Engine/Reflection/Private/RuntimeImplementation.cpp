// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Private\RuntimeImplementation.h>
namespace Engine
{
	namespace Reflection
	{
		namespace Private
		{
			RuntimeImplementation::TypesMap RuntimeImplementation::m_DataStructureTypes;
			RuntimeImplementation::TypesMap RuntimeImplementation::m_EnumTypes;

			const DataStructureType *const RuntimeImplementation::GetDataStructureType(const String &TypeName)
			{
				if (m_DataStructureTypes.Contains(TypeName))
					return (DataStructureType*)m_DataStructureTypes[TypeName];

				return nullptr;
			}


			const EnumType *const RuntimeImplementation::GetEnumType(const String &TypeName)
			{
				if (m_EnumTypes.Contains(TypeName))
					return (EnumType*)m_EnumTypes[TypeName];

				return nullptr;
			}


			void RuntimeImplementation::RegisterTypeInfo(Type *Type)
			{
				if (Type->GetType() == Type::Types::DataStructure)
				{
					String scopedName = Type->GetScopedName();

					Assert(!m_DataStructureTypes.Contains(scopedName), "Type already exists");

					m_DataStructureTypes.Add(scopedName, Type);
				}
				else if (Type->GetType() == Type::Types::Enum)
				{
					const String &name = Type->GetName();

					Assert(!m_EnumTypes.Contains(name), "Type already exists");

					m_EnumTypes.Add(name, Type);
				}
			}


			void RuntimeImplementation::UnregisterTypeInfo(Type *Type)
			{
				if (Type->GetType() == Type::Types::DataStructure)
					m_DataStructureTypes.Remove(Type->GetScopedName());
				else if (Type->GetType() == Type::Types::Enum)
					m_EnumTypes.Remove(Type->GetName());
			}
		}
	}
}