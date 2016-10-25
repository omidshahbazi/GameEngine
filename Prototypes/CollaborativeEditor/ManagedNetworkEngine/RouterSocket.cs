using NetworkEngine.Interop;

namespace NetwokEngine
{
	public class RouterSocket : SocketBase
	{
		public RouterSocket(Context Context, int BufferSize) :
			base(Context, BufferSize, SocketType.ROUTER)
		{ }
	}
}