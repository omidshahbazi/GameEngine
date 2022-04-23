// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <CompiledProgramAssetParser\CompiledProgramParser.h>

namespace Engine
{
	using namespace Common;
	using namespace Containers;

	namespace ResourceAssetParser
	{
		void CompiledProgramParser::Parse(const ByteBuffer& Buffer, String& Source)
		{
			uint32 length = Buffer.ReadValue<uint32>();

			Source = String(ReinterpretCast(const String::CharType*, Buffer.ReadValue(length)), length);
		}

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
			IMPLEMENT_PARSE(HullShader);
			IMPLEMENT_PARSE(DomainShader);
			IMPLEMENT_PARSE(GeometryShader);
			IMPLEMENT_PARSE(FragmentShader);
			IMPLEMENT_PARSE(ComputeShader);

			uint64 count = Buffer.ReadValue<uint32>();
			CompiledProgramInfo.MetaInfo.Structs.Recap(count);
			for (uint8 i = 0; i < count; ++i)
			{
				StructMetaInfo structInfo = {};

				uint32 nameLength = Buffer.ReadValue<uint32>();
				structInfo.Name = String(ReinterpretCast(cstr, Buffer.ReadValue(nameLength)), nameLength);

				uint64 variableCount = Buffer.ReadValue<uint32>();
				structInfo.Variables.Recap(variableCount);
				for (uint8 j = 0; j < variableCount; ++j)
				{
					StructMetaInfo::VariableMetaInfo variableInfo = {};

					variableInfo.DataType = (ProgramDataTypes)Buffer.ReadValue<int32>();

					nameLength = Buffer.ReadValue<uint32>();
					variableInfo.Name = String(ReinterpretCast(cstr, Buffer.ReadValue(nameLength)), nameLength);

					structInfo.Variables.Add(variableInfo);
				}

				structInfo.Size = Buffer.ReadValue<uint16>();

				CompiledProgramInfo.MetaInfo.Structs.Add(structInfo);
			}

			count = Buffer.ReadValue<uint32>();
			CompiledProgramInfo.MetaInfo.Variables.Recap(count);
			for (uint8 i = 0; i < count; ++i)
			{
				VariableMetaInfo variableInfo = {};

				variableInfo.Handle = Buffer.ReadValue<uint8>();
				variableInfo.DataType = (ProgramDataTypes)Buffer.ReadValue<int32>();

				uint32 nameLength = Buffer.ReadValue<uint32>();
				variableInfo.UserDefinedType = String(ReinterpretCast(cstr, Buffer.ReadValue(nameLength)), nameLength);

				nameLength = Buffer.ReadValue<uint32>();
				variableInfo.Name = String(ReinterpretCast(cstr, Buffer.ReadValue(nameLength)), nameLength);

				CompiledProgramInfo.MetaInfo.Variables.Add(variableInfo);
			}

#undef IMPLEMENT_PARSE
		}

		uint64 CompiledProgramParser::GetDumpSize(const String& Source)
		{
			uint64 size = 0;

			size += sizeof(uint32);
			size += Source.GetLength();

			return size;
		}

		uint64 CompiledProgramParser::GetDumpSize(const CompiledProgramInfo& CompiledProgramInfo)
		{
			uint64 size = 0;
			size += sizeof(int32);

#define IMPLEMENT_DUMP_SIZE(StageVariable) \
			size += sizeof(uint16); \
			size += CompiledProgramInfo.StageVariable.Size;

			IMPLEMENT_DUMP_SIZE(VertexShader);
			IMPLEMENT_DUMP_SIZE(HullShader);
			IMPLEMENT_DUMP_SIZE(DomainShader);
			IMPLEMENT_DUMP_SIZE(GeometryShader);
			IMPLEMENT_DUMP_SIZE(FragmentShader);
			IMPLEMENT_DUMP_SIZE(ComputeShader);

			size += sizeof(uint32);
			for (auto structInfo : CompiledProgramInfo.MetaInfo.Structs)
			{
				size += sizeof(uint32);
				size += structInfo.Name.GetLength();

				size += sizeof(uint32);
				for (auto variableInfo : structInfo.Variables)
				{
					size += sizeof(int32);

					size += sizeof(uint32);
					size += variableInfo.Name.GetLength();
				}

				size += sizeof(uint16);
			}

			size += sizeof(uint32);
			for (auto variableInfo : CompiledProgramInfo.MetaInfo.Variables)
			{
				size += sizeof(uint8);
				size += sizeof(int32);

				size += sizeof(uint32);
				size += variableInfo.UserDefinedType.GetLength();

				size += sizeof(uint32);
				size += variableInfo.Name.GetLength();
			}

#undef IMPLEMENT_DUMP_SIZE

			return size;
		}

		void CompiledProgramParser::Dump(ByteBuffer& Buffer, const String& Source)
		{
			Buffer.Append(Source.GetLength());

			Buffer.AppendBuffer(ReinterpretCast(const byte*, Source.GetValue()), 0, Source.GetLength());
		}

		void CompiledProgramParser::Dump(ByteBuffer& Buffer, DeviceTypes DeviceType, const CompiledProgramInfo& CompiledProgramInfo)
		{
			Buffer.Append((int32)DeviceType);

#define IMPLEMENT_DUMP(StageVariable) \
			Buffer.Append((uint16)CompiledProgramInfo.StageVariable.Size); \
			if (CompiledProgramInfo.StageVariable.Size != 0) \
				Buffer.AppendBuffer(CompiledProgramInfo.StageVariable.Buffer, 0, CompiledProgramInfo.StageVariable.Size);

			IMPLEMENT_DUMP(VertexShader);
			IMPLEMENT_DUMP(HullShader);
			IMPLEMENT_DUMP(DomainShader);
			IMPLEMENT_DUMP(GeometryShader);
			IMPLEMENT_DUMP(FragmentShader);
			IMPLEMENT_DUMP(ComputeShader);

			Buffer.Append(CompiledProgramInfo.MetaInfo.Structs.GetSize());
			for (auto structInfo : CompiledProgramInfo.MetaInfo.Structs)
			{
				Buffer.Append(structInfo.Name.GetLength());
				Buffer << structInfo.Name;

				Buffer.Append(structInfo.Variables.GetSize());
				for (auto variableInfo : structInfo.Variables)
				{
					Buffer.Append((int32)variableInfo.DataType);

					Buffer.Append(variableInfo.Name.GetLength());
					Buffer << variableInfo.Name;
				}

				Buffer.Append(structInfo.Size);
			}

			Buffer.Append(CompiledProgramInfo.MetaInfo.Variables.GetSize());
			for (auto variableInfo : CompiledProgramInfo.MetaInfo.Variables)
			{
				Buffer.Append(variableInfo.Handle);
				Buffer.Append((int32)variableInfo.DataType);

				Buffer.Append(variableInfo.UserDefinedType.GetLength());
				Buffer << variableInfo.UserDefinedType;

				Buffer.Append(variableInfo.Name.GetLength());
				Buffer << variableInfo.Name;
			}

#undef IMPLEMENT_DUMP
		}
	}
}