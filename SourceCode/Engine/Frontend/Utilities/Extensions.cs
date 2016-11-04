// Copyright 2012-2015 ?????????????. All Rights Reserved.
using System;

namespace Frontend
{
	static class Extensions
	{
		public static string ToHHMMSS(this TimeSpan a)
		{
			return a.ToString(@"hh\:mm\:ss");
		}
	}
}
