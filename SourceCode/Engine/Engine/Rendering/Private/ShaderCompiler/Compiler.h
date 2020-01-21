// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMPILER_H
#define COMPILER_H

#include <Containers\Strings.h>
#include <Rendering\DeviceInterface.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class Compiler
				{
				public:
					bool Compile(DeviceInterface::Type DeviceType, const String &Shader, String &VertexShader, String &FragmentShader);
				};
			}
		}
	}
}

#endif