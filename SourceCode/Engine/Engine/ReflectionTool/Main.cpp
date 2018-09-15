// Copyright 2012-2015 ?????????????. All Rights Reserved.
#include <ReflectionTool\ReflectionGenerator.h>
#include <Utility\ArgumentParser.h>

using namespace Engine::Common;
using namespace Engine::Utility;
using namespace Engine::ReflectionTool;

int main(uint8 ArgumentsCount, const char8 **Arguments)
{
	ArgumentParser args(ArgumentsCount, Arguments, true);

	if (args.GetCount() >= 2)
	{
		ReflectionGenerator rg(args.GetAsString(0), args.GetAsString(1));
		if (rg.Generate())
			return 0;
	}



	//ReflectionGenerator rg("D:\\Projects\\Game Engine\\SourceCode\\Engine\\Engine\\Basic\\Public\\AnyDataType.h", "");
	//if (rg.Generate())
	//	return 0;

	return 1;
}