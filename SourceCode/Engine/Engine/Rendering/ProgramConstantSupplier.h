// Copyright 2016-2020 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_CONSTANT_SUPPLIER_H
#define PROGRAM_CONSTANT_SUPPLIER_H

#include <MemoryManagement\Singleton.h>
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
		class CPUConstantBuffer;
		class Program;

		using namespace Private;
		using namespace Private::Commands;

		class RENDERING_API ProgramConstantSupplier
		{
			SINGLETON_DECLARATION(ProgramConstantSupplier);

			friend class DrawCommand;

		public:
			typedef std::function<const CPUConstantBuffer* (void)> FetchBufferFunction;
			typedef std::function<const TextureResource* (void)> FetchTexturetFunction;

		private:
			typedef Map<String, std::shared_ptr<FetchBufferFunction>> BufferConstantMap;
			typedef Map<String, std::shared_ptr<FetchTexturetFunction>> TextureConstantMap;

		private:
			ProgramConstantSupplier(void)
			{
			}

		public:
			void RegisterBufferConstant(const String& Name, FetchBufferFunction Function);
			void RegisterTextureConstant(const String& Name, FetchTexturetFunction Function);

		private:
			void SupplyConstants(IDevice* Device, Program* Program) const;

		private:
			BufferConstantMap m_BufferConstants;
			TextureConstantMap m_TextureConstants;
		};
	}
}

#endif