// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\ShaderParser.h>
#include <ResourceAssetParser\ShaderParser.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace Rendering;

	namespace ResourceAssetParser
	{
		void ShaderParser::Parse(const ByteBuffer& Buffer, ShaderInfo& ShaderInfo)
		{
			ShaderInfo.Source = String(ReinterpretCast(const String::CharType*, Buffer.GetBuffer()), Buffer.GetSize());
		}

		uint64 ShaderParser::GetDumpSize(const ShaderInfo& ShaderInfo)
		{
			return ShaderInfo.Source.GetLength() + 1;
		}

		void ShaderParser::Dump(ByteBuffer& Buffer, const ShaderInfo& ShaderInfo)
		{
			Buffer.AppendBuffer(ReinterpretCast(const byte*, ShaderInfo.Source.GetValue()), 0, GetDumpSize(ShaderInfo));
		}
	}
}