// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef DATA_STRUCTURE_TYPE_H
#define DATA_STRUCTURE_TYPE_H
#include <Reflection\Type.h>
#include <Reflection\FunctionType.h>
#include <Common\BitwiseUtils.h>

namespace Engine
{
	using namespace Common;

	namespace Reflection
	{
		enum class AccessSpecifiers
		{
			None = 0,
			Private = 2,
			Protected = 4,
			Public = 8,

			Count
		};

		//TODO: Figure out all AcceessFlags
		class REFLECTION_API DataStructureType : public Type
		{
		public:
			DataStructureType(Type* TopNest) :
				Type(TopNest)
			{
			}
			virtual ~DataStructureType(void)
			{
			}

			INLINE Types GetType(void) const
			{
				return Types::DataStructure;
			}

			void GetParents(AccessSpecifiers AccessFlags, TypeList& List) const;

			INLINE void GetNestedTypes(AccessSpecifiers AccessFlags, TypeList& List) const
			{
				if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) || BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected))
					List.AddRange(m_NonPublicNestedTypes);
				if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public))
					List.AddRange(m_PublicNestedTypes);
			}

			INLINE void GetFunctions(AccessSpecifiers AccessFlags, TypeList& List) const
			{
				if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) || BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected))
					List.AddRange(m_NonPublicFunctions);
				if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public))
					List.AddRange(m_PublicFunctions);
			}

			INLINE const FunctionType* const GetFunction(const String& Name, AccessSpecifiers Access = AccessSpecifiers::Public) const
			{
				for each (auto type in (Access == AccessSpecifiers::Public ? m_PublicFunctions : m_NonPublicFunctions))
					if (type->GetName() == Name)
						return (FunctionType*)&type;

				return nullptr;
			}

			INLINE void GetProperties(AccessSpecifiers AccessFlags, TypeList& List) const
			{
				if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Private) || BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Protected))
					List.AddRange(m_NonPublicProperties);
				if (BitwiseUtils::IsEnabled(AccessFlags, AccessSpecifiers::Public))
					List.AddRange(m_PublicProperties);
			}

			AnyDataType CreateInstance(void) const;
			AnyDataType CreateInstance(const AnyDataType& Argument) const;
			AnyDataType CreateInstance(const ArgumentsList& Arguments) const;

		protected:
			virtual void CreateInstanceInternal(AnyDataType& ReturnValue, const ArgumentsList* Arguments) const = 0;

		protected:
			StringList m_ParentsName;

			TypeList m_PublicNestedTypes;
			TypeList m_NonPublicNestedTypes;

			TypeList m_PublicFunctions;
			TypeList m_NonPublicFunctions;

			TypeList m_PublicProperties;
			TypeList m_NonPublicProperties;
		};
	}
}
#endif
