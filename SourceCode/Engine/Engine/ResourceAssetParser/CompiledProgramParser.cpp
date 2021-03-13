// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\CompiledProgramParser.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Rendering;

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
			IMPLEMENT_PARSE(TessellationShader);
			IMPLEMENT_PARSE(GeometryShader);
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
			IMPLEMENT_DUMP_SIZE(TessellationShader);
			IMPLEMENT_DUMP_SIZE(GeometryShader);
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
			IMPLEMENT_DUMP(TessellationShader);
			IMPLEMENT_DUMP(GeometryShader);
			IMPLEMENT_DUMP(FragmentShader);
			IMPLEMENT_DUMP(ComputeShader);

			//CompiledProgramInfo.MetaInfo.

#undef IMPLEMENT_DUMP
		}
	}
}