using NetwokEngine;
using System;
using System.Windows.Forms;

namespace Editor
{
	static class Program
	{
		[STAThread]
		static void Main()
		{
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault(false);

			Context context = new Context();
			PairSocket socket = new PairSocket(context, 128);
			socket.Bind("tcp://localhost:5000");

			//while (true)
			//{
				socket.Write(System.Text.Encoding.ASCII.GetBytes("Test"));
				Console.Write(System.Text.Encoding.ASCII.GetString(socket.Read()));
			//}
		}
	}
}
