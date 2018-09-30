// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMPILER_H
#define COMPILER_H

#include <Containers\Strings.h>
#include <Rendering\DeviceInterfarce.h>

namespace Engine
{
	namespace Rendering
	{
		namespace Private
		{
			namespace ShaderCompiler
			{
				class Compiler
				{
				public:
					bool Compile(DeviceInterfarce::Type DeviceType, const String &Shader, String &Result);
				};
			}
		}
	}
}

#endif