// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_CONSTANT_SUPPLIER_H
#define PROGRAM_CONSTANT_SUPPLIER_H

#include <MemoryManagement\Singleton.h>
#include <RenderSystem\Private\ProgramConstantHolder.h>
#include <ResourceCommon\Resource.h>
#include <Containers\Strings.h>
#include <Containers\Map.h>
#include <functional>
#include <memory>

namespace Engine
{
	using namespace ResourceCommon;
	using namespace Containers;

	namespace RenderSystem
	{
		namespace Private
		{
			namespace Commands
			{
				class GPUConstantBuffer;
				class DrawCommand;
			}
		}

		class ConstantBuffer;
		class Program;

		using namespace Private;
		using namespace Private::Commands;

		class RENDERSYSTEM_API ProgramConstantSupplier
		{
			SINGLETON_DECLARATION(ProgramConstantSupplier);

			friend class DrawCommand;

		public:
			typedef std::function<const ConstantBuffer* (void)> FetchBufferFunction;
			typedef std::function<const TextureResource* (void)> FetchTexturetFunction;

		private:
			typedef Map<ProgramConstantHolder::ConstantHash, std::shared_ptr<FetchBufferFunction>> BufferConstantMap;
			typedef Map<ProgramConstantHolder::ConstantHash, std::shared_ptr<FetchTexturetFunction>> TextureConstantMap;
			typedef ProgramConstantHolder::ConstantDataBase<GPUConstantBuffer*> GPUBufferConstantDataBase;
			typedef Map<ProgramConstantHolder::ConstantHash, GPUBufferConstantDataBase> GPUBufferDataBaseMap;

		private:
			ProgramConstantSupplier(void)
			{
			}

			~ProgramConstantSupplier(void);

		public:
			void RegisterBufferConstant(const String& Name, FetchBufferFunction Function);
			void RegisterTextureConstant(const String& Name, FetchTexturetFunction Function);

		private:
			void SupplyConstants(GPUBufferDataBaseMap& Buffers, ProgramConstantHolder::TextureDataBaseMap& Textures) const;

		private:
			BufferConstantMap m_BufferConstants;
			TextureConstantMap m_TextureConstants;
		};
	}
}

#endif