// Copyright 2016-2020 ?????????????. All Rights Reserved.

namespace Engine
{
	public abstract class TargetRules
	{
		internal const string FilePostfix = ".TargetRules.cs";

		public abstract string ModuleName
		{
			get;
		}
	}
}