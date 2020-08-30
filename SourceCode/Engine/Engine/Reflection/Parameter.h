// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef PARAMETER_H
#define PARAMETER_H
#include <Reflection\DataType.h>
#include <Containers\AnyDataType.h>

namespace Engine
{
	namespace Reflection
	{
		class Parameter
		{
		public:
			Parameter(const DataType &DataType, const String &Name) :
				m_DataType(DataType),
				m_Name(Name)
			{
			}
			~Parameter(void)
			{
			}

			INLINE const DataType &GetDataType(void) const
			{
				return m_DataType;
			}

			INLINE void GetSignature(String &Signature) const
			{
				m_DataType.GetSignature(Signature);
			}

		private:
			DataType m_DataType;
			String m_Name;
		};

		typedef Vector<Parameter> ParameterList;
	}
}

#endif
