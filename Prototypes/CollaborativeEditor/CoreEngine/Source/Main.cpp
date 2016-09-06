#include <Public\Context.h>
#include <Public\PairSocket.h>
#include <Public\Buffer.h>

using namespace NetworkEngine;

void main()
{
	Context context;
	PairSocket socket(&context, 128);
	socket.Bind("tcp://*:5555");

	while (true)
	{
		Buffer *buffer = socket.Read();

		socket.Write(buffer);


	}
}