#include <Public\Context.h>
#include <Public\PairSocket.h>
#include <Public\Buffer.h>

void main()
{
	Context context;
	PairSocket socket(&context, 128);
	socket.Bind("tcp://*:5000");

	while (true)
	{
		socket.Read();
	}
}