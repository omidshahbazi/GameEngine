// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.System.Build;
using System;
using System.Collections.Generic;
using System.Reflection;
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

		public static Type[] GetTypes<T>(this Assembly Self) where T : class
		{
			List<Type> result = new List<Type>();

			Type[] types = Self.GetTypes();

			Type baseType = typeof(T);

			foreach (Type type in types)
			{
				if (!baseType.IsAssignableFrom(type))
					continue;

				result.Add(type);
			}

			return result.ToArray();
		}
	}
}
