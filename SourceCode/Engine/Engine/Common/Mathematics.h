// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once

#ifndef MATHEMATICS_H
#define MATHEMATICS_H

namespace Engine
{
	namespace Common
	{
		class Mathematics
		{
		public:
			template<typename T> static T Clamp(T Value, T Min, T Max)
			{
				if (Value < Min)
					return Min;

				if (Value > Max)
					return Max;

				return Value;
			}
		};
	}
}

#endif