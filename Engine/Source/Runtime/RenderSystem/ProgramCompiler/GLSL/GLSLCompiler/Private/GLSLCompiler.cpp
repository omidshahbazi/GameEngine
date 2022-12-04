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
				/* MaxCombinedTextureImageUnits */ 16,
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
					/* nonInductiveForLoops */ true,
					/* whileLoops */ true,
					/* doWhileLoops */ true,
					/* generalUniformIndexing */ true,
					/* generalAttributeMatrixVectorIndexing */ true,
					/* generalVaryingIndexing */ true,
					/* generalSamplerIndexing */ true,
					/* generalVariableIndexing */ true,
					/* generalConstantMatrixVectorIndexing */ true
				}
			};

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
				shader.setEnvTarget(EShTargetSpv, EShTargetSpv_1_5);
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