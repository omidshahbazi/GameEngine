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
			NativeHandler = LibZmq.zmq_socket(Context, (int)Type);

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

		public virtual void Write(byte[] Buffer)
		{
			Marshal.Copy(Buffer, 0, buffer, Buffer.Length);
			LibZmq.zmq_buffer_send(NativeHandler, buffer, Buffer.Length, 0);
		}

		public override void Dispose()
		{
			LibZmq.zmq_close(NativeHandler);
		}
	}
}