// Copyright 2016-2020 ?????????????. All Rights Reserved.

namespace Engine.Frontend
{
	public abstract class TargetRules
	{
		public const string FilePostfix = ".TargetRules.cs";

		public abstract string ModuleName
		{
			get;
		}
	}
}