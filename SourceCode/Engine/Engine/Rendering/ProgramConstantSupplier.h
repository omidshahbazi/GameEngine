// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_CONSTANT_SUPPLIER_H
#define PROGRAM_CONSTANT_SUPPLIER_H

#include <MemoryManagement\Singleton.h>
#include <Rendering\Private\ProgramConstantHolder.h>
#include <ResourceSystem\Resource.h>
#include <Containers\Strings.h>
#include <Containers\Map.h>
#include <functional>
#include <memory>

namespace Engine
{
	using namespace ResourceSystem;
	using namespace Containers;

	namespace Rendering
	{
		namespace Private
		{
			namespace Commands
			{
				class DrawCommand;
			}
		}

		class IDevice;
		class ConstantBuffer;
		class Program;

		using namespace Private;
		using namespace Private::Commands;

		class RENDERING_API ProgramConstantSupplier
		{
			SINGLETON_DECLARATION(ProgramConstantSupplier);

			friend class DrawCommand;

		public:
			typedef std::function<const ConstantBuffer* (void)> FetchBufferFunction;
			typedef std::function<const TextureResource* (void)> FetchTexturetFunction;

		private:
			typedef Map<ProgramConstantHolder::ConstantHash, std::shared_ptr<FetchBufferFunction>> BufferConstantMap;
			typedef Map<ProgramConstantHolder::ConstantHash, std::shared_ptr<FetchTexturetFunction>> TextureConstantMap;

		private:
			ProgramConstantSupplier(void)
			{
			}

			~ProgramConstantSupplier(void);

		public:
			void RegisterBufferConstant(const String& Name, FetchBufferFunction Function);
			void RegisterTextureConstant(const String& Name, FetchTexturetFunction Function);

		private:
			void SupplyConstants(IDevice* Device, ProgramConstantHolder::BufferDataBaseMap& Buffers, ProgramConstantHolder::TextureDataBaseMap& Textures) const;

		private:
			BufferConstantMap m_BufferConstants;
			TextureConstantMap m_TextureConstants;
		};
	}
}

#endif