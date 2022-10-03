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

				INLINE void GetParentNames(AccessSpecifiers AccessFlags, StringList& List) const
				{
					if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) && m_ParentNames.Contains(AccessSpecifiers::Private))
						List.AddRange(m_ParentNames[AccessSpecifiers::Private]);
					if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected) && m_ParentNames.Contains(AccessSpecifiers::Protected))
						List.AddRange(m_ParentNames[AccessSpecifiers::Protected]);
					if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public) && m_ParentNames.Contains(AccessSpecifiers::Public))
						List.AddRange(m_ParentNames[AccessSpecifiers::Public]);
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

				INLINE String GetDeclarationMacroName(void) const
				{
					return m_Name.ToUpper() + "_OBJECT";
				}

			protected:
				void CreateInstanceInternal(const ArgumentsList* Argumetns, AnyDataType& ReturnValue) const override
				{
				}

			private:
				AllocatorBase* m_Allocator;
				uint16 m_BlockLevel;
				AccessSpecifiers m_LastAccessSpecifier;

				FunctionTypeList m_PublicConstructors;
				FunctionTypeList m_NonPublicConstructors;
			};
		}
	}
}
#endif
