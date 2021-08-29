// Copyright 2016-2020 ?????????????. All Rights Reserved.
using System;
using System.Text;

namespace Engine.Frontend.Utilities
{
	static class Extensions
	{
		public static string ToHHMMSS(this TimeSpan a)
		{
			return a.ToString(@"hh\:mm\:ss");
		}

		public static void AppendIndent(this StringBuilder Builder, uint Indent = 0)
		{
			for (uint i = 0; i < Indent; ++i)
				Builder.Append('\t');
		}
	}
}
