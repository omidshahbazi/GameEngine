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
			RequestSocket socket = new RequestSocket(context, 128);
			socket.Connect("tcp://localhost:5555");

			while (true)
			{
				socket.Write(System.Text.Encoding.ASCII.GetBytes("Test"));

				byte[] buffer = socket.Read();

				if (buffer != null)
					Console.Write(System.Text.Encoding.ASCII.GetString(buffer));
			}
		}
	}
}
