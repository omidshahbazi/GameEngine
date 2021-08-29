// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\Program.h>
#include <Rendering\Private\ThreadedDevice.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace Rendering
	{
		Program::Program(ThreadedDevice* Device, Handle Handle, const MetaInfo& MetaInfo) :
			NativeType(Device, Handle),
			m_MetaInfo(MetaInfo)
		{
			GenerateConstantData();
		}

		void Program::SetName(const WString& Name)
		{
			NativeType::SetName(Name);

			GetDevice()->SetResourceName(GetHandle(), IDevice::ResourceTypes::Program, GetName().GetValue());
		}

		const StructMetaInfo* Program::GetStructInfoOf(const String& VariableName) const
		{
			ConstantHash hash = GetHash(VariableName);

			int32 index = m_MetaInfo.Variables.FindIf([hash](auto& item) { return GetHash(item.Name) == hash; });
			if (index == -1)
				return nullptr;

			const VariableMetaInfo& variableInfo = m_MetaInfo.Variables[index];

			index = m_MetaInfo.Structs.FindIf([&variableInfo](auto& item) { return item.Name == variableInfo.UserDefinedType; });
			if (index == -1)
				return nullptr;

			return &m_MetaInfo.Structs[index];
		}

		void Program::GenerateConstantData(void)
		{
			for (auto& constant : m_MetaInfo.Variables)
			{
				if (constant.DataType == ProgramDataTypes::Unknown)
				{
					const StructMetaInfo* structInfo = GetStructInfoOf(constant.Name);
					if (structInfo == nullptr)
						return;

					CreateBufferData(constant.Handle, constant.Name, constant.UserDefinedType, structInfo->Size);

					continue;
				}

				CreateTextureData(constant.Handle, constant.Name);
			}
		}
	}
}