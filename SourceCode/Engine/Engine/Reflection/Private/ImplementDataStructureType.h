// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef IMPLEMENT_DATA_STRUCTURE_TYPE_H
#define IMPLEMENT_DATA_STRUCTURE_TYPE_H
#include <Reflection\DataStructureType.h>

namespace Engine
{
	namespace Reflection
	{
		namespace Private
		{
			class REFLECTION_API ImplementDataStructureType : public DataStructureType
			{
			public:
				ImplementDataStructureType(DataStructureType *TopNest);

				virtual ~ImplementDataStructureType(void)
				{
				}

				INLINE void AddParentName(const String& Value, AccessSpecifiers Access)
				{
					m_ParentsName.Add(Value);//, Access
				}

				INLINE void AddNestedType(Type* Value, AccessSpecifiers Access)
				{
					if (Access == AccessSpecifiers::None || Access == AccessSpecifiers::Private)
						m_NonPublicNestedTypes.Add(Value);
					else
						m_PublicNestedTypes.Add(Value);
				}

				INLINE void AddFunction(Type *Value, AccessSpecifiers Access)
				{
					if (Access == AccessSpecifiers::None || Access == AccessSpecifiers::Private)
						m_NonPublicFunctions.Add(Value);
					else
						m_PublicFunctions.Add(Value);
				}

				INLINE void AddProperty(Type *Value, AccessSpecifiers Access)
				{
					if (Access == AccessSpecifiers::None || Access == AccessSpecifiers::Private)
						m_NonPublicProperties.Add(Value);
					else
						m_PublicProperties.Add(Value);
				}

				INLINE void SetName(const String &Value)
				{
					m_Name = Value;
				}
			};
		}
	}
}
#endif
