// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\ProgramConstantSupplier.h>
#include <Rendering\Program.h>
#include <Rendering\CPUConstantBuffer.h>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		SINGLETON_DEFINITION(ProgramConstantSupplier);

		void ProgramConstantSupplier::RegisterBufferConstant(const String& Name, FetchBufferFunction Function)
		{
			m_BufferConstants[Name] = std::make_shared<FetchBufferFunction>(Function);
		}

		void ProgramConstantSupplier::RegisterTextureConstant(const String& Name, FetchTexturetFunction Function)
		{
			m_TextureConstants[Name] = std::make_shared<FetchTexturetFunction>(Function);
		}

		void ProgramConstantSupplier::SupplyConstants(Program* Program) const
		{
#define IMPLEMENT_ITERATION(Map, SupplierMap) \
			for (auto& item : Map) \
			{ \
				auto& constant = item.GetSecond(); \
				if (!SupplierMap.Contains(constant.Name)) \
					continue; \
				auto value = (*SupplierMap[constant.Name])(); \
				if (value == nullptr) \
					continue; \

#define END_OF_IMPLEMENT() \
			}

			auto& bufferConstants = Program->GetBuffers();
			IMPLEMENT_ITERATION(bufferConstants, m_BufferConstants)
				constant.Value->Lock();
				constant.Value->Set(ConstCast(CPUConstantBuffer&, *value));
				constant.Value->Unlock();
			END_OF_IMPLEMENT();

			auto& texureConstants = Program->GetTextures();
			IMPLEMENT_ITERATION(texureConstants, m_TextureConstants)
				constant.Value = ConstCast(TextureResource*, value);
			END_OF_IMPLEMENT();

#undef END_OF_IMPLEMENT
#undef IMPLEMENT_ITERATION
		}
	}
}