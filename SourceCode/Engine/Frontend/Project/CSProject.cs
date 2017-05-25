// Copyright 2016-2017 ?????????????. All Rights Reserved.

namespace Engine.Frontend.Project
{
	class CSProject : ProjectBase
	{
		public enum FrameworkVersions
		{
			v2_0 = 0,
			v3_0,
			v3_5,
			v4_0,
			v4_5,
			v4_5_1
		}

		private StringList referenceBinaryFiles = new StringList();

		public FrameworkVersions FrameworkVersion
		{
			get;
			set;
		}

		public virtual string[] ReferenceBinaryFiles
		{
			get { return referenceBinaryFiles.ToArray(); }
		}

		public virtual void AddReferenceBinaryFile(string FilePath)
		{
			referenceBinaryFiles.Add(FilePath);
		}
	}
}
