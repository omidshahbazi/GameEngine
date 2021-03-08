// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ProgramCompiler\GLSLANGCompiler.h>
#include <Common\CharacterUtility.h>
#include <Platform\PlatformMemory.h>
#include <glslang\SPIRV\GlslangToSpv.h>

using namespace spv;

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
				SINGLETON_DEFINITION(GLSLANGCompiler)

				GLSLANGCompiler::GLSLANGCompiler(void)
				{
					InitializeProcess();
				}

				GLSLANGCompiler::~GLSLANGCompiler(void)
				{
					FinalizeProcess();
				}

				bool GLSLANGCompiler::Compile(EShClient Client, EShLanguage Type, cstr Source, byte* SPIRV, uint16& Size, str ErrorMessage)
				{
#define CHECK_CALL(Expression, Object) \
					if (!(Expression)) \
					{ \
						cstr message = Object.getInfoLog(); \
						PlatformMemory::Copy(message, ErrorMessage, CharacterUtility::GetLength(message)); \
						return false; \
					}

					const int32 DEFAULT_VERSION = 460;

					TShader shader(Type);
					shader.setStrings(&Source, 1);
					shader.setEnvInput(EShSourceGlsl, Type, Client, DEFAULT_VERSION);
					shader.setEnvClient(Client, EShTargetOpenGL_450);
					shader.setEnvTarget(EShTargetSpv, EShTargetSpv_1_5);

					//shader.preprocess()

					TBuiltInResource builtinResource = {};

					CHECK_CALL(shader.parse(&builtinResource, DEFAULT_VERSION, ECoreProfile, false, true, EShMsgRelaxedErrors), shader);

					TProgram program;
					program.addShader(&shader);
					CHECK_CALL(program.link(EShMsgSpvRules), program);

					std::vector<uint32> spirvVector(Size);
					spirvVector.clear();

					SpvBuildLogger logger;
					SpvOptions options = {};

					GlslangToSpv(*program.getIntermediate(Type), spirvVector, &logger, &options);

					const std::string allMessages = logger.getAllMessages();
					if (allMessages.length() != 0)
					{
						PlatformMemory::Copy(allMessages.c_str(), ErrorMessage, allMessages.length());

						return false;
					}

					uint32 desiredSize = spirvVector.size() * sizeof(uint32);
					if (desiredSize > Size)
					{
						cstr message = "Not enough SPIRV buffer size";
						PlatformMemory::Copy(message, ErrorMessage, CharacterUtility::GetLength(message));

						return false;
					}

					PlatformMemory::Copy(ReinterpretCast(byte*, spirvVector.data()), SPIRV, desiredSize);
					Size = desiredSize;

					return true;
#undef CHECK_CALL
				}

				bool GLSLANGCompiler::Compile(glslang_client_t Client, glslang_stage_t Type, cstr Source, byte* SPIRV, uint16& Size, str ErrorMessage)
				{
					glslang_initialize_process();

					glslang_input_t input = {};
					input.language = GLSLANG_SOURCE_GLSL;
					input.stage = Type;
					input.client = Client;
					input.client_version = (Client == GLSLANG_CLIENT_OPENGL ? GLSLANG_TARGET_OPENGL_450 : GLSLANG_TARGET_VULKAN_1_2);
					input.target_language = GLSLANG_TARGET_SPV;
					input.target_language_version = GLSLANG_TARGET_SPV_1_2;
					input.code = Source;
					input.default_version = 460;
					input.default_profile = GLSLANG_CORE_PROFILE;
					input.force_default_version_and_profile = false;
					input.forward_compatible = false;
					input.messages = GLSLANG_MSG_DEFAULT_BIT;

					glslang_resource_t res = {
						/* .MaxLights = */ 32,
						/* .MaxClipPlanes = */ 6,
						/* .MaxTextureUnits = */ 32,
						/* .MaxTextureCoords = */ 32,
						/* .MaxVertexAttribs = */ 64,
						/* .MaxVertexUniformComponents = */ 4096,
						/* .MaxVaryingFloats = */ 64,
						/* .MaxVertexTextureImageUnits = */ 32,
						/* .MaxCombinedTextureImageUnits = */ 80,
						/* .MaxTextureImageUnits = */ 32,
						/* .MaxFragmentUniformComponents = */ 4096,
						/* .MaxDrawBuffers = */ 32,
						/* .MaxVertexUniformVectors = */ 128,
						/* .MaxVaryingVectors = */ 8,
						/* .MaxFragmentUniformVectors = */ 16,
						/* .MaxVertexOutputVectors = */ 16,
						/* .MaxFragmentInputVectors = */ 15,
						/* .MinProgramTexelOffset = */ -8,
						/* .MaxProgramTexelOffset = */ 7,
						/* .MaxClipDistances = */ 8,
						/* .MaxComputeWorkGroupCountX = */ 65535,
						/* .MaxComputeWorkGroupCountY = */ 65535,
						/* .MaxComputeWorkGroupCountZ = */ 65535,
						/* .MaxComputeWorkGroupSizeX = */ 1024,
						/* .MaxComputeWorkGroupSizeY = */ 1024,
						/* .MaxComputeWorkGroupSizeZ = */ 64,
						/* .MaxComputeUniformComponents = */ 1024,
						/* .MaxComputeTextureImageUnits = */ 16,
						/* .MaxComputeImageUniforms = */ 8,
						/* .MaxComputeAtomicCounters = */ 8,
						/* .MaxComputeAtomicCounterBuffers = */ 1,
						/* .MaxVaryingComponents = */ 60,
						/* .MaxVertexOutputComponents = */ 64,
						/* .MaxGeometryInputComponents = */ 64,
						/* .MaxGeometryOutputComponents = */ 128,
						/* .MaxFragmentInputComponents = */ 128,
						/* .MaxImageUnits = */ 8,
						/* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
						/* .MaxCombinedShaderOutputResources = */ 8,
						/* .MaxImageSamples = */ 0,
						/* .MaxVertexImageUniforms = */ 0,
						/* .MaxTessControlImageUniforms = */ 0,
						/* .MaxTessEvaluationImageUniforms = */ 0,
						/* .MaxGeometryImageUniforms = */ 0,
						/* .MaxFragmentImageUniforms = */ 8,
						/* .MaxCombinedImageUniforms = */ 8,
						/* .MaxGeometryTextureImageUnits = */ 16,
						/* .MaxGeometryOutputVertices = */ 256,
						/* .MaxGeometryTotalOutputComponents = */ 1024,
						/* .MaxGeometryUniformComponents = */ 1024,
						/* .MaxGeometryVaryingComponents = */ 64,
						/* .MaxTessControlInputComponents = */ 128,
						/* .MaxTessControlOutputComponents = */ 128,
						/* .MaxTessControlTextureImageUnits = */ 16,
						/* .MaxTessControlUniformComponents = */ 1024,
						/* .MaxTessControlTotalOutputComponents = */ 4096,
						/* .MaxTessEvaluationInputComponents = */ 128,
						/* .MaxTessEvaluationOutputComponents = */ 128,
						/* .MaxTessEvaluationTextureImageUnits = */ 16,
						/* .MaxTessEvaluationUniformComponents = */ 1024,
						/* .MaxTessPatchComponents = */ 120,
						/* .MaxPatchVertices = */ 32,
						/* .MaxTessGenLevel = */ 64,
						/* .MaxViewports = */ 16,
						/* .MaxVertexAtomicCounters = */ 0,
						/* .MaxTessControlAtomicCounters = */ 0,
						/* .MaxTessEvaluationAtomicCounters = */ 0,
						/* .MaxGeometryAtomicCounters = */ 0,
						/* .MaxFragmentAtomicCounters = */ 8,
						/* .MaxCombinedAtomicCounters = */ 8,
						/* .MaxAtomicCounterBindings = */ 1,
						/* .MaxVertexAtomicCounterBuffers = */ 0,
						/* .MaxTessControlAtomicCounterBuffers = */ 0,
						/* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
						/* .MaxGeometryAtomicCounterBuffers = */ 0,
						/* .MaxFragmentAtomicCounterBuffers = */ 1,
						/* .MaxCombinedAtomicCounterBuffers = */ 1,
						/* .MaxAtomicCounterBufferSize = */ 16384,
						/* .MaxTransformFeedbackBuffers = */ 4,
						/* .MaxTransformFeedbackInterleavedComponents = */ 64,
						/* .MaxCullDistances = */ 8,
						/* .MaxCombinedClipAndCullDistances = */ 8,
						/* .MaxSamples = */ 4,
						/* .maxMeshOutputVerticesNV = */ 256,
						/* .maxMeshOutputPrimitivesNV = */ 512,
						/* .maxMeshWorkGroupSizeX_NV = */ 32,
						/* .maxMeshWorkGroupSizeY_NV = */ 1,
						/* .maxMeshWorkGroupSizeZ_NV = */ 1,
						/* .maxTaskWorkGroupSizeX_NV = */ 32,
						/* .maxTaskWorkGroupSizeY_NV = */ 1,
						/* .maxTaskWorkGroupSizeZ_NV = */ 1,
						/* .maxMeshViewCountNV = */ 4,
						/* .maxDualSourceDrawBuffersEXT = */ 1,

						/* .limits = */ {
							/* .nonInductiveForLoops = */ 1,
							/* .whileLoops = */ 1,
							/* .doWhileLoops = */ 1,
							/* .generalUniformIndexing = */ 10,
							/* .generalAttributeMatrixVectorIndexing = */ 1,
							/* .generalVaryingIndexing = */ 1,
							/* .generalSamplerIndexing = */ 1,
							/* .generalVariableIndexing = */ 1,
							/* .generalConstantMatrixVectorIndexing = */ 1,
						} };
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
			}
		}
	}
}