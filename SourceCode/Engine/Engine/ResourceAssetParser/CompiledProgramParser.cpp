// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\CompiledProgramParser.h>
#include <Rendering\Private\OpenGL\OpenGLDevice.h>
#include <Rendering\Private\DirectX12\DirectX12Device.h>
#include <Rendering\Private\Vulkan\VulkanDevice.h>
#include <Rendering\Private\ProgramCompiler\Compiler.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Rendering;
	using namespace Rendering::Private::OpenGL;
	using namespace Rendering::Private::ProgramCompiler;

	namespace ResourceAssetParser
	{
		void CompiledProgramParser::Parse(const ByteBuffer& Buffer, CompiledProgramInfo& CompiledProgramInfo)
		{
			uint64 index = 0;

#define IMPLEMENT_PARSE(StageVariable) \
			CompiledProgramInfo.StageVariable.Size = Buffer.ReadValue<uint16>(index); \
			index += sizeof(uint16); \
			if (CompiledProgramInfo.StageVariable.Size != 0) \
			{ \
				PlatformMemory::Copy(Buffer.ReadValue(index, CompiledProgramInfo.StageVariable.Size), CompiledProgramInfo.StageVariable.Buffer, CompiledProgramInfo.StageVariable.Size); \
				index += CompiledProgramInfo.StageVariable.Size; \
			}

			IMPLEMENT_PARSE(VertexShader);
			IMPLEMENT_PARSE(GeometryShader);
			IMPLEMENT_PARSE(DomainShader);
			IMPLEMENT_PARSE(FragmentShader);
			IMPLEMENT_PARSE(ComputeShader);

#undef IMPLEMENT_PARSE
		}

		uint64 CompiledProgramParser::GetDumpSize(const CompiledProgramInfo& CompiledProgramInfo)
		{
			uint64 size = 0;

#define IMPLEMENT_DUMP_SIZE(StageVariable) \
			size += sizeof(uint16); \
			size += CompiledProgramInfo.StageVariable.Size;

			IMPLEMENT_DUMP_SIZE(VertexShader);
			IMPLEMENT_DUMP_SIZE(GeometryShader);
			IMPLEMENT_DUMP_SIZE(DomainShader);
			IMPLEMENT_DUMP_SIZE(FragmentShader);
			IMPLEMENT_DUMP_SIZE(ComputeShader);

#undef IMPLEMENT_DUMP_SIZE

			return size;
		}

		void CompiledProgramParser::Dump(ByteBuffer& Buffer, const CompiledProgramInfo& CompiledProgramInfo)
		{
#define IMPLEMENT_DUMP(StageVariable) \
			Buffer.Append(CompiledProgramInfo.StageVariable.Size); \
			if (CompiledProgramInfo.StageVariable.Size != 0) \
				Buffer.AppendBuffer(CompiledProgramInfo.StageVariable.Buffer, 0, CompiledProgramInfo.StageVariable.Size);

			IMPLEMENT_DUMP(VertexShader);
			IMPLEMENT_DUMP(GeometryShader);
			IMPLEMENT_DUMP(DomainShader);
			IMPLEMENT_DUMP(FragmentShader);
			IMPLEMENT_DUMP(ComputeShader);

#undef IMPLEMENT_DUMP
		}

		bool CompiledProgramParser::Compile(const ProgramInfo& Info, DeviceTypes* DeviceTypes, uint8 DeviceTypeCount, CompiledProgramInfo* CompiledInfos)
		{
			if (Info.Source.GetLength() == 0)
				return false;

			auto onError = [&](const String& Message, uint16 Line)
			{
				//CALL_CALLBACK(IListener, OnError, Message);
			};

			ProgramInfo info = {};
			info.Source =
				"struct INPUT_DATA { float3 pos : POSITION; float3 col : UV; };"
				"struct DATA { matrix4 _MVP;  matrix4 _View; float time; };"
				"DATA data;"
				"float4 VertexMain(INPUT_DATA InputData)"
				"{"
				"	return data._MVP * data._View * float4(InputData.pos, 1);"
				"}"
				"float4 FragmentMain(INPUT_DATA InputData)"
				"{"
				"	return float4(InputData.col, data.time);"
				"}";

			for (uint8 i = 0; i < DeviceTypeCount; ++i)
			{
				Compiler::MetaInfo metaInfo = {};
				Compiler::OutputInfo outputInfo = {};
				outputInfo.MetaInfo = &metaInfo;
				if (!Compiler::GetInstance()->Compile(DeviceTypes[i], &info, outputInfo, onError))
					return false;

				IDevice::Shaders shaders;
				shaders.VertexShader = outputInfo.VertexShader.GetValue();
				shaders.GeometryShader = outputInfo.GeometryShader.GetValue();
				shaders.DomainShader = outputInfo.DomainShader.GetValue();
				shaders.FragmentShader = outputInfo.FragmentShader.GetValue();
				shaders.ComputeShader = outputInfo.ComputeShader.GetValue();

				CompiledProgramInfo& compiledProgrm = CompiledInfos[i];

				IDevice::CompiledShaders compiledShaders = {};
				compiledShaders.VertexShader.Buffer = compiledProgrm.VertexShader.Buffer;
				compiledShaders.VertexShader.Size = compiledProgrm.VertexShader.Size;
				compiledShaders.GeometryShader.Buffer = compiledProgrm.GeometryShader.Buffer;
				compiledShaders.GeometryShader.Size = compiledProgrm.GeometryShader.Size;
				compiledShaders.DomainShader.Buffer = compiledProgrm.DomainShader.Buffer;
				compiledShaders.DomainShader.Size = compiledProgrm.DomainShader.Size;
				compiledShaders.FragmentShader.Buffer = compiledProgrm.FragmentShader.Buffer;
				compiledShaders.FragmentShader.Size = compiledProgrm.FragmentShader.Size;
				compiledShaders.ComputeShader.Buffer = compiledProgrm.ComputeShader.Buffer;
				compiledShaders.ComputeShader.Size = compiledProgrm.ComputeShader.Size;

				cstr message = nullptr;
				bool result = false;

				switch (DeviceTypes[i])
				{
				case DeviceTypes::OpenGL:
					result = OpenGLDevice::CompileProgramAPI(&shaders, &compiledShaders, &message);
					break;

				case DeviceTypes::DirectX12:
					result = DirectX12Device::CompileProgramAPI(&shaders, &compiledShaders, &message);
					break;

				case DeviceTypes::Vulkan:
					result = VulkanDevice::CompileProgramAPI(&shaders, &compiledShaders, &message);
					break;
				}

				if (!result)
				{
					//if (message != nullptr)
					//	CALL_CALLBACK(IListener, OnError, message);

					return false;
				}

				compiledProgrm.VertexShader.Size = compiledShaders.VertexShader.Size;
				compiledProgrm.GeometryShader.Size = compiledShaders.GeometryShader.Size;
				compiledProgrm.DomainShader.Size = compiledShaders.DomainShader.Size;
				compiledProgrm.FragmentShader.Size = compiledShaders.FragmentShader.Size;
				compiledProgrm.ComputeShader.Size = compiledShaders.ComputeShader.Size;
			}

			return true;
		}
	}
}