using NetworkEngine.Interop;

namespace NetwokEngine
{
	public class DealerSocket : SocketBase
	{
		public DealerSocket(Context Context, int BufferSize) :
			base(Context, BufferSize, SocketType.DEALER)
		{ }
	}
}