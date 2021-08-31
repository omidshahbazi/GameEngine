// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GLSLANG_COMPILER_H
#define GLSLANG_COMPILER_H

#include <Common\PrimitiveTypes.h>
#include <MemoryManagement\Singleton.h>
#include <GLSLANG\include\glslang\Public\ShaderLang.h>

using namespace glslang;

namespace Engine
{
	using namespace Common;
	using namespace MemoryManagement;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				class GLSLANGCompiler
				{
					SINGLETON_DECLARATION(GLSLANGCompiler)

				private:
					GLSLANGCompiler(void);

					~GLSLANGCompiler(void);

				public:
					bool Compile(EShClient Client, EShLanguage Type, cstr Source, cstr EntryPointName, byte* SPIRV, uint16& Size, str ErrorMessage);
				};
			}
		}
	}
}

#endif