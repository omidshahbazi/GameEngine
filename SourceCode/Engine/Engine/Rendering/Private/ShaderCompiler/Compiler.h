// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef COMPILER_H
#define COMPILER_H

#include <MemoryManagement\Singleton.h>
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
				//TODO: REQUIRED: Add shader validator and error generator before pass to api
				class RENDERING_API Compiler
				{
				public:
					class RENDERING_API IListener
					{
					public:
						virtual bool FetchShaderSource(const String& Name, String& Source)
						{
							return false;
						}
					};

					SINGLETON_DECLARATION(Compiler)

						LISTENER_DECLARATION(IListener)

				private:
					Compiler(void)
					{
					}

				public:
					bool Compile(DeviceInterface::Type DeviceType, const String& Version, const ShaderInfo* Info, String& VertexShader, String& FragmentShader);
				};
			}
		}
	}
}

#endif