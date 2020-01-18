using NetworkEngine.Interop;

namespace NetwokEngine
{
	public class PairSocket : SocketBase
	{
		public PairSocket(Context Context, int BufferSize) :
			base(Context, BufferSize, SocketType.PAIR)
		{ }
	}
}