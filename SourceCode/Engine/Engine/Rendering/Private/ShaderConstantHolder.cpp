// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Private\ShaderConstantHolder.h>
#include <MemoryManagement\Allocator\RootAllocator.h>
#include <Utility\Hash.h>

namespace Engine
{
	using namespace Utility;

	namespace Rendering
	{
		namespace Private
		{
			ShaderConstantHolder::ConstantInfo::ConstantInfo(const String& Name, const AnyDataType& Value) :
				Hash(GetHash(Name)),
				Name(Name),
				Value(Value)
			{
			}

			ShaderConstantHolder::ConstantHash ShaderConstantHolder::GetHash(const String& Name)
			{
				return Hash::CRC32(Name.GetValue(), Name.GetLength());
			}
		}
	}
}