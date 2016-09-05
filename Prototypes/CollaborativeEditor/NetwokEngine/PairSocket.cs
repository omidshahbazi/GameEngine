using NetworkEngine.Interop;

namespace NetwokEngine
{
	public class PairSocket : SocketBase
	{
		public PairSocket(Context Context) :
			base(Context, SocketType.PAIR)
		{ }
	}
}