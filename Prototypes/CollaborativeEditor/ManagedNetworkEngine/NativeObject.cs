using System;

namespace NetwokEngine
{
	public abstract class NativeObject : IDisposable
	{
		public IntPtr NativeHandler
		{
			get;
			protected set;
		}

		public abstract void Dispose();
	}
}