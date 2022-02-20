// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef I_FENCE_H
#define I_FENCE_H

namespace Engine
{
	namespace RenderDevice
	{
		class IFence
		{
		public:
			virtual ~IFence(void)
			{
			}

			virtual bool GetIsPassed(void) = 0;

			virtual bool Wait(void) = 0;
		};
	}
}

#endif