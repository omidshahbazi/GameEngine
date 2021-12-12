// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ProgramCompiler\Compiler.h>
#include <ProgramCompiler\ProgramToAPICompiler.h>
#include <ProgramCompilerCommon\ProgramCompilerException.h>
#include <ProgramCompilerCommon\IByteCodeCompiler.h>
#include <ProgramCompilerCommon\Common.h>
#include <RenderDevice\IDevice.h>
#include <DynamicModuleSystem\ModuleManager.h>
#include <Debugging\CoreDebug.h>

namespace Engine
{
	using namespace ProgramCompilerCommon;
	using namespace RenderDevice;
	using namespace DynamicModuleSystem;
	using namespace Debugging;

	namespace ProgramCompiler
	{
		bool Compiler::Compile(const ProgramInfo& Info, const DeviceTypes* Types, uint8 TypesCount, CompiledProgramInfo* CompiledInfos)
		{
			static IByteCodeCompiler* compilers[DEVICE_TYPE_COUNT] = {};

			if (Info.Source.GetLength() == 0)
				return false;

			OutputInfo outputInfos[DEVICE_TYPE_COUNT];
			try
			{
				ProgramToAPICompiler::GetInstance()->Compile(&Info, Types, TypesCount, outputInfos);
			}
			catch (const Exception& ex)
			{
				for (uint8 i = 0; i < TypesCount; ++i)
				{
					CompiledProgramInfo& compiledProgrm = CompiledInfos[i];

					compiledProgrm.VertexShader.Size = 0;
					compiledProgrm.TessellationShader.Size = 0;
					compiledProgrm.GeometryShader.Size = 0;
					compiledProgrm.FragmentShader.Size = 0;
					compiledProgrm.ComputeShader.Size = 0;

					compiledProgrm.ErrorMessage = ex.ToString();
				}

				return false;
			}

			for (uint8 i = 0; i < TypesCount; ++i)
			{
				if (outputInfos->VertexShader == String::Empty &&
					outputInfos->TessellationShader == String::Empty &&
					outputInfos->GeometryShader == String::Empty &&
					outputInfos->FragmentShader == String::Empty &&
					outputInfos->ComputeShader == String::Empty)
					continue;

				DeviceTypes deviceType = Types[i];

				OutputInfo& outputInfo = outputInfos[i];
				CompiledProgramInfo& compiledProgrm = CompiledInfos[i];

				IByteCodeCompiler* compiler = compilers[(int32)deviceType];

				if (compiler == nullptr)
				{
					String moduleName = "";
					switch (deviceType)
					{
					case DeviceTypes::OpenGL:
					case DeviceTypes::Vulkan:
						moduleName = "GLSLCompiler";
						break;

					case DeviceTypes::DirectX12:
						moduleName = "HLSLCompiler";
						break;

					default:
						CoreDebugAssert(Categories::ProgramCompiler, false, "Device type is not supported");
					}

					compiler = compilers[(int32)deviceType] = ModuleManager::GetInstance()->Load<IByteCodeCompiler>(moduleName);
					CoreDebugAssert(Categories::ProgramCompiler, compiler != nullptr, "Couldn't load %S module", moduleName);
				}

				try
				{
#define IMPLEMENT_COMPILE(StageType, StageName) \
					if (outputInfo.StageName == String::Empty) \
					{ \
						compiledProgrm.StageName.Buffer = nullptr; \
						compiledProgrm.StageName.Size = 0; \
					} \
					else \
						compiler->Compile(deviceType, StageType, outputInfo.StageName.GetValue(), Constants::ENTRY_POINT_NAME, false, compiledProgrm.StageName.Buffer, compiledProgrm.StageName.Size);

					IMPLEMENT_COMPILE(Stages::Vertex, VertexShader);
					IMPLEMENT_COMPILE(Stages::Tessellation, TessellationShader);
					IMPLEMENT_COMPILE(Stages::Geometry, GeometryShader);
					IMPLEMENT_COMPILE(Stages::Fragment, FragmentShader);
					IMPLEMENT_COMPILE(Stages::Compute, ComputeShader);

#undef IMPLEMENT_COMPILE
				}
				catch (const Exception& ex)
				{
					CoreDebugAssert(Categories::ProgramCompiler, false, "It seems that an invalid shader tries to get compiled in an API {%S}", ex.ToString());
				}

				compiledProgrm.MetaInfo = outputInfo.MetaInfo;
			}

			return true;
		}
	}
}