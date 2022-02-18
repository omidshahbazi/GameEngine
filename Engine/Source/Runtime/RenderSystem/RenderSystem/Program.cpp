// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Program.h>
#include <RenderSystem\Material.h>
#include <RenderSystem\Private\ThreadedDevice.h>

namespace Engine
{
	using namespace ResourceSystem;

	namespace RenderSystem
	{
		using namespace Private;

		Program::Program(ThreadedDevice* Device, ResourceHandle Handle, const MetaInfo& MetaInfo) :
			NativeType(Device, Handle),
			m_MetaInfo(MetaInfo)
		{
			SetName("Program");
		}

		void Program::SetNameInternal(const WString& Name)
		{
			GetDevice()->SetResourceName(GetHandle(), IDevice::ResourceTypes::Program, GetName().GetValue());
		}
	}
}