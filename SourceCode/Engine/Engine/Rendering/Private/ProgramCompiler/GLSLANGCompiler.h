// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GLSLANG_COMPILER_H
#define GLSLANG_COMPILER_H

#include <Common\PrimitiveTypes.h>
#include <MemoryManagement\Singleton.h>
#include <glslang\Public\ShaderLang.h>
#include <glslang\include\glslang_c_interface.h>

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
					bool Compile(EShClient Client, EShLanguage Type, cstr Source, byte* SPIRV, uint16& Size, str ErrorMessage);

					static bool Compile(glslang_client_t Client, glslang_stage_t Type, cstr Source, byte* SPIRV, uint16& Size, str ErrorMessage);
				};
			}
		}
	}
}

#endif