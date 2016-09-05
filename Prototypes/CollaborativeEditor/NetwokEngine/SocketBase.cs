using NetworkEngine.Interop;

namespace NetwokEngine
{
	public abstract class SocketBase : NativeObject
	{
		protected SocketBase(Context Context, SocketType Type)
		{
			NativeHandler = LibZmq.zmq_socket(Context.NativeHandler, (int)Type);
		}

		public override void Dispose()
		{
			LibZmq.zmq_close(NativeHandler);
		}
	}
}