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

		class REFLECTION_API DataStructureType : public Type
		{
		public:
			DataStructureType(AllocatorBase* Allocator, Type* TopNest);
			virtual ~DataStructureType(void)
			{
				for (auto type : m_PublicNestedTypes)
				{
					Destruct(type);
					DeallocateMemory(m_PublicNestedTypes.GetAllocator(), type);
				}

				for (auto type : m_NonPublicNestedTypes)
				{
					Destruct(type);
					DeallocateMemory(m_NonPublicNestedTypes.GetAllocator(), type);
				}

				for (auto type : m_PublicFunctions)
				{
					Destruct(type);
					DeallocateMemory(m_PublicFunctions.GetAllocator(), type);
				}

				for (auto type : m_NonPublicFunctions)
				{
					Destruct(type);
					DeallocateMemory(m_NonPublicFunctions.GetAllocator(), type);
				}

				for (auto type : m_PublicProperties)
				{
					Destruct(type);
					DeallocateMemory(m_PublicProperties.GetAllocator(), type);
				}

				for (auto type : m_NonPublicProperties)
				{
					Destruct(type);
					DeallocateMemory(m_NonPublicProperties.GetAllocator(), type);
				}
			}

			INLINE Types GetType(void) const override
			{
				return Types::DataStructure;
			}

			void GetParents(AccessSpecifiers AccessFlags, TypeList& List) const;

			void GetNestedTypes(AccessSpecifiers AccessFlags, TypeList& List) const;

			void GetFunctions(AccessSpecifiers AccessFlags, TypeList& List) const;

			const FunctionType* const GetFunction(const String& Name, AccessSpecifiers Access) const;

			void GetProperties(AccessSpecifiers AccessFlags, TypeList& List) const;

			AnyDataType CreateInstance(void) const;
			AnyDataType CreateInstance(const AnyDataType& Argument) const;
			AnyDataType CreateInstance(const ArgumentsList& Arguments) const;

		protected:
			virtual void CreateInstanceInternal(AnyDataType& ReturnValue, const ArgumentsList* Arguments) const = 0;

		protected:
			StringList m_PublicParentsName;
			StringList m_NonPublicParentsName;

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
