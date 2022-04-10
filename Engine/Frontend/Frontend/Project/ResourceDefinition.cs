// Copyright 2016-2020 ?????????????. All Rights Reserved.

using System;

namespace Engine.Frontend.Project
{
	class ResourceDefinition
	{
		public string CompanyName
		{
			get;
			set;
		}

		public string ProductName
		{
			get;
			set;
		}

		public string ProductDescription
		{
			get;
			set;
		}

		public Version ProductVersion
		{
			get;
			set;
		}

		public string Copyright
		{
			get;
			set;
		}

		public string IconPath
		{
			get;
			set;
		}

		public ResourceDefinition()
		{
			ProductVersion = new Version();
		}
	}
}
