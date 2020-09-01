// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Private\RuntimeImplementation.h>
namespace Engine
{
	namespace Reflection
	{
		namespace Private
		{
			uint16 RuntimeImplementation::m_MetaIndex;
			RuntimeImplementation::IMetaObject* RuntimeImplementation::m_Metas[];
			RuntimeImplementation::TypeMap RuntimeImplementation::m_DataStructureTypes;
			RuntimeImplementation::TypeMap RuntimeImplementation::m_EnumTypes;

			const DataStructureType* const RuntimeImplementation::GetDataStructureType(const String& FullQualifiedTypeName)
			{
				if (m_DataStructureTypes.Contains(FullQualifiedTypeName))
					return ReinterpretCast(DataStructureType*, m_DataStructureTypes[FullQualifiedTypeName]);

				return nullptr;
			}

			const DataStructureType* const RuntimeImplementation::FindDataStructureType(const String& TypeName)
			{
				for each (const auto & elem in m_DataStructureTypes)
				{
					if (!elem.GetFirst().EndsWith(TypeName))
						continue;

					return ReinterpretCast(DataStructureType*, elem.GetSecond());
				}

				return nullptr;
			}

			const EnumType* const RuntimeImplementation::GetEnumType(const String& TypeName)
			{
				if (m_EnumTypes.Contains(TypeName))
					return (EnumType*)m_EnumTypes[TypeName];

				return nullptr;
			}

			void RuntimeImplementation::RegisterMeta(IMetaObject* Meta)
			{
				m_Metas[m_MetaIndex++] = Meta;
			}

			void RuntimeImplementation::InitializeMeta(void)
			{
				static bool initialized = false;
				if (initialized)
					return;
				initialized = true;

				for (uint16 i = 0; i < m_MetaIndex; ++i)
					m_Metas[i]->Initialize();
			}

			void RuntimeImplementation::RegisterTypeInfo(Type* Type)
			{
				if (Type->GetType() == Type::Types::DataStructure)
				{
					String scopedName = Type->GetFullQualifiedName();

					Assert(!m_DataStructureTypes.Contains(scopedName), "Type already exists");

					m_DataStructureTypes.Add(scopedName, Type);
				}
				else if (Type->GetType() == Type::Types::Enum)
				{
					const String& name = Type->GetName();

					Assert(!m_EnumTypes.Contains(name), "Type already exists");

					m_EnumTypes.Add(name, Type);
				}
			}

			void RuntimeImplementation::UnregisterTypeInfo(Type* Type)
			{
				if (Type->GetType() == Type::Types::DataStructure)
					m_DataStructureTypes.Remove(Type->GetFullQualifiedName());
				else if (Type->GetType() == Type::Types::Enum)
					m_EnumTypes.Remove(Type->GetName());
			}
		}
	}
}