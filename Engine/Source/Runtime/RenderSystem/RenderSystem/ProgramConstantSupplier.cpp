// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\ProgramConstantSupplier.h>
#include <RenderSystem\Material.h>
#include <RenderDevice\IDevice.h>
#include <RenderSystem\Private\GPUConstantBuffer.h>

namespace Engine
{
	using namespace Containers;

	namespace RenderSystem
	{
		SINGLETON_DEFINITION(ProgramConstantSupplier);

		ProgramConstantSupplier::~ProgramConstantSupplier(void)
		{
			m_FetchBufferConstants.Clear();
			m_FetchTextureConstants.Clear();
		}

		void ProgramConstantSupplier::RegisterConstantBuffer(const String& Name, FetchBufferFunction Function)
		{
			m_FetchBufferConstants[Material::GetHash(Name)] = std::make_shared<FetchBufferFunction>(Function);
		}

		void ProgramConstantSupplier::RegisterTexture(const String& Name, FetchTexturetFunction Function)
		{
			m_FetchTextureConstants[Material::GetHash(Name)] = std::make_shared<FetchTexturetFunction>(Function);
		}

		void ProgramConstantSupplier::SupplyConstants(ProgramInstance& ProgramInstance) const
		{
			const auto& handles = ProgramInstance.GetHandles();
			auto& buffer = ProgramInstance.GetBuffers();
			auto& textures = ProgramInstance.GetTextures();

			for (auto& item : handles)
			{
				auto hash = item.GetFirst();

				if (m_FetchBufferConstants.Contains(hash))
				{
					auto value = (*m_FetchBufferConstants[hash])();
					if (value == nullptr)
						continue;
					
					ProgramInstance.SetBuffer(hash, value);
				}
				else if (m_FetchTextureConstants.Contains(hash))
				{
					auto value = (*m_FetchTextureConstants[hash])();
					if (value == nullptr)
						continue;

					ProgramInstance.SetTexture(hash, value);
				}
			}
		}
	}
}