// Copyright 2016-2020 ?????????????. All Rights Reserved.
using System.Collections.Generic;

namespace Engine
{
	public abstract class TargetRules : BaseRules
	{
		internal const string FilePostfix = ".TargetRules.cs";

		public abstract string ModuleName
		{
			get;
		}

		public List<string> RequiredModuleNames
		{
			get;
			private set;
		}

		public TargetRules(OperatingSystems OperatingSystem, Configurations Configuration, Platforms Platform)
		{
			RequiredModuleNames = new List<string>();
		}
	}
}