// Copyright 2016-2017 ?????????????. All Rights Reserved.

namespace Engine.Frontend.Project
{
	class CSProject : ProjectBase
	{
		public class Profile : ProfileBase
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

			public FrameworkVersions FrameworkVersion
			{
				get;
				set;
			}

			public Profile(ProjectBase Project) :
				base(Project)
			{
			}
		}

		private StringList referenceBinaryFiles = new StringList();

		public virtual string[] ReferenceBinaryFiles
		{
			get { return referenceBinaryFiles.ToArray(); }
		}

		public override ProfileBase CreateProfile()
		{
			Profile profile = new Profile(this);
			AddProfile(profile);
			return profile;
		}

		public virtual void AddReferenceBinaryFile(string FilePath)
		{
			referenceBinaryFiles.Add(FilePath);
		}
	}
}
