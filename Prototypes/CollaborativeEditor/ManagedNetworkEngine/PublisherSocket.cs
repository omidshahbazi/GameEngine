using NetworkEngine.Interop;

namespace NetwokEngine
{
	public class PublisherSocket : SocketBase
	{
		public PublisherSocket(Context Context, int BufferSize) :
			base(Context, BufferSize, SocketType.PUB)
		{ }
	}
}