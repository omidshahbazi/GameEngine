using Engine.Containers;
using Engine.CoreSystem;
using System.Windows.Forms;

namespace Engine.Editor
{
	public partial class MainForm : Form
	{
		public MainForm()
		{
			InitializeComponent();

			Core core = Core.Instance;
			var window =  core.CreateWindow(new Vector2I() { X = 800, Y = 600 }, "Editor");
			core.Initialize();
		}
	}
}