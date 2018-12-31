// Copyright 2016-2017 ?????????????. All Rights Reserved.
#pragma once
#ifndef PROGRAM_CONSTANT_SUPPLIER_H
#define PROGRAM_CONSTANT_SUPPLIER_H

#include <MemoryManagement\Singleton.h>
#include <Rendering\Private\ShaderCompiler\DataTypes.h>
#include <Containers\AnyDataType.h>
#include <Containers\Strings.h>
#include <Containers\Map.h>
#include <functional>
#include <memory>

namespace Engine
{
	using namespace Containers;

	namespace Rendering
	{
		class IDevice;
		class Program;

		using namespace Private::ShaderCompiler;

		class RENDERING_API ProgramConstantSupplier
		{
			SINGLETON_DECLARATION(ProgramConstantSupplier)
			
			friend class DeviceInterface;

		public:
			typedef std::function<const AnyDataType &(void)> FetchConstantFunction;
			typedef std::shared_ptr<FetchConstantFunction> FetchConstantFunctionFunctionPtr;

		private:
			struct ConstantSupplierInfo
			{
			public:
				DataTypes DataType;
				FetchConstantFunctionFunctionPtr Function;
			};

			typedef Map<String, ConstantSupplierInfo> InfoMap;

		private:
			ProgramConstantSupplier(void);

			void SupplyConstants(IDevice *Device, Program *Program) const;

		private:
			InfoMap m_Infos;
		};
	}
}

#endif