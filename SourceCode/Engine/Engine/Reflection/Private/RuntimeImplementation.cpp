// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\Private\RuntimeImplementation.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace Debugging;

	namespace Reflection
	{
		namespace Private
		{
			class Destructor
			{
			public:
				~Destructor(void)
				{
					if (RuntimeImplementation::m_DataStructureTypes != nullptr)
					{
						Destruct(RuntimeImplementation::m_DataStructureTypes);
						DeallocateMemory(RootAllocator::GetInstance(), RuntimeImplementation::m_DataStructureTypes);
					}

					if (RuntimeImplementation::m_EnumTypes != nullptr)
					{
						Destruct(RuntimeImplementation::m_EnumTypes);
						DeallocateMemory(RootAllocator::GetInstance(), RuntimeImplementation::m_EnumTypes);
					}
				}
			};

			Destructor destructor;

			SpinLock RuntimeImplementation::m_Lock;
			uint16 RuntimeImplementation::m_MetaIndex;
			RuntimeImplementation::IMetaObject* RuntimeImplementation::m_Metas[];
			RuntimeImplementation::TypeMap* RuntimeImplementation::m_DataStructureTypes = nullptr;
			RuntimeImplementation::TypeMap* RuntimeImplementation::m_EnumTypes = nullptr;

#define MAP_INITIALIZER(Map) \
			if (Map == nullptr) \
			{ \
				Map = ReinterpretCast(RuntimeImplementation::TypeMap*, AllocateMemory(RootAllocator::GetInstance(), sizeof(RuntimeImplementation::TypeMap))); \
				Construct(Map); \
			}

			const DataStructureType* const RuntimeImplementation::GetDataStructureType(const String& FullQualifiedTypeName)
			{
				MAP_INITIALIZER(m_DataStructureTypes);

				if (m_DataStructureTypes->Contains(FullQualifiedTypeName))
					return ReinterpretCast(DataStructureType*, (*m_DataStructureTypes)[FullQualifiedTypeName]);

				return nullptr;
			}

			const DataStructureType* const RuntimeImplementation::FindDataStructureType(const String& TypeName)
			{
				MAP_INITIALIZER(m_DataStructureTypes);

				for (const auto& elem : (*m_DataStructureTypes))
				{
					if (!elem.GetFirst().EndsWith(TypeName))
						continue;

					return ReinterpretCast(DataStructureType*, elem.GetSecond());
				}

				return nullptr;
			}

			const EnumType* const RuntimeImplementation::GetEnumType(const String& TypeName)
			{
				MAP_INITIALIZER(m_EnumTypes);

				if (m_EnumTypes->Contains(TypeName))
					return (EnumType*)(*m_EnumTypes)[TypeName];

				return nullptr;
			}

			void RuntimeImplementation::RegisterMeta(IMetaObject* Meta)
			{
				m_Metas[m_MetaIndex++] = Meta;
			}

			void RuntimeImplementation::InitializeMeta(void)
			{
				m_Lock.Lock();

				static bool initialized = false;
				if (!initialized)
				{
					initialized = true;

					for (uint16 i = 0; i < m_MetaIndex; ++i)
						m_Metas[i]->Initialize();
				}

				m_Lock.Release();
			}

			void RuntimeImplementation::RegisterTypeInfo(Type* Type)
			{
				MAP_INITIALIZER(m_DataStructureTypes);
				MAP_INITIALIZER(m_EnumTypes);

				if (Type->GetType() == Type::Types::DataStructure)
				{
					String scopedName = Type->GetFullQualifiedName();

					THROW_IF_EXCEPTION(Categories::Reflection, !m_DataStructureTypes->Contains(scopedName), "Type already exists");

					m_DataStructureTypes->Add(scopedName, Type);
				}
				else if (Type->GetType() == Type::Types::Enum)
				{
					const String& name = Type->GetName();

					THROW_IF_EXCEPTION(Categories::Reflection, !m_EnumTypes->Contains(name), "Type already exists");

					m_EnumTypes->Add(name, Type);
				}
			}

			void RuntimeImplementation::UnregisterTypeInfo(Type* Type)
			{
				MAP_INITIALIZER(m_DataStructureTypes);
				MAP_INITIALIZER(m_EnumTypes);

				if (Type->GetType() == Type::Types::DataStructure)
					m_DataStructureTypes->Remove(Type->GetFullQualifiedName());
				else if (Type->GetType() == Type::Types::Enum)
					m_EnumTypes->Remove(Type->GetName());
			}
		}
	}
}