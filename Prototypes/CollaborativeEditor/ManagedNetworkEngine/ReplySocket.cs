using NetworkEngine.Interop;

namespace NetwokEngine
{
	public class ReplySocket : SocketBase
	{
		public ReplySocket(Context Context, int BufferSize) :
			base(Context, BufferSize, SocketType.REP)
		{ }
	}
}