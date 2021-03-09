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

				const TBuiltInResource DefaultTBuiltInResource = {};
				const TBuiltInResource DefaultTBuiltInResource1 = 
				{
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
						/* .generalUniformIndexing = */ 1,
						/* .generalAttributeMatrixVectorIndexing = */ 1,
						/* .generalVaryingIndexing = */ 1,
						/* .generalSamplerIndexing = */ 1,
						/* .generalVariableIndexing = */ 1,
						/* .generalConstantMatrixVectorIndexing = */ 1,
					} };

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

					TShader shader(Type);
					shader.setStrings(&Source, 1);
					shader.setEnvInput(EShSourceGlsl, Type, Client, DEFAULT_VERSION);
					shader.setEnvClient(Client, EShTargetOpenGL_450);
					shader.setEnvTarget(EShTargetSpv, EShTargetSpv_1_5);

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