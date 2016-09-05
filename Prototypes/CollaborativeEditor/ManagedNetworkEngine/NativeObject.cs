using System;
using System.Runtime.InteropServices;

namespace NetwokEngine
{
	public abstract class NativeObject : IDisposable
	{
		protected IntPtr NativeHandler
		{
			get;
			set;
		}

		public abstract void Dispose();

		public static implicit operator IntPtr(NativeObject Object)
		{
			return Object.NativeHandler;
		}
	}
}