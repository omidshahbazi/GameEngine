// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef META_OBJECT_H
#define META_OBJECT_H
#include <Reflection\Private\ImplementObjectType.h>
#include <ReflectionGenerator\Private\Specifiers.h>
#include <Allocators\AllocatorBase.h>

namespace Engine
{
	using namespace Reflection;
	using namespace Reflection::Private;

	namespace ReflectionGenerator
	{
		namespace Private
		{
			class REFLECTIONGENERATOR_API MetaObject : public ImplementObjectType, public Specifiers
			{
			public:
				MetaObject(AllocatorBase* Allocator, MetaObject* TopNest) :
					m_Allocator(Allocator),
					ImplementObjectType(Allocator, TopNest),
					m_BlockLevel(0),
					m_LastAccessSpecifier(AccessSpecifiers::None)
				{
				}

				virtual ~MetaObject(void)
				{
					for (auto type : m_PublicConstructors)
					{
						Destruct(type);
						DeallocateMemory(m_Allocator, type);
					}

					for (auto type : m_NonPublicConstructors)
					{
						Destruct(type);
						DeallocateMemory(m_Allocator, type);
					}
				}

				INLINE uint16 GetBlockLevel(void) const
				{
					return m_BlockLevel;
				}
				INLINE void SetBlockLevel(uint16 Value)
				{
					m_BlockLevel = Value;
				}

				INLINE void SetLastAccessSpecifier(AccessSpecifiers Value)
				{
					m_LastAccessSpecifier = Value;
				}

				INLINE void SetNamespace(const String& Namespace)
				{
					m_Namespace = Namespace;
				}

				INLINE const String& GetNamespace(void) const
				{
					return m_Namespace;
				}

				INLINE void AddParentName(const String& Value, AccessSpecifiers AccessSpecifier)
				{
					ImplementObjectType::AddParentName(Value, AccessSpecifier);
				}
				INLINE void GetParentsName(AccessSpecifiers AccessFlags, StringList& List) const
				{
					if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) || BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected))
						List.AddRange(m_NonPublicParentsName);
					if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public))
						List.AddRange(m_PublicParentsName);
				}

				INLINE void AddNestedType(Type* Value)
				{
					ImplementObjectType::AddNestedType(Value, m_LastAccessSpecifier);
				}

				INLINE void AddConstructor(FunctionType* Value)
				{
					if (m_LastAccessSpecifier == AccessSpecifiers::Public)
						m_PublicConstructors.Add(Value);
					else
						m_NonPublicConstructors.Add(Value);
				}

				void GetConstructors(AccessSpecifiers AccessFlags, FunctionTypeList& List) const
				{
					if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) || BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected))
						List.AddRange(m_NonPublicConstructors);
					if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public))
						List.AddRange(m_PublicConstructors);
				}

				INLINE void AddFunction(FunctionType* Value)
				{
					ImplementObjectType::AddFunction(Value, m_LastAccessSpecifier);
				}

				INLINE void AddProperty(PropertyType* Value)
				{
					ImplementObjectType::AddProperty(Value, m_LastAccessSpecifier);
				}

				INLINE String GetUniqueName(void) const
				{
					return (m_TopNest == nullptr ? "" : m_TopNest->GetName() + "_") + m_Name;
				}

				INLINE String GetDeclarationMacroName(void) const
				{
					return m_Name.ToUpper() + "_OBJECT";
				}

			protected:
				void CreateInstanceInternal(AnyDataType& ReturnValue, const ArgumentsList* Argumetns) const override
				{
				}

			private:
				AllocatorBase* m_Allocator;
				uint16 m_BlockLevel;
				AccessSpecifiers m_LastAccessSpecifier;
				String m_Namespace;

				FunctionTypeList m_PublicConstructors;
				FunctionTypeList m_NonPublicConstructors;
			};
		}
	}
}
#endif
