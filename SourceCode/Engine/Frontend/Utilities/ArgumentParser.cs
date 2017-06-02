// Copyright 2016-2017 ?????????????. All Rights Reserved.

using System;

namespace Engine.Frontend.Utilities
{
	class ArgumentParser
	{
		public class Argument
		{
			public string Name
			{
				get;
				private set;
			}

			public Argument(string Argument)
			{
				Name = Argument;
			}
		}

		private Argument[] arguments = null;

		public uint Count
		{
			get { return (uint)arguments.Length; }
		}

		public ArgumentParser(string[] Arguments, string Separator)
		{
			if (Arguments == null)
				return;

			arguments = new Argument[Arguments.Length];

			for (int i = 0; i < Arguments.Length; ++i)
			{
				string arg = Arguments[i];

				if (arg.StartsWith(Separator))
					arg = arg.Substring(Separator.Length);

				arguments[i] = new Argument(arg);
			}
		}

		public bool IsDefinedInEnum<T>(int Index)
		{
			Argument arg = GetArgumentInternal(Index);

			if (arg == null)
				return false;

			return Enum.IsDefined(typeof(T), arg.Name);
		}

		public T GetAsEnum<T>(int Index)
		{
			Argument arg = GetArgument(Index);

			return (T)Enum.Parse(typeof(T), arg.Name);
		}

		public Argument GetArgument(int Index)
		{
			Argument arg = GetArgumentInternal(Index);

			if (arg == null)
				throw new Exception("Argument [" + Index + "] is out of range");

			return arg;
		}

		private Argument GetArgumentInternal(int Index)
		{
			if (Index < arguments.Length)
				return arguments[Index];

			return null;
		}
	}
}
