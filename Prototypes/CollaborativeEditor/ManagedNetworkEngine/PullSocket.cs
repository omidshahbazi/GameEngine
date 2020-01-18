using NetworkEngine.Interop;

namespace NetwokEngine
{
	public class PullSocket : SocketBase
	{
		public PullSocket(Context Context, int BufferSize) :
			base(Context, BufferSize, SocketType.PULL)
		{ }
	}
}