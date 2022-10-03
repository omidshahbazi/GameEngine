// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef PARAMETER_TYPE_H
#define PARAMETER_TYPE_H
#include <Reflection\Type.h>
#include <Reflection\DataType.h>

namespace Engine
{
	namespace Reflection
	{
		class ParameterType : public Type
		{
		public:
			ParameterType(void) :
				Type(nullptr)
			{
			}

			~ParameterType(void)
			{
			}

			INLINE const DataType& GetDataType(void) const
			{
				return m_DataType;
			}

		protected:
			DataType m_DataType;
		};

		typedef Vector<ParameterType> ParameterTypeList;
	}
}

#endif
