// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\CompiledProgramParser.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Rendering;

	namespace ResourceAssetParser
	{
		void CompiledProgramParser::Parse(const ByteBuffer& Buffer, ProgramInfo& ProgramInfo)
		{
			ProgramInfo.Source = String(ReinterpretCast(const String::CharType*, Buffer.GetBuffer()), Buffer.GetSize());
		}

		uint64 CompiledProgramParser::GetDumpSize(const ProgramInfo& ProgramInfo)
		{
			return ProgramInfo.Source.GetLength() + 1;
		}

		void CompiledProgramParser::Dump(ByteBuffer& Buffer, const ProgramInfo& ProgramInfo)
		{
			Buffer.AppendBuffer(ReinterpretCast(const byte*, ProgramInfo.Source.GetValue()), 0, GetDumpSize(ProgramInfo));
		}
	}
}