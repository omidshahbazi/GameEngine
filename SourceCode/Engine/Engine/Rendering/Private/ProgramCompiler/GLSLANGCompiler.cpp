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
				const int32 DEFAULT_VERSION = 460;

				const TBuiltInResource DefaultTBuiltInResource
				{
					/* MaxLights */ 0,
					/* MaxClipPlanes */ 0,
					/* MaxTextureUnits */ 0,
					/* MaxTextureCoords */ 0,
					/* MaxVertexAttribs */ 0,
					/* MaxVertexUniformComponents */ 0,
					/* MaxVaryingFloats */ 0,
					/* MaxVertexTextureImageUnits */ 0,
					/* MaxCombinedTextureImageUnits */ 0,
					/* MaxTextureImageUnits */ 0,
					/* MaxFragmentUniformComponents */ 0,
					/* MaxDrawBuffers */ 8,
					/* MaxVertexUniformVectors */ 0,
					/* MaxVaryingVectors */ 0,
					/* MaxFragmentUniformVectors */ 0,
					/* MaxVertexOutputVectors */ 0,
					/* MaxFragmentInputVectors */ 0,
					/* MinProgramTexelOffset */ 0,
					/* MaxProgramTexelOffset */ 0,
					/* MaxClipDistances */ 0,
					/* MaxComputeWorkGroupCountX */ 0,
					/* MaxComputeWorkGroupCountY */ 0,
					/* MaxComputeWorkGroupCountZ */ 0,
					/* MaxComputeWorkGroupSizeX */ 0,
					/* MaxComputeWorkGroupSizeY */ 0,
					/* MaxComputeWorkGroupSizeZ */ 0,
					/* MaxComputeUniformComponents */ 0,
					/* MaxComputeTextureImageUnits */ 0,
					/* MaxComputeImageUniforms */ 0,
					/* MaxComputeAtomicCounters */ 0,
					/* MaxComputeAtomicCounterBuffers */ 0,
					/* MaxVaryingComponents */ 0,
					/* MaxVertexOutputComponents */ 0,
					/* MaxGeometryInputComponents */ 0,
					/* MaxGeometryOutputComponents */ 0,
					/* MaxFragmentInputComponents */ 0,
					/* MaxImageUnits */ 0,
					/* MaxCombinedImageUnitsAndFragmentOutputs */ 0,
					/* MaxCombinedShaderOutputResources */ 0,
					/* MaxImageSamples */ 0,
					/* MaxVertexImageUniforms */ 0,
					/* MaxTessControlImageUniforms */ 0,
					/* MaxTessEvaluationImageUniforms */ 0,
					/* MaxGeometryImageUniforms */ 0,
					/* MaxFragmentImageUniforms */ 0,
					/* MaxCombinedImageUniforms */ 0,
					/* MaxGeometryTextureImageUnits */ 0,
					/* MaxGeometryOutputVertices */ 0,
					/* MaxGeometryTotalOutputComponents */ 0,
					/* MaxGeometryUniformComponents */ 0,
					/* MaxGeometryVaryingComponents */ 0,
					/* MaxTessControlInputComponents */ 0,
					/* MaxTessControlOutputComponents */ 0,
					/* MaxTessControlTextureImageUnits */ 0,
					/* MaxTessControlUniformComponents */ 0,
					/* MaxTessControlTotalOutputComponents */ 0,
					/* MaxTessEvaluationInputComponents */ 0,
					/* MaxTessEvaluationOutputComponents */ 0,
					/* MaxTessEvaluationTextureImageUnits */ 0,
					/* MaxTessEvaluationUniformComponents */ 0,
					/* MaxTessPatchComponents */ 0,
					/* MaxPatchVertices */ 0,
					/* MaxTessGenLevel */ 0,
					/* MaxViewports */ 0,
					/* MaxVertexAtomicCounters */ 0,
					/* MaxTessControlAtomicCounters */ 0,
					/* MaxTessEvaluationAtomicCounters */ 0,
					/* MaxGeometryAtomicCounters */ 0,
					/* MaxFragmentAtomicCounters */ 0,
					/* MaxCombinedAtomicCounters */ 0,
					/* MaxAtomicCounterBindings */ 0,
					/* MaxVertexAtomicCounterBuffers */ 0,
					/* MaxTessControlAtomicCounterBuffers */ 0,
					/* MaxTessEvaluationAtomicCounterBuffers */ 0,
					/* MaxGeometryAtomicCounterBuffers */ 0,
					/* MaxFragmentAtomicCounterBuffers */ 0,
					/* MaxCombinedAtomicCounterBuffers */ 0,
					/* MaxAtomicCounterBufferSize */ 0,
					/* MaxTransformFeedbackBuffers */ 0,
					/* MaxTransformFeedbackInterleavedComponents */ 0,
					/* MaxCullDistances */ 0,
					/* MaxCombinedClipAndCullDistances */ 0,
					/* MaxSamples */ 0,
					/* maxMeshOutputVerticesNV */ 0,
					/* maxMeshOutputPrimitivesNV */ 0,
					/* maxMeshWorkGroupSizeX_NV */ 0,
					/* maxMeshWorkGroupSizeY_NV */ 0,
					/* maxMeshWorkGroupSizeZ_NV */ 0,
					/* maxTaskWorkGroupSizeX_NV */ 0,
					/* maxTaskWorkGroupSizeY_NV */ 0,
					/* maxTaskWorkGroupSizeZ_NV */ 0,
					/* maxMeshViewCountNV */ 0,
					/* maxDualSourceDrawBuffersEXT */ 0,

					/* limits */
					{
						/* nonInductiveForLoops */ 0,
						/* whileLoops */ 0,
						/* doWhileLoops */ 0,
						/* generalUniformIndexing */ 0,
						/* generalAttributeMatrixVectorIndexing */ 0,
						/* generalVaryingIndexing */ 0,
						/* generalSamplerIndexing */ 0,
						/* generalVariableIndexing */ 0,
						/* generalConstantMatrixVectorIndexing */ 0,
					}
				};

				SINGLETON_DEFINITION(GLSLANGCompiler)

					GLSLANGCompiler::GLSLANGCompiler(void)
				{
					InitializeProcess();
				}

				GLSLANGCompiler::~GLSLANGCompiler(void)
				{
					FinalizeProcess();
				}

				bool GLSLANGCompiler::Compile(EShClient Client, EShLanguage Type, cstr Source, cstr EntryPointName, byte* SPIRV, uint16& Size, str ErrorMessage)
				{
#define CHECK_CALL(Expression, Object) \
					if (!(Expression)) \
					{ \
						cstr message = Object.getInfoLog(); \
						PlatformMemory::Copy(message, ErrorMessage, CharacterUtility::GetLength(message)); \
						return false; \
					}

					TShader shader(Type);
					shader.setStrings(&Source, 1);
					shader.setEnvInput(EShSourceGlsl, Type, Client, DEFAULT_VERSION);
					shader.setEnvClient(Client, EShTargetOpenGL_450);
					shader.setEnvTarget(EShTargetSpv, EShTargetSpv_1_5);
					shader.setEntryPoint(EntryPointName);

					CHECK_CALL(shader.parse(&DefaultTBuiltInResource, DEFAULT_VERSION, ECoreProfile, false, true, EShMsgRelaxedErrors), shader);
					
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
			}
		}
	}
}