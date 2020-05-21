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
		void ShaderParser::Parse(const ByteBuffer& Buffer, ShaderInfo& TextInfo)
		{
			TextInfo.Source = String(ReinterpretCast(const String::CharType*, Buffer.GetBuffer()), Buffer.GetSize());
		}

		uint64 ShaderParser::GetDumpSize(const ShaderInfo& TextInfo)
		{
			return TextInfo.Source.GetLength() + 1;
		}

		void ShaderParser::Dump(ByteBuffer& Buffer, const ShaderInfo& TextInfo)
		{
			Buffer.AppendBuffer(ReinterpretCast(const byte*, TextInfo.Source.GetValue()), 0, GetDumpSize(TextInfo));
		}
	}
}