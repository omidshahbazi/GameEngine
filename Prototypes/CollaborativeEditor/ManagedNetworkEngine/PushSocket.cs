using NetworkEngine.Interop;

namespace NetwokEngine
{
	public class PushSocket : SocketBase
	{
		public PushSocket(Context Context, int BufferSize) :
			base(Context, BufferSize, SocketType.PUSH)
		{ }
	}
}