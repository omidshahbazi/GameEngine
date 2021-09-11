// Copyright 2016-2020 ?????????????. All Rights Reserved.
using Engine.Frontend.Utilities;
using GameFramework.Common.Utilities;
using System;

namespace Engine.Frontend
{
	class Program
	{
		static int Main(string[] Args)
		{
			try
			{
				EntryPoint.Start(ArgumentParser.Parse(Args));

				return 0;
			}
			catch (FrontendException ex)
			{
				ConsoleHelper.WriteError(ex.Message);

#if DEBUG
				Console.Read();
#endif
				return 1;
			}
			catch (Exception ex)
			{
				ConsoleHelper.WriteException(ex, "");

#if DEBUG
				Console.Read();
#endif
				return 1;
			}
		}
	}
}