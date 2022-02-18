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

		void ProgramConstantSupplier::SupplyConstants(GPUBufferDataMap& Buffers, TextureDataMap& Textures) const
		{
#define IMPLEMENT_ITERATION(Map, SupplierMap) \
			for (auto& item : Map) \
			{ \
				auto hash = item.GetFirst(); \
				auto& constant = item.GetSecond(); \
				if (!SupplierMap.Contains(hash)) \
					continue; \
				auto value = (*SupplierMap[hash])(); \
				if (value == nullptr) \
					continue; \

#define END_OF_IMPLEMENT() \
			}

			IMPLEMENT_ITERATION(Buffers, m_FetchBufferConstants)
				constant.Value->Copy(value);
			END_OF_IMPLEMENT();

			IMPLEMENT_ITERATION(Textures, m_FetchTextureConstants)
				constant.Value = ConstCast(TextureResource*, value);
			END_OF_IMPLEMENT();

#undef END_OF_IMPLEMENT
#undef IMPLEMENT_ITERATION
		}
	}
}