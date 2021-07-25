// Copyright 2016-2020 ?????????????. All Rights Reserved.
#include <Rendering\ProgramConstantSupplier.h>
#include <Rendering\IDevice.h>
#include <Rendering\ConstantBuffer.h>

#include <Rendering\Private\OpenGL\OpenGLDevice.h>
using namespace Engine::Rendering::Private::OpenGL;

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		SINGLETON_DEFINITION(ProgramConstantSupplier);

		ProgramConstantSupplier::~ProgramConstantSupplier(void)
		{
			m_BufferConstants.Clear();
			m_TextureConstants.Clear();
		}

		void ProgramConstantSupplier::RegisterBufferConstant(const String& Name, FetchBufferFunction Function)
		{
			m_BufferConstants[Program::GetHash(Name)] = std::make_shared<FetchBufferFunction>(Function);
		}

		void ProgramConstantSupplier::RegisterTextureConstant(const String& Name, FetchTexturetFunction Function)
		{
			m_TextureConstants[Program::GetHash(Name)] = std::make_shared<FetchTexturetFunction>(Function);
		}

		void ProgramConstantSupplier::SupplyConstants(ProgramConstantHolder::BufferDataBaseMap& Buffers, ProgramConstantHolder::TextureDataBaseMap& Textures) const
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