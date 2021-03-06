// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <ResourceAssetParser\TextParser.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;
	using namespace ResourceSystem;

	namespace ResourceAssetParser
	{
		void TextParser::Parse(const ByteBuffer& Buffer, TextInfo& TextInfo)
		{
			TextInfo.Value = WString(ReinterpretCast(const WString::CharType*, Buffer.GetBuffer()), Buffer.GetSize() / sizeof(WString::CharType));
		}

		uint64 TextParser::GetDumpSize(const TextInfo& TextInfo)
		{
			return (TextInfo.Value.GetLength() + 1) * sizeof(WString::CharType);
		}

		void TextParser::Dump(ByteBuffer& Buffer, const TextInfo& TextInfo)
		{
			Buffer.AppendBuffer(ReinterpretCast(const byte*, TextInfo.Value.GetValue()), 0, GetDumpSize(TextInfo));
		}
	}
}