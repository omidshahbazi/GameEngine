// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ProgramConstantHolder.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Utility\Hash.h>

namespace Engine
{
	using namespace Utility;

	namespace Rendering
	{
		namespace Private
		{
			ProgramConstantHolder::ConstantInfo::ConstantInfo(const String& Name, const AnyDataType& Value) :
				Hash(GetHash(Name)),
				Name(Name),
				Value(Value)
			{
			}

			ProgramConstantHolder::ConstantHash ProgramConstantHolder::GetHash(const String& Name)
			{
				return Hash::CRC32(Name.GetValue(), Name.GetLength());
			}
		}
	}
}