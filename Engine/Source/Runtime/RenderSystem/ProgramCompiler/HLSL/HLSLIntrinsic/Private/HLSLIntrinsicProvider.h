// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef HLSL_INTRINSIC_Provider_H
#define HLSL_INTRINSIC_Provider_H

#include <APIIntrinsic\IIntrinsicProvider.h>

namespace Engine
{
	using namespace APIIntrinsic;

	namespace HLSLIntrinsic
	{
		namespace Private
		{
			class HLSLIntrinsicProvider : public IIntrinsicProvider
			{
			public:
				void Initialize(IInitializeHelper* Helper) override;

				virtual const ConstantMap& GetConstants(void) const override
				{
					return m_Constants;
				}

				virtual const FunctionMap& GetFunctions(void) const override
				{
					return m_Functions;
				}

			private:
				ConstantMap m_Constants;
				FunctionMap m_Functions;
			};
		}
	}
}

#endif