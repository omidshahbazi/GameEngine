// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <WrapperTool/WrapperGenerator.h>
#include <Utility/ArgumentParser.h>

using namespace Engine::Common;
using namespace Engine::Utility;
using namespace Engine::WrapperTool;

int main(uint8 ArgumentsCount, const char8** Arguments)
{

	//WrapperGenerator wg("CoreSystem.dll", "CORESYSTEM_API", "E:/Projects/GameEngine/SourceCode/Engine/Engine/CoreSystem/Core.h", "E:/Projects/GameEngine/SourceCode/Engine/Intermediate/CoreSystem/Generated/Core.Wrapper");
	WrapperGenerator wg("Containers.dll", "CONTAINERS_API", "E:/Projects/GameEngine/SourceCode/Engine/Engine/Containers/Vector2.h", "E:/Projects/GameEngine/SourceCode/Engine/Intermediate/Containers/Generated/Vector2.Wrapper");
	wg.Generate();

	return 0;



	ArgumentParser args(ArgumentsCount, Arguments, true);

	if (args.GetCount() >= 3)
	{
		WrapperGenerator wg(args.GetAsString(0), args.GetAsString(1), args.GetAsString(2), args.GetAsString(3));
		if (wg.Generate())
			return 0;
	}

	return 1;
}