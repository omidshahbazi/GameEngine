using NetworkEngine.Interop;

namespace NetwokEngine
{
	public class RequestSocket : SocketBase
	{
		public RequestSocket(Context Context, int BufferSize) :
			base(Context, BufferSize, SocketType.REQ)
		{ }
	}
}