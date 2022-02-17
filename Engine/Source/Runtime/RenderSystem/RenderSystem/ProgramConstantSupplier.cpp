// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <RenderSystem\ProgramConstantSupplier.h>
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
			m_BufferConstants.Clear();
			m_TextureConstants.Clear();
		}

		void ProgramConstantSupplier::RegisterConstantBuffer(const String& Name, FetchBufferFunction Function)
		{
			m_BufferConstants[Pass::GetHash(Name)] = std::make_shared<FetchBufferFunction>(Function);
		}

		void ProgramConstantSupplier::RegisterTexture(const String& Name, FetchTexturetFunction Function)
		{
			m_TextureConstants[Pass::GetHash(Name)] = std::make_shared<FetchTexturetFunction>(Function);
		}

		void ProgramConstantSupplier::SupplyConstants(GPUBufferDataBaseMap& Buffers, ProgramConstantHolder::TextureDataBaseMap& Textures) const
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

			IMPLEMENT_ITERATION(Buffers, m_BufferConstants)
				constant.Value->Copy(value);
			END_OF_IMPLEMENT();

			IMPLEMENT_ITERATION(Textures, m_TextureConstants)
				constant.Value = ConstCast(TextureResource*, value);
			END_OF_IMPLEMENT();

#undef END_OF_IMPLEMENT
#undef IMPLEMENT_ITERATION
		}
	}
}