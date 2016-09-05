using NetworkEngine.Interop;

namespace NetwokEngine
{
	public class Context : NativeObject
	{
		public Context()
		{
			NativeHandler = LibZmq.zmq_ctx_new();
        }

		public override void Dispose()
		{
			LibZmq.zmq_ctx_destroy(NativeHandler);
		}
	}
}