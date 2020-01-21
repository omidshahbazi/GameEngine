// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef META_TYPE_H
#define META_TYPE_H
#include <Reflection\Private\ImplementDataStructureType.h>
#include <ReflectionTool\Specifiers.h>
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
			MetaDataStructure(DataStructureType *TopNest) :
				ImplementDataStructureType(TopNest),
				m_BlockLevel(0),
				m_LastAccessSpecifier(AccessSpecifiers::None)
			{
			}
			~MetaDataStructure(void)
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

			INLINE void SetNamespace(const String &Namespace)
			{
				m_NameSpace = Namespace;
			}

			INLINE void AddSubType(Type *Value)
			{
				ImplementDataStructureType::AddSubType(Value, m_LastAccessSpecifier);
			}

			INLINE void AddConstructor(Type *Value)
			{
				if (m_LastAccessSpecifier == AccessSpecifiers::None || m_LastAccessSpecifier == AccessSpecifiers::Private)
					m_NonPublicConstructors.Add(Value);
				else
					m_PublicConstructors.Add(Value);
			}

			INLINE void AddFunction(Type *Value)
			{
				ImplementDataStructureType::AddFunction(Value, m_LastAccessSpecifier);
			}

			INLINE void AddProperty(Type *Value)
			{
				ImplementDataStructureType::AddProperty(Value, m_LastAccessSpecifier);
			}

			INLINE void AddParentName(const String &Value)
			{
				m_ParentsName.Add(Value);
			}
			INLINE const StringList &GetParentsName(void) const
			{
				return m_ParentsName;
			}

			INLINE const String &GetNamespace(void) const
			{
				return m_NameSpace;
			}

			INLINE const TypesList &GetPublicConstructors(void) const
			{
				return m_PublicConstructors;
			}

			INLINE const TypesList &GetNonPublicConstructors(void) const
			{
				return m_NonPublicConstructors;
			}

			INLINE String GetUniqueName(void) const
			{
				return (m_TopNest == nullptr ? "" : m_TopNest->GetName() + "_") + m_Name;
			}

			INLINE String GetDeclarationMacroName(void) const
			{
				return m_Name.ToUpper() + "_OBJECT";
			}

			INLINE static AccessSpecifiers ParseAccessSpecifier(Token &Token)
			{
				static const String AccessSpecifiersName[] = { "", "private", "protected", "public" };

				for (AccessSpecifiers res = AccessSpecifiers((int)AccessSpecifiers::None + 1); res != AccessSpecifiers::Count; res = AccessSpecifiers((int32)res + 1))
					if (Token.Matches(AccessSpecifiersName[(int)res], Token::SearchCases::CaseSensitive))
						return res;

				return AccessSpecifiers::None;
			}

		protected:
			void CreateInstanceInternal(AnyDataType &ReturnValue, const ArgumentsList *Argumetns) const
			{
			}

		private:
			uint16 m_BlockLevel;
			AccessSpecifiers m_LastAccessSpecifier;
			StringList m_ParentsName;
			TypesList m_PublicConstructors;
			TypesList m_NonPublicConstructors;
			String m_NameSpace;
		};
	}
}
#endif
