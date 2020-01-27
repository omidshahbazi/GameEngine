// Copyright 2016-2020 ?????????????. All Rights Reserved.
using System;

namespace Engine.Frontend.Utilities
{
	static class Extensions
	{
		public static string ToHHMMSS(this TimeSpan a)
		{
			return a.ToString(@"hh\:mm\:ss");
		}
	}
}
