// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_CONSTANT_SUPPLIER_H
#define PROGRAM_CONSTANT_SUPPLIER_H

#include <MemoryManagement\Singleton.h>
#include <RenderSystem\Private\ConstantData.h>
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
			class ProgramInstance;
			class GPUConstantBuffer;
		}

		class ConstantBuffer;
		class Program;
		class DrawCommand;

		using namespace Private;

		class RENDERSYSTEM_API ProgramConstantSupplier
		{
			SINGLETON_DECLARATION(ProgramConstantSupplier);

			friend class CommandBuffer;

		public:
			typedef std::function<const ConstantBuffer* (void)> FetchBufferFunction;
			typedef std::function<const TextureResource* (void)> FetchTexturetFunction;

		private:
			typedef Map<ProgramConstantHash, std::shared_ptr<FetchBufferFunction>> FetchBufferConstantMap;
			typedef Map<ProgramConstantHash, std::shared_ptr<FetchTexturetFunction>> FetchTextureConstantMap;
			typedef ProgramBufferMap<GPUConstantBuffer> ProgramGPUConstantBufferMap;

		private:
			ProgramConstantSupplier(void)
			{
			}

			~ProgramConstantSupplier(void);

		public:
			void RegisterConstantBuffer(const String& Name, FetchBufferFunction Function);
			void RegisterTexture(const String& Name, FetchTexturetFunction Function);

		private:
			void SupplyConstants(ProgramInstance& ProgramInstance) const;

		private:
			FetchBufferConstantMap m_FetchBufferConstants;
			FetchTextureConstantMap m_FetchTextureConstants;
		};
	}
}

#endif