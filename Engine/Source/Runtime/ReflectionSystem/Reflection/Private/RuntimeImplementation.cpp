// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Reflection\Private\RuntimeImplementation.h>
#include <Allocators\RootAllocator.h>
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
					if (RuntimeImplementation::m_ObjectTypes != nullptr)
					{
						Destruct(RuntimeImplementation::m_ObjectTypes);
						DeallocateMemory(RootAllocator::GetInstance(), RuntimeImplementation::m_ObjectTypes);
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
			uint16 RuntimeImplementation::m_MetaObjectCount;
			RuntimeImplementation::IMetaType* RuntimeImplementation::m_MetaTypes[];
			RuntimeImplementation::TypeMap* RuntimeImplementation::m_ObjectTypes = nullptr;
			RuntimeImplementation::TypeMap* RuntimeImplementation::m_EnumTypes = nullptr;

#define INITIALIZE_MAP(Map) \
			if (Map == nullptr) \
			{ \
				Map = ReinterpretCast(RuntimeImplementation::TypeMap*, AllocateMemory(RootAllocator::GetInstance(), sizeof(RuntimeImplementation::TypeMap))); \
				Construct(Map); \
			}

			const ObjectType* RuntimeImplementation::GetObjectType(const String& FullQualifiedTypeName)
			{
				INITIALIZE_MAP(m_ObjectTypes);

				if (m_ObjectTypes->Contains(FullQualifiedTypeName))
					return ReinterpretCast(ObjectType*, (*m_ObjectTypes)[FullQualifiedTypeName]);

				return nullptr;
			}

			const ObjectType* RuntimeImplementation::FindObjectType(const String& TypeName)
			{
				INITIALIZE_MAP(m_ObjectTypes);

				for (const auto& elem : (*m_ObjectTypes))
				{
					if (!elem.GetFirst().EndsWith(TypeName))
						continue;

					return ReinterpretCast(ObjectType*, elem.GetSecond());
				}

				return nullptr;
			}

			const EnumType* RuntimeImplementation::GetEnumType(const String& TypeName)
			{
				INITIALIZE_MAP(m_EnumTypes);

				if (m_EnumTypes->Contains(TypeName))
					return (EnumType*)(*m_EnumTypes)[TypeName];

				return nullptr;
			}

			void RuntimeImplementation::RegisterMeta(IMetaType* Meta)
			{
				m_MetaTypes[m_MetaObjectCount++] = Meta;
			}

			void RuntimeImplementation::InitializeMeta(void)
			{
				m_Lock.Lock();

				static bool initialized = false;
				if (!initialized)
				{
					initialized = true;

					for (uint16 i = 0; i < m_MetaObjectCount; ++i)
						m_MetaTypes[i]->Initialize();
				}

				m_Lock.Release();
			}

			void RuntimeImplementation::RegisterType(Type* Type)
			{
				INITIALIZE_MAP(m_ObjectTypes);
				INITIALIZE_MAP(m_EnumTypes);

				if (IsTypeOf(Type, ObjectType))
				{
					String scopedName = Type->GetFullQualifiedName();

					THROW_IF_EXCEPTION(Categories::Reflection, m_ObjectTypes->Contains(scopedName), "Type already exists");

					m_ObjectTypes->Add(scopedName, Type);
				}
				else if (IsTypeOf(Type, EnumType))
				{
					const String& name = Type->GetFullQualifiedName();

					THROW_IF_EXCEPTION(Categories::Reflection, m_EnumTypes->Contains(name), "Type already exists");

					m_EnumTypes->Add(name, Type);
				}
				else
					CoreDebugAssert(Categories::Reflection, false, "Unhandled type");
			}

			void RuntimeImplementation::UnregisterType(Type* Type)
			{
				INITIALIZE_MAP(m_ObjectTypes);
				INITIALIZE_MAP(m_EnumTypes);

				if (IsTypeOf(Type, ObjectType))
					m_ObjectTypes->Remove(Type->GetFullQualifiedName());
				else if (IsTypeOf(Type, EnumType))
					m_EnumTypes->Remove(Type->GetFullQualifiedName());
				else
					CoreDebugAssert(Categories::Reflection, false, "Unhandled type");
			}

#undef INITIALIZE_MAP
		}
	}
}