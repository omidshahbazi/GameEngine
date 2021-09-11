// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\Material.h>

namespace Engine
{
	using namespace Platform;

	namespace RenderSystem
	{
		using namespace Private;

		Material::Material(void)
		{
		}

		Material::Material(const Material& Other)
		{
			*this = Other;
		}

		void Material::SetQueue(RenderQueues Queue)
		{
			for (auto& pass : m_Passes)
				pass.SetQueue(Queue);
		}

		bool Material::SetBuffer(Pass::ConstantHash Hash, const byte* Data, uint16 Size)
		{
			for (auto& pass : m_Passes)
				if (!pass.SetBuffer(Hash, Data, Size))
					return false;

			return true;
		}

		bool Material::SetTexture(Pass::ConstantHash Hash, const TextureResource* Value)
		{
			for (auto& pass : m_Passes)
				if (!pass.SetTexture(Hash, Value))
					return false;

			return true;
		}

		bool Material::SetSprite(Pass::ConstantHash Hash, const SpriteResource* Value)
		{
			for (auto& pass : m_Passes)
				if (!pass.SetSprite(Hash, Value))
					return false;

			return true;
		}

		void Material::SetRenderState(const IDevice::State& State)
		{
			for (auto& pass : m_Passes)
				pass.SetRenderState(State);
		}
	}
}