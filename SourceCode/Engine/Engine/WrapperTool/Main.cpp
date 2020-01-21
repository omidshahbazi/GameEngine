// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <WrapperTool\WrapperGenerator.h>
#include <Utility\ArgumentParser.h>

using namespace Engine::Common;
using namespace Engine::Utility;
using namespace Engine::WrapperTool;

int main(uint8 ArgumentsCount, const char8** Arguments)
{
	ArgumentParser args(ArgumentsCount, Arguments, true);

	if (args.GetCount() >= 2)
	{
		WrapperGenerator wg(args.GetAsString(0), args.GetAsString(1));
		if (wg.Generate())
			return 0;
	}

	return 1;
}