// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <HLSLCompiler\Private\HLSLCompiler.h>
#include <Common\CharacterUtility.h>
#include <Platform\PlatformMemory.h>
#include <Containers\Exception.h>
#include <d3dcompiler.h>

namespace Engine
{
	using namespace Common;
	using namespace Platform;
	using namespace Containers;

	namespace HLSLCompiler
	{
		namespace Private
		{
			void HLSLCompiler::Compile(DeviceTypes Client, Stages Stage, cstr Source, cstr EntryPointName, bool DebugMode, byte* ByteCode, uint16& Size)
			{
				switch (Client)
				{
				case DeviceTypes::DirectX12:
					break;

				default:
					THROW_FULL_EXCEPTION(Categories::ProgramCompiler, "Compile HLSL shader has failed", "Unsupported Client has provided");
				}

				char8 target[7];
				target[2] = '_';
				target[3] = '5';
				target[4] = '_';
				target[5] = '1';
				target[6] = '\0';

				switch (Stage)
				{
				case Stages::Vertex:
					target[0] = 'v';
					target[1] = 's';
					break;

				case Stages::Tessellation:
					target[0] = 'd';
					target[1] = 's';
					break;

				case Stages::Geometry:
					target[0] = 'g';
					target[1] = 's';
					break;

				case Stages::Fragment:
					target[0] = 'p';
					target[1] = 's';
					break;

				case Stages::Compute:
					target[0] = 'c';
					target[1] = 's';
					break;

				default:
					THROW_FULL_EXCEPTION(Categories::ProgramCompiler, "Compile HLSL shader has failed", "Unknown stage has provided");
				}

				ID3DBlob* dataBlob = nullptr;
				ID3DBlob* messageBlob = nullptr;

				uint32 flags = 0;

				if (DebugMode)
					flags |= D3DCOMPILE_DEBUG;

				if (SUCCEEDED(D3DCompile2(Source, CharacterUtility::GetLength(Source), nullptr, nullptr, nullptr, EntryPointName, target, flags, 0, 0, nullptr, 0, &dataBlob, &messageBlob)))
				{
					PlatformMemory::Copy(ReinterpretCast(byte*, dataBlob->GetBufferPointer()), ByteCode, dataBlob->GetBufferSize());
					Size = dataBlob->GetBufferSize();

					return;
				}

				THROW_FULL_EXCEPTION(Categories::ProgramCompiler, "Compile HLSL shader has failed", ReinterpretCast(cstr, messageBlob->GetBufferPointer()));
			}
		}
	}
}