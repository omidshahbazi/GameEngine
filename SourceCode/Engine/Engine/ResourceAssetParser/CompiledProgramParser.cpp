// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\CompiledProgramParser.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Rendering;

	namespace ResourceAssetParser
	{
		void CompiledProgramParser::Parse(const ByteBuffer& Buffer, DeviceTypes& DeviceType, CompiledProgramInfo& CompiledProgramInfo)
		{
			DeviceType = (DeviceTypes)Buffer.ReadValue<int32>();

#define IMPLEMENT_PARSE(StageVariable) \
			{ \
				uint16 size = Buffer.ReadValue<uint16>(); \
				if (CompiledProgramInfo.StageVariable.Size < size) \
					return; \
				CompiledProgramInfo.StageVariable.Size = size; \
				if (size != 0) \
				{ \
					PlatformMemory::Copy(Buffer.ReadValue(size), CompiledProgramInfo.StageVariable.Buffer, size); \
				} \
			}

			IMPLEMENT_PARSE(VertexShader);
			IMPLEMENT_PARSE(TessellationShader);
			IMPLEMENT_PARSE(GeometryShader);
			IMPLEMENT_PARSE(FragmentShader);
			IMPLEMENT_PARSE(ComputeShader);

#undef IMPLEMENT_PARSE
		}

		uint64 CompiledProgramParser::GetDumpSize(const CompiledProgramInfo& CompiledProgramInfo)
		{
			uint64 size = 0;
			size += sizeof(int32);

#define IMPLEMENT_DUMP_SIZE(StageVariable) \
			size += sizeof(uint16); \
			size += CompiledProgramInfo.StageVariable.Size;

			IMPLEMENT_DUMP_SIZE(VertexShader);
			IMPLEMENT_DUMP_SIZE(TessellationShader);
			IMPLEMENT_DUMP_SIZE(GeometryShader);
			IMPLEMENT_DUMP_SIZE(FragmentShader);
			IMPLEMENT_DUMP_SIZE(ComputeShader);

#undef IMPLEMENT_DUMP_SIZE

			return size;
		}

		void CompiledProgramParser::Dump(ByteBuffer& Buffer, DeviceTypes DeviceType, const CompiledProgramInfo& CompiledProgramInfo)
		{
			Buffer.Append((int32)DeviceType);

#define IMPLEMENT_DUMP(StageVariable) \
			Buffer.Append((uint16)CompiledProgramInfo.StageVariable.Size); \
			if (CompiledProgramInfo.StageVariable.Size != 0) \
				Buffer.AppendBuffer(CompiledProgramInfo.StageVariable.Buffer, 0, CompiledProgramInfo.StageVariable.Size);

			IMPLEMENT_DUMP(VertexShader);
			IMPLEMENT_DUMP(TessellationShader);
			IMPLEMENT_DUMP(GeometryShader);
			IMPLEMENT_DUMP(FragmentShader);
			IMPLEMENT_DUMP(ComputeShader);

			//CompiledProgramInfo.MetaInfo.Variables[0].

#undef IMPLEMENT_DUMP
		}
	}
}