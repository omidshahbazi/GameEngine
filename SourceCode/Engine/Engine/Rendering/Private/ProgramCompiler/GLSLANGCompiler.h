// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef GLSLANG_COMPILER_H
#define GLSLANG_COMPILER_H

#include <Common\PrimitiveTypes.h>
#include <Platform\PlatformMemory.h>
#include <glslang\include\glslang_c_interface.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;

	namespace Rendering
	{
		namespace Private
		{
			namespace ProgramCompiler
			{
				class GLSLANGCompiler
				{
				public:
					static bool Compile(glslang_client_t Client, glslang_stage_t Type, cstr Source, byte* SPIRV, uint16& Size, str ErrorMessage)
					{
						glslang_initialize_process();

						glslang_input_t input = {};
						input.language = GLSLANG_SOURCE_GLSL;
						input.stage = Type;
						input.client = Client;
						input.client_version = (Client == GLSLANG_CLIENT_OPENGL ? GLSLANG_TARGET_OPENGL_450 : GLSLANG_TARGET_VULKAN_1_2);
						input.target_language = GLSLANG_TARGET_SPV;
						input.target_language_version = GLSLANG_TARGET_SPV_1_5;
						input.code = Source;
						input.default_version = 460;
						input.default_profile = GLSLANG_CORE_PROFILE;
						input.force_default_version_and_profile = false;
						input.forward_compatible = false;
						input.messages = GLSLANG_MSG_DEFAULT_BIT;

						glslang_resource_t res = {};
						input.resource = &res;

						glslang_shader_t* shader = glslang_shader_create(&input);

						if (!glslang_shader_preprocess(shader, &input))
						{
							cstr message = glslang_shader_get_info_log(shader);
							PlatformMemory::Copy(message, ErrorMessage, CharacterUtility::GetLength(message));

							return false;
						}

						if (!glslang_shader_parse(shader, &input))
						{
							cstr message = glslang_shader_get_info_log(shader);
							PlatformMemory::Copy(message, ErrorMessage, CharacterUtility::GetLength(message));

							return false;
						}

						glslang_program_t* program = glslang_program_create();

						glslang_program_add_shader(program, shader);

						if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT))
						{
							cstr message = glslang_program_get_info_log(program);
							PlatformMemory::Copy(message, ErrorMessage, CharacterUtility::GetLength(message));

							return false;
						}

						glslang_program_SPIRV_generate(program, input.stage);

						uint32* data = glslang_program_SPIRV_get_ptr(program);
						uint32 size = glslang_program_SPIRV_get_size(program);

						uint32 desiredSize = size * sizeof(uint32);

						if (desiredSize > Size)
						{
							cstr message = "Not enough SPIRV buffer size";
							PlatformMemory::Copy(message, ErrorMessage, CharacterUtility::GetLength(message));

							return false;
						}

						PlatformMemory::Copy(ReinterpretCast(byte*, data), SPIRV, desiredSize);
						Size = desiredSize;

						glslang_shader_delete(shader);
						glslang_program_delete(program);
						glslang_finalize_process();

						return true;
					}
				};
			}
		}
	}
}

#endif