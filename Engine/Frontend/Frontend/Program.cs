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
#if !DEBUG
			try
			{
#endif
				EntryPoint.Start(ArgumentParser.Parse(Args));

				return 0;
#if !DEBUG
			}
			catch (FrontendException ex)
			{
				ConsoleHelper.WriteError(ex.Message);
			}
			catch (Exception ex)
			{
				ConsoleHelper.WriteException(ex, "");
			}

			return 1;
#endif
		}
	}
}