using NetworkEngine.Interop;
using System;
using System.Runtime.InteropServices;

namespace NetwokEngine
{
	public abstract class SocketBase : NativeObject
	{
		private int bufferSize;
		private IntPtr buffer;

		protected SocketBase(Context Context, int BufferSize, SocketType Type)
		{
			NativeHandler = LibZmq.zmq_socket(Context.NativeHandler, (int)Type);

			bufferSize = BufferSize;
			buffer = Marshal.AllocHGlobal(bufferSize);
		}

		public virtual void Bind(string Address)
		{
			Marshal.FreeHGlobal(buffer);
			LibZmq.zmq_bind(NativeHandler, Address);
		}

		public virtual byte[] Read()
		{
			int length = LibZmq.zmq_buffer_recv(NativeHandler, buffer, bufferSize, 0);

			if (length == 0)
				return null;

			byte[] buf = new byte[length];
			Marshal.Copy(buffer, buf, 0, length);

			return buf;
		}

		public override void Dispose()
		{
			LibZmq.zmq_close(NativeHandler);
		}
	}
}