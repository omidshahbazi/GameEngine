// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef META_TYPE_H
#define META_TYPE_H
#include <Reflection\Private\ImplementDataStructureType.h>
#include <ReflectionTool\Specifiers.h>
#include <ReflectionTool\ReflectionToolAllocators.h>
#include <Utility\Lexer\Token.h>

namespace Engine
{
	using namespace Reflection::Private;
	using namespace Utility::Lexer;
	using namespace Reflection;

	namespace ReflectionTool
	{
		class REFLECTIONTOOL_API MetaDataStructure : public ImplementDataStructureType, public Specifiers
		{
		public:
			MetaDataStructure(DataStructureType* TopNest) :
				ImplementDataStructureType(&ReflectionToolAllocators::TypesAllocator, TopNest),
				m_BlockLevel(0),
				m_LastAccessSpecifier(AccessSpecifiers::None)
			{
			}
			virtual ~MetaDataStructure(void)
			{
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
				m_NameSpace = Namespace;
			}

			INLINE void AddParentName(const String& Value)
			{
				ImplementDataStructureType::AddParentName(Value, m_LastAccessSpecifier);
			}
			INLINE void GetParentsName(AccessSpecifiers AccessFlags, StringList& List) const
			{
				if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) || BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected))
				{
					//TODO: Figure out all AcceessFlags
				}
				if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public))
					List.AddRange(m_ParentsName);
			}

			INLINE void AddNestedType(Type* Value)
			{
				ImplementDataStructureType::AddNestedType(Value, m_LastAccessSpecifier);
			}

			INLINE void AddConstructor(Type* Value)
			{
				if (m_LastAccessSpecifier == AccessSpecifiers::None || m_LastAccessSpecifier == AccessSpecifiers::Private)
					m_NonPublicConstructors.Add(Value);
				else
					m_PublicConstructors.Add(Value);
			}

			INLINE void AddFunction(Type* Value)
			{
				ImplementDataStructureType::AddFunction(Value, m_LastAccessSpecifier);
			}

			INLINE void AddProperty(Type* Value)
			{
				ImplementDataStructureType::AddProperty(Value, m_LastAccessSpecifier);
			}

			INLINE const String& GetNamespace(void) const
			{
				return m_NameSpace;
			}

			INLINE void GetConstructors(AccessSpecifiers AccessFlags, TypeList& List) const
			{
				if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) || BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected))
					List.AddRange(m_NonPublicConstructors);
				if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public))
					List.AddRange(m_PublicConstructors);
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
			uint16 m_BlockLevel;
			AccessSpecifiers m_LastAccessSpecifier;
			TypeList m_PublicConstructors;
			TypeList m_NonPublicConstructors;
			String m_NameSpace;
		};
	}
}
#endif
