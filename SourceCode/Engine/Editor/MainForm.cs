using Engine.Containers;
using Engine.CoreSystem;
using System.Windows.Forms;

namespace Engine.Editor
{
	//https://docs.microsoft.com/en-us/dotnet/framework/interop/passing-structures
	//https://docs.microsoft.com/en-us/dotnet/framework/interop/marshaling-classes-structures-and-unions
	public partial class MainForm : Form
	{
		private Core core = null;
		private Timer timer = null;

		public MainForm()
		{
			InitializeComponent();

			core = Core.Instance;
			var window = core.CreateWindow(new Vector2I() { X = 800, Y = 600 }, "Editor");
			core.Initialize();

			timer = new Timer();
			timer.Interval = 1;
			timer.Tick += Timer_Tick;
			timer.Start();
		}

		private void Timer_Tick(object sender, System.EventArgs e)
		{
			core.Update();
		}
	}
}