// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\RenderTarget.h>
#include <RenderSystem\Private\ThreadedDevice.h>

namespace Engine
{
	namespace RenderSystem
	{
		RenderTarget::RenderTarget(ThreadedDevice* Device, ResourceHandle Handle, const TexturesList& Textures) :
			NativeType(Device, Handle),
			m_Textures(Textures)
		{
			SetName("RenderTarget");
		}

		void RenderTarget::SetNameInternal(const WString& Name)
		{
			GetDevice()->SetResourceName(GetHandle(), IDevice::ResourceTypes::RenderTarget, GetName().GetValue());
		}
	}
}