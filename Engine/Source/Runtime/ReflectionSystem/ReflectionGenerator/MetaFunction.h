// Copyright 2016-2020 ?????????????. All Rights Reserved.
#ifndef META_FUNCTION_H
#define META_FUNCTION_H
#include <Reflection\Private\ImplementFunctionType.h>
#include <ReflectionGenerator\Specifiers.h>

namespace Engine
{
	namespace ReflectionGenerator
	{
		class REFLECTIONGENERATOR_API MetaFunction : public ImplementFunctionType, public Specifiers
		{
		public:
			MetaFunction(Type *TopNest) :
				ImplementFunctionType(TopNest)
			{
			}
			virtual ~MetaFunction(void)
			{
			}

			INLINE void GetSignature(String &Signature, String &SignatureName) const
			{
				m_ReturnType.GetSignature(Signature);

				SignatureName = m_Name + "Signature";

				Signature += "(" + m_TopNest->GetFullQualifiedName() + "::*" + SignatureName + ")";

				Signature += "(";

				for (uint32 i = 0; i < m_Parameters.GetSize(); i++)
				{
					const Parameter &param = m_Parameters[i];
					param.GetSignature(Signature);

					if (i < m_Parameters.GetSize() - 1)
						Signature += ", ";
				}

				Signature += ")";

				if (m_IsConst)
					Signature += " const";
			}

			INLINE String GetUniqueName(void) const
			{
				return (m_TopNest->GetName() + "_") + m_Name;
			}

		protected:
			void InvokeInternal(void *TargetObject, AnyDataType &ReturnValue, const ArgumentsList *Argumetns) const override
			{
			}
		};
	}
}
#endif
