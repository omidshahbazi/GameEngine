using NetworkEngine.Interop;

namespace NetwokEngine
{
	public class SubscriberSocket : SocketBase
	{
		public SubscriberSocket(Context Context, int BufferSize) :
			base(Context, BufferSize, SocketType.SUB)
		{ }
	}
}