// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <GLSLCompiler\Private\GLSLCompiler.h>
#include <Common\CharacterUtility.h>
#include <Platform\PlatformMemory.h>
#include <GLSLANG\include\glslang\SPIRV\GlslangToSpv.h>
#include <GLSLANG\include\glslang\Public\ShaderLang.h>
#include <Containers\Exception.h>

using namespace glslang;
using namespace spv;

namespace Engine
{
	using namespace Common;
	using namespace Platform;
	using namespace Containers;

	namespace GLSLCompiler
	{
		namespace Private
		{
			const TBuiltInResource DefaultTBuiltInResource = {
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
				/* .maxMeshOutputVerticesEXT = */ 256,
				/* .maxMeshOutputPrimitivesEXT = */ 256,
				/* .maxMeshWorkGroupSizeX_EXT = */ 128,
				/* .maxMeshWorkGroupSizeY_EXT = */ 128,
				/* .maxMeshWorkGroupSizeZ_EXT = */ 128,
				/* .maxTaskWorkGroupSizeX_EXT = */ 128,
				/* .maxTaskWorkGroupSizeY_EXT = */ 128,
				/* .maxTaskWorkGroupSizeZ_EXT = */ 128,
				/* .maxMeshViewCountEXT = */ 4,
				/* .maxDualSourceDrawBuffersEXT = */ 1,

				/* .limits = */ {
					/* .nonInductiveForLoops = */ true,
					/* .whileLoops = */ true,
					/* .doWhileLoops = */ true,
					/* .generalUniformIndexing = */ true,
					/* .generalAttributeMatrixVectorIndexing = */ true,
					/* .generalVaryingIndexing = */ true,
					/* .generalSamplerIndexing = */ true,
					/* .generalVariableIndexing = */ true,
					/* .generalConstantMatrixVectorIndexing = */ true
				} };

			void GLSLCompiler::Initialize(void)
			{
				InitializeProcess();
			}

			void GLSLCompiler::Uninitialize(void)
			{
				FinalizeProcess();
			}

			void GLSLCompiler::Compile(DeviceTypes Client, Stages Stage, cstr Source, cstr EntryPointName, bool DebugMode, byte* ByteCode, uint16& Size)
			{
				const int32 DEFAULT_VERSION = 460;

				EShClient client;
				EShTargetClientVersion clientVersion;
				switch (Client)
				{
				case DeviceTypes::OpenGL:
					client = EShClientOpenGL;
					clientVersion = EShTargetOpenGL_450;
					break;

				case DeviceTypes::Vulkan:
					client = EShClientVulkan;
					clientVersion = EShTargetVulkan_1_2;
					break;

				default:
					THROW_FULL_EXCEPTION(Categories::ProgramCompiler, "Compile GLSL shader has failed", "Unsupported Client has provided");
				}

#define CHECK_CALL(Expression, Object) \
					if (!(Expression)) \
						THROW_FULL_EXCEPTION(Categories::ProgramCompiler, "Compile GLSL shader has failed", Object.getInfoLog());

				EShLanguage type;
				switch (Stage)
				{
				case Stages::Vertex:
					type = EShLangVertex;
					break;

				case Stages::Hull:
					type = EShLangTessControl;
					break;

				case Stages::Domain:
					type = EShLangTessEvaluation;
					break;

				case Stages::Geometry:
					type = EShLangGeometry;
					break;

				case Stages::Fragment:
					type = EShLangFragment;
					break;

				case Stages::Compute:
					type = EShLangCompute;
					break;

				default:
					THROW_FULL_EXCEPTION(Categories::ProgramCompiler, "Compile GLSL shader has failed", "Unknown stage has provided");
				}

				TShader shader(type);
				shader.setStrings(&Source, 1);
				shader.setEnvInput(EShSourceGlsl, type, client, DEFAULT_VERSION);
				shader.setEnvClient(client, clientVersion);
				shader.setEnvTarget(EShTargetSpv, EShTargetSpv_1_6);
				shader.setEntryPoint(EntryPointName);

				EShMessages message = (EShMessages)(EShMsgRelaxedErrors | EShMsgSuppressWarnings);

				CHECK_CALL(shader.parse(&DefaultTBuiltInResource, DEFAULT_VERSION, ECoreProfile, false, true, message), shader);

				TProgram program;
				program.addShader(&shader);
				CHECK_CALL(program.link(EShMsgSpvRules), program);

				std::vector<uint32> spirvVector(Size);
				spirvVector.clear();

				SpvBuildLogger logger;
				SpvOptions options = {};

				GlslangToSpv(*program.getIntermediate(type), spirvVector, &logger, &options);

				const std::string allMessages = logger.getAllMessages();
				if (allMessages.length() != 0)
					THROW_FULL_EXCEPTION(Categories::ProgramCompiler, "Compile GLSL shader has failed", allMessages.c_str());

				uint32 desiredSize = spirvVector.size() * sizeof(uint32);
				if (desiredSize > Size)
					THROW_FULL_EXCEPTION(Categories::ProgramCompiler, "Compile GLSL shader has failed", "Not enough buffer size for shader machine code");

				PlatformMemory::Copy(ReinterpretCast(byte*, spirvVector.data()), ByteCode, desiredSize);
				Size = desiredSize;
#undef CHECK_CALL
			}
		}
	}
}