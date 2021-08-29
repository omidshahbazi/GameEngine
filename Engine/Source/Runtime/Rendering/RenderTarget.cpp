// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\RenderTarget.h>
#include <Rendering\Private\ThreadedDevice.h>

namespace Engine
{
	namespace Rendering
	{
		RenderTarget::RenderTarget(ThreadedDevice*Device, Handle Handle, const TexturesList &Textures) :
			NativeType(Device, Handle),
			m_Textures(Textures)
		{
		}

		void RenderTarget::SetName(const WString& Name)
		{
			NativeType::SetName(Name);

			GetDevice()->SetResourceName(GetHandle(), IDevice::ResourceTypes::RenderTarget, GetName().GetValue());
		}
	}
}