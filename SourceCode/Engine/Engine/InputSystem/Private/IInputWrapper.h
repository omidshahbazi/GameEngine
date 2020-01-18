// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef I_INPUT_WRAPPER_H
#define I_INPUT_WRAPPER_H

namespace Engine
{
	namespace InputSystem
	{
		namespace Private
		{
			class IInputWrapper
			{
			public:
				virtual void Update(void) = 0;
			};
		}
	}
}

#endif