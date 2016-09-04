using System;
using System.Windows.Forms;
using ZeroMQ.Interop;

namespace Editor
{
    static class Program
	{
		[STAThread]
		static void Main()
		{
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault(false);

            IntPtr context = LibZmq.zmq_ctx_new();
            IntPtr socket = LibZmq.zmq_socket(context, 1);
            LibZmq.zmq_bind(socket, "");
		}
	}
}
