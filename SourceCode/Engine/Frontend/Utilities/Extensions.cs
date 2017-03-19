// Copyright 2016-2017 ?????????????. All Rights Reserved.
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
