// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GLSL_INTRINSIC_Provider_H
#define GLSL_INTRINSIC_Provider_H

#include <APIIntrinsic\IIntrinsicProvider.h>

namespace Engine
{
	using namespace APIIntrinsic;

	namespace GLSLIntrinsic
	{
		namespace Private
		{
			class GLSLIntrinsicProvider : public IIntrinsicProvider
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