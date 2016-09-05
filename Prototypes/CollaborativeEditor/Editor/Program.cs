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
			PairSocket socket = new PairSocket(context);

		}
	}
}
